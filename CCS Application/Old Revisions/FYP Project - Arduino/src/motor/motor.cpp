
#include <modbus.h>

//// 
//// DEFINES
//// 

// Define motor functions register addresses
#define MTRREG_RSTATUS 0x0066
#define MTRREG_MODE 0x0136
#define MTRREG_ACCELTIME 0x00E6
#define MTRREG_SPEED 0x0056

// Define motor status values
#define MTRVAL_RSTATUS_ON 0x01
#define MTRVAL_RSTATUS_OFF 0x00

// Define motor mode values
#define MTRVAL_MODE_MODBUS 0x01
#define MTRVAL_MODE_GPIO 0x00

// 
// GLOBAL VARIABLES
// 

const int num_vitals = 5;

const uint16_t vitals_registers[] = {
    0x005F,
    0x0066,
    0x0076,
    0x0096,
    0x00C6
};

//// 
//// COMMAND FUNCTIONS
//// 

void start_motor () {
  send_command(MTRREG_RSTATUS, MTRVAL_RSTATUS_ON);
}

void stop_motor () {
  send_command(MTRREG_RSTATUS, MTRVAL_RSTATUS_OFF);
}

void set_motor_speed (uint8_t speed) {
  send_command(MTRREG_SPEED, speed);
}

void set_motor_mode_modbus () {
  send_command(MTRREG_MODE, MTRVAL_MODE_MODBUS);
}

// Do not use, functionality not implemented in hardware. For good reason too.
void set_motor_mode_gpio () {
  send_command(MTRREG_MODE, MTRVAL_MODE_GPIO);
}

void set_motor_accel_time (uint8_t time) {
  send_command(MTRREG_ACCELTIME, time);
}

//// 
//// REQUEST FUNCTIONS
//// 

uint16_t* fetch_motor_vitals () {
  static uint16_t vitals[num_vitals];

  for (int i = 0; i < num_vitals; i++) {
    vitals[i] = fetch_value(vitals_registers[i]);
    delay(200);
  }

  return vitals;
}

// 
// LIFECYCLE FUNCTIONS
// 

void setup_motor () {
  set_motor_mode_modbus();
}