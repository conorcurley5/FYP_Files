#include <Arduino.h>
// Include for ModbusMaster type 
#include <ModbusMaster.h>

#ifndef MOTOR_H
#define MOTOR_H

    extern const int num_vitals;

    void start_motor ();

    void stop_motor ();

    void set_motor_speed (uint8_t speed);

    void set_motor_mode_modbus ();

    void set_motor_mode_gpio ();

    void set_motor_accel_time (uint8_t time); 

    uint16_t* fetch_motor_vitals ();

    void setup_motor();

#endif