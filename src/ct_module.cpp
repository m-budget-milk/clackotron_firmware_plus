#include <Wire.h>
#include "ct_module.h"
#include "ct_logging.h"
#include "config.h"

CTModule::CTModule() {
    this->rs485 = nullptr;
}

CTModule::~CTModule() {
    delete this->rs485;
}

void CTModule::setup() {
    Wire.begin();

    this->rs485 = new KMP_RS485(Serial1, MODULE_RS485_DE, MODULE_RS485_TX, MODULE_RS485_RX, HIGH);
    this->rs485->begin(RS485_BAUDRATE);
}

void CTModule::writeChar(uint8_t addr, char c) {
    if (!this->canShowChar(c)) {
        CTLog::error("module: cannot show character '" + String(c) + "', using ' ' instead");
        return;
    }

    int pos = this->getPosForChar(c);
    this->sendToModule(0xC0, addr, pos);
}

void CTModule::writeRaw(uint8_t addr, uint8_t data) {
    this->sendToModule(0xC0, addr, data);
}

void CTModule::changeAddress(uint8_t oldAddr, uint8_t newAddr) {
    this->sendToModule(0xCE, oldAddr, newAddr);
}

void CTModule::zero(uint8_t addr) {
    this->sendToModule(0xC5, addr);
}

void CTModule::zeroAll(uint8_t* addresses, uint8_t count) {
    if (addresses == nullptr || count == 0) {
        CTLog::info("module: zeroAll called with empty address list");
        return;
    }
    
    CTLog::info("module: zeroing all modules");
    for (uint8_t i = 0; i < count; i++) {
        if (addresses[i] != 0x00) {
            this->zero(addresses[i]);
            delay(100); // Small delay between commands
        }
    }
    CTLog::info("module: finished zeroing all modules");
}

void CTModule::step(uint8_t addr) {
    this->sendToModule(0xC6, addr);
}

bool CTModule::getType(uint8_t addr, uint8_t* type) {
    if (this->rs485 == nullptr || type == nullptr) return false;

    // Drain stale bytes before issuing a new request.
    while (this->rs485->available() > 0) {
        this->rs485->read();
    }

    this->sendToModule(0xDD, addr);

    int response = this->waitForResponseByte(200);
    if (response < 0) {
        CTLog::error("module: failed to read type from module " + String(addr, HEX));
        return false;
    }

    *type = (uint8_t)response;
    CTLog::info("module: module " + String(addr, HEX) + " type = 0x" + String(*type, HEX));
    return true;
}

bool CTModule::getAddress(uint8_t addr, uint8_t* readAddr) {
    if (this->rs485 == nullptr || readAddr == nullptr) return false;

    // Drain stale bytes before issuing a new request.
    while (this->rs485->available() > 0) {
        this->rs485->read();
    }

    this->sendToModule(0xDE, addr);

    int response = this->waitForResponseByte(200);
    if (response < 0) {
        CTLog::error("module: failed to read address from module " + String(addr, HEX));
        return false;
    }

    *readAddr = (uint8_t)response;
    CTLog::info("module: module " + String(addr, HEX) + " address = " + String(*readAddr, HEX));
    return true;
}

// Check if module can show a character and get the position of the character
// if possible. The char to blade mapping is based on the following table:
// https://github.com/adfinis/sbb-fallblatt/blob/master/doc/char_mapping.md#alphanummeric

bool CTModule::canShowChar(char c) {
    if (c >= 0x30 && c <= 0x39) return true;
    if (c >= 0x41 && c <= 0x5A) return true;
    if (c >= 0x61 && c <= 0x7A) return true;
    if (c == '/' || c == '-' || c == '.' || c == ' ') return true;
    return false;
}

uint8_t CTModule::getPosForChar(char c) {
    if (c >= 'a' && c <= 'z') c -= ('a' - 'A');
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c == '/') return 26;
    if (c == '-') return 27;
    if (c >= '1' && c <= '9') return c - 21;
    if (c == '0') return 37;
    if (c == '.') return 38;
    return 39;
}

bool CTModule::canShowHour(uint8_t n) {
    return n >= 0 && n <= 23;
}

uint8_t CTModule::getPosForHour(uint8_t n) {
    return n;
}

bool CTModule::canShowMinute(uint8_t n) {
    return n >= 0 && n <= 59;
}

uint8_t CTModule::getPosForMinute(uint8_t n) {
    if (n >= 31) return n - 31;
    return n + 30;
}

// Send a command to a module using its address and 0-2 parameters.
// This is based on the protocol specification provided by adfinis:
// https://github.com/adfinis/sbb-fallblatt/blob/master/doc/protocol_new_modules.md

void CTModule::sendToModule(uint8_t cmd, uint8_t addr) {
    if (this->rs485 == nullptr) return;

    String dbg = "module: sending command " + String(cmd, HEX);
    dbg += " to module " + String(addr, HEX);
    dbg += " (no parameters)";
    CTLog::debug(dbg);

    this->sendModuleStart(cmd, addr);
    this->sendModuleEnd();
}

void CTModule::sendToModule(uint8_t cmd, uint8_t addr, uint8_t arg1) {
    if (this->rs485 == nullptr) return;

    String dbg = "module: sending command " + String(cmd, HEX);
    dbg += " to module " + String(addr, HEX);
    dbg += " (parameters: 0x" + String(arg1, HEX) + ")";
    CTLog::debug(dbg);

    this->sendModuleStart(cmd, addr);
    this->rs485->write(arg1);
    this->rs485->flush();
    this->sendModuleEnd();
}

void CTModule::sendToModule(uint8_t cmd, uint8_t addr, uint8_t arg1, uint8_t arg2) {
    if (this->rs485 == nullptr) return;

    String dbg = "module: sending command " + String(cmd, HEX);
    dbg += " to module " + String(addr, HEX);
    dbg += " (parameters: 0x" + String(arg1, HEX) + ", 0x" + String(arg2, HEX) + ")";
    CTLog::debug(dbg);

    this->sendModuleStart(cmd, addr);
    this->rs485->write(arg1);
    this->rs485->flush();
    this->rs485->write(arg2);
    this->rs485->flush();
    this->sendModuleEnd();
}

int CTModule::waitForResponseByte(unsigned long timeoutMs) {
    unsigned long start = millis();
    while ((millis() - start) < timeoutMs) {
        if (this->rs485->available() > 0) {
            return this->rs485->read();
        }
        delay(1);
    }
    return -1;
}

void CTModule::sendModuleStart(uint8_t cmd, uint8_t addr) {
    this->rs485->beginTransmission();
    this->rs485->sendBreak(MODULE_BREAK_DURATION);
    this->rs485->write(0xFF);
    this->rs485->flush();
    this->rs485->write(cmd);
    this->rs485->flush();
    this->rs485->write(addr);
    this->rs485->flush();
}

void CTModule::sendModuleEnd() {
    this->rs485->endTransmission();
    delay(MODULE_WRITE_DELAY);
}
