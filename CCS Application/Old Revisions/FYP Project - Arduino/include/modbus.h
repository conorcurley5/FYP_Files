#include <Arduino.h>
// Include async libraries for Websockets and servers
#include <ModbusMaster.h>

#ifndef MODBUS_H
#define MODBUS_H

    // Initialise RE/DE pins before transmission
    void preTransmission();

    // // Initialise RE/DE pins after transmission
    void postTransmission();

    // Get the contents of a given register
    uint16_t fetch_value (uint16_t reg);

    // Send a command to a given register with value
    bool send_command (uint16_t reg, uint8_t value);

    void setup_modbus();

#endif