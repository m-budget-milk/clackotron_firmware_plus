#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include "ct_webserver_async.h"
#include "ct_logging.h"
#include "config.h"

void CTWebServerAsync::setup(CTModule* module, uint8_t* moduleAddresses) {
    this->module = module;
    this->moduleAddresses = moduleAddresses;

    if (this->server != nullptr) return;

    this->server = new AsyncWebServer(ASYNC_WEBSERVER_PORT);
    
    AsyncElegantOTA.begin(this->server);

    this->createEditGetRoute();
    this->createEditPostRoute();
    this->createReprogramRoute();
    this->createZeroRoute();
    this->createCatchallRoute();

    this->server->begin();
}

void CTWebServerAsync::createEditGetRoute() {
    this->server->on("/edit", HTTP_GET, [](AsyncWebServerRequest *request){
        if (!request->hasParam("file")) {
            request->send(400, "text/plain", "Query parameter 'file' is required");
            return;
        }

        AsyncWebParameter* param = request->getParam("file");
        String filename = "/config/" + param->value() + ".json";

        if (!LittleFS.exists(filename)) {
            request->send(404, "text/plain", "File not found");
            return;
        }

        CTLog::debug("webserver: reading file " + filename + " from LittleFS");

        File file = LittleFS.open(filename, "r");
        if (!file) {
            request->send(500, "text/plain", "Failed to open file");
            return;
        }

        String contents = file.readString();

        String interface = "<html>";
        interface += "<head>";
            interface += "<title>Edit " + String(filename) + "</title>";
        interface += "</head>";
        interface += "<body>";
            interface += "<form method=\"post\" action=\"/save\">";
                interface += "<textarea name=\"data\" rows=\"30\" cols=\"150\">" + contents + "</textarea><br>";
                interface += "<input type=\"hidden\" name=\"file\" value=\"" + String(filename) + "\">";
                interface += "<input type=\"submit\" value=\"Save\">";
            interface += "</form>";
        interface += "</body></html>";

        request->send(200, "text/html", interface);
    });
}

void CTWebServerAsync::createEditPostRoute() {
    this->server->on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (!request->hasParam("file", true)) {
            request->send(400, "text/plain", "Parameter 'file' is required");
            return;
        }

        if (!request->hasParam("data", true)) {
            request->send(400, "text/plain", "Parameter 'data' is required");
            return;
        }


        String filename = request->getParam("file", true)->value();
        String data = request->getParam("data", true)->value();

        File file = LittleFS.open(filename, "w");
        if (!file) {
            request->send(500, "text/plain", "Failed to open file");
            return;
        }

        file.print(data);
        file.close();

        CTLog::info("webserver: saved file " + filename + " to LittleFS");

        filename.replace("/config/", "");
        filename.replace(".json", "");
        
        request->send(200, "text/html", "File saved <a href=\"/edit?file=" + filename + "\">back</a>");
    });
}

void CTWebServerAsync::createReprogramRoute() {
    this->server->on("/rewrite-address", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (!request->hasParam("from")) {
            request->send(400, "text/plain", "Query parameter 'from' is required");
            return;
        }

        if (!request->hasParam("to")) {
            request->send(400, "text/plain", "Query parameter 'to' is required");
            return;
        }

        String fromAddr = request->getParam("from")->value();
        String toAddr = request->getParam("to")->value();

        if (this->module == nullptr) {
            request->send(500, "text/plain", "Internal module is not set");
            return;
        }

        this->module->changeAddress((uint8_t)fromAddr.toInt(), (uint8_t)toAddr.toInt());

        request->send(200, "text/text", "Rewrote module address from " + fromAddr + " to " + toAddr + ".\n");
    });
}

void CTWebServerAsync::createZeroRoute() {
    this->server->on("/zero", HTTP_GET, [this](AsyncWebServerRequest *request){
        if (this->module == nullptr) {
            request->send(500, "text/plain", "Internal module is not set");
            return;
        }

        if (request->hasParam("addr")) {
            // Zero a specific module
            String addr = request->getParam("addr")->value();
            uint8_t moduleAddr = (uint8_t)addr.toInt();
            
            CTLog::info("webserver: zeroing module " + String(moduleAddr, HEX));
            this->module->zero(moduleAddr);
            
            request->send(200, "text/plain", "Zeroed module " + addr + "\n");
        } else {
            // Zero all modules
            if (this->moduleAddresses == nullptr) {
                request->send(500, "text/plain", "Module addresses not set");
                return;
            }

            CTLog::info("webserver: zeroing all modules");
            this->module->zeroAll(this->moduleAddresses, MAX_CONNECTED_MODULES);
            
            request->send(200, "text/plain", "Zeroed all modules\n");
        }
    });
}

void CTWebServerAsync::createCatchallRoute() {
    this->server->onNotFound([](AsyncWebServerRequest *request){
        request->send(404);
    });
}
