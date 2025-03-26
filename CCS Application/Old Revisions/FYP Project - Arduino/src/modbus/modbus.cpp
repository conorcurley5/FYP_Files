// Include ModbusMaster library
#include <ModbusMaster.h>

// Define Modbus Pins
#define MAX485_DE      9
#define MAX485_RE_NEG  8

ModbusMaster node;

// Initialise RE/DE pins before transmission
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

// // Initialise RE/DE pins after transmission
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

// Get the contents of a given register
uint16_t fetch_value (uint16_t reg) {
  uint16_t result;
  uint16_t response = 0x0000;

  result = node.readHoldingRegisters(reg, 1);

  if (result == node.ku8MBSuccess) {
    response = node.getResponseBuffer(0);
  }

  node.clearResponseBuffer();
  return response;
} 

bool send_command (uint16_t reg, uint8_t value) {
    uint8_t result;
    bool response = false;
    result = node.writeSingleRegister(0x0066, 0);

    if (result == node.ku8MBSuccess) {
        response = true;
    }

    node.clearResponseBuffer();
    
    return response;
}

void setup_modbus () {
    pinMode(MAX485_RE_NEG, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    digitalWrite(MAX485_RE_NEG, 0);
    digitalWrite(MAX485_DE, 0);

    node.begin(0x01, Serial);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}