#include <Arduino.h>

#include <slave.h>
#include <motor.h>
#include <modbus.h>

void setup() {
  Serial.begin(9600);

  setup_wire();
  setup_modbus();
  setup_motor();
}

void loop() {
  handle_next_action();

  
}