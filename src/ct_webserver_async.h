#ifndef _CLACKOTRON_WEBSERVER_ASYNC_H_
#define _CLACKOTRON_WEBSERVER_ASYNC_H_

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "ct_module.h"

/// @brief A class for controlling the async webserver (config mode)
class CTWebServerAsync {
    public:
        /// @brief Set up the webserver and OTA endpoints
        void setup(CTModule* module, uint8_t* moduleAddresses);

    private:
        AsyncWebServer* server;
        CTModule* module;
        uint8_t* moduleAddresses;

        void createEditGetRoute();
        void createEditPostRoute();
        void createReprogramRoute();
        void createZeroRoute();
        void createCatchallRoute();
};

#endif // _CLACKOTRON_WEBSERVER_ASYNC_H_
