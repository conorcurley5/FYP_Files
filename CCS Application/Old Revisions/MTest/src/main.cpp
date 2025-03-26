// Include ModbusMaster library
#include <ModbusMaster.h>
#include <Wire.h>

// // Define Modbus Pins
#define MAX485_DE      9
#define MAX485_RE_NEG  10

ModbusMaster node;

unsigned long previousMillis = 0;  // Stores the last time the code was executed
const long interval = 2000;

volatile bool motorModePending = false;
volatile int motorModeState = 0;

volatile bool motorSpeedPending = false;
volatile int motorSpeedState = 0;

volatile bool motorOnPending = false;
volatile int motorOnState = 0;

volatile bool vitalsPending = false;
volatile int vitalRegister = 0;

volatile bool isSetup = false;

bool deactivateVitals = false;

int num_vitals = 5;

// Define vitals register addresses
uint16_t register_indices[] = {
    0x005F,
    0x0066,
    0x0076,
    0x0096,
    0x00C6
};

uint16_t vitals[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

int parseInt (char buffer[]) {
  int num = 0;

  for (int i = 0; buffer[i] != '\0'; i++) {
    num = num * 10 + (buffer[i] - '0');  // Convert each character to an integer and accumulate
  }

  return num;
}

// // Initialise RE/DE pins before transmission
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

// Initialise RE/DE pins after transmission
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

uint16_t get_register_value (uint16_t reg) {
  uint16_t result;
  result = node.readHoldingRegisters(reg, 1);
  uint16_t response = 0x00;

  if (result == node.ku8MBSuccess) {
    response = node.getResponseBuffer(0);
  }

  node.clearResponseBuffer();
  return response;
} 

void start_motor () {
  uint8_t result;
  result = node.writeSingleRegister(0x0066, 1);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Start Success.");
  } else {
    Serial.println("Error starting motor rotation.");
  }

  node.clearResponseBuffer();
}

void stop_motor () {
  uint8_t result;
  result = node.writeSingleRegister(0x0066, 0);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Stop Success.");
  } else {
    Serial.println("Error stopping motor rotation.");
  }

  node.clearResponseBuffer();
}

void set_motor_speed (int speed) {
  uint8_t result;
  result = node.writeSingleRegister(0x0056, speed);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Speed Set.");
  } else {
    Serial.println("Error setting motor speed.");
  }
  
  node.clearResponseBuffer();
}

void set_motor_mode (int mode) {
  uint8_t result;
  
  result = node.writeSingleRegister(0x0136, mode);

  delay(500);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Mode Set.");
  } else {
    Serial.println("Error setting motor mode.");
    Serial.println(result);
  }
  
  node.clearResponseBuffer();
}

void set_motor_accel_time (int time) {
  uint8_t result;
  result = node.writeSingleRegister(0x00E6, time);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Accel Set.");
  } else {
    Serial.println("Error setting motor accel.");
  }
  
  node.clearResponseBuffer();
}

void handle_request(char action[], char payload[]) {
  // Set Speed
  
  return;
}

// Handle an incoming request from the master.
void onReceive(int howMany) {
  deactivateVitals = true;
  delay(1000);

  char buffer[howMany];
  while (Wire.available()) {
    int index = howMany - Wire.available();
    buffer[index] = (char)Wire.read();
  }
  buffer[howMany] = '\0'; 

  int index = -1;

  for (int i = 0; i < howMany; i++) {
    if (buffer[i] == '=') {
      index = i;
    }
  }

  if (index == -1) return;

  char action[index + 1];
  char payload[howMany-index];

  for (int i = 0; i < index; i++) {
    action[i] = buffer[i];
  }
  action[index] = '\0';
    
  for (int i = index + 1; i < howMany; i++) {
    payload[i - index - 1] = buffer[i];  // Use i - index - 1 to correctly place characters in the payload
  }
  payload[howMany - index - 1] = '\0';
  
  if (strcmp(action, "SET_MTR_SPD") == 0) {
    int speed = parseInt(payload);
    motorSpeedPending = true;
    motorSpeedState = speed;
  } else if (strcmp(action, "TGL_MTR_ON") == 0) {
    motorOnPending = true;
    if (strcmp(payload, "1") == 0) {
      motorOnState = 1;
    } else {
      motorOnState = 0;
    }
  } else if (strcmp(action, "TGL_MTR_MODE") == 0) {
    motorModePending = true;
    if (strcmp(payload, "0") == 0) {
      motorModeState = 0;
    } else {
      motorModeState = 1;
    }
  } else if (strcmp(action, "VITALS") == 0) {
    vitalRegister = parseInt(payload);
  } else {
    Serial.println("unknown command");
  }
}

void onRequest() {
  for (int i = 0; i < num_vitals; i++) {
    uint16_t b = vitals[i];
    uint8_t msb = b >> 8;
    uint8_t lsb = b;

    Wire.write(msb);
    Wire.write(lsb);
  }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // Wire.begin(8);
  // Wire.onRequest(onRequest);
  // Wire.onReceive(onReceive);
  Serial.println("TX, RX");
  Serial.println(TX1);
  Serial.println(RX1);

  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);

  node.begin(0x01, Serial1);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void fetch_vitals() { 
  for (int i = 0; i < num_vitals; i++) {
    vitals[i] = get_register_value(register_indices[i]);
    delay(200);
  }
}

void loop() {
  // Serial.println("Hello Again!");
  // delay(5000);
  // put your main code here, to run repeatedly:
  // if (motorModePending) {
  //     set_motor_mode(motorModeState);  // Perform the Modbus operation here
  //     motorModePending = false;  // Reset the flag after the operation 
  //     deactivateVitals = false;
  // }

  if (!isSetup) {
    set_motor_mode(1);
    isSetup = true;
  }

  // set_motor_speed(500);
  // start_motor();
  // delay(3000);
  // stop_motor();
  // delay(3000);

  // digitalWrite(MAX485_RE_NEG, 1);
  // digitalWrite(MAX485_DE, 1);

  // delay(3000);

  // digitalWrite(MAX485_RE_NEG, 0);
  // digitalWrite(MAX485_DE, 0);

  // delay(3000);

  // if (motorOnPending) {
  //   if (motorOnState == 0) {
  //     stop_motor();
  //   } else {
  //     start_motor();
  //   } // Reset the flag after the operation
  //   motorOnPending = false;
  //   motorOnState = 0;
  //   deactivateVitals = false;
  // }

  // if (motorSpeedPending) {
  //     set_motor_speed(motorSpeedState);  // Perform the Modbus operation here
  //     motorSpeedPending = false;
  //     motorSpeedState = 0;  // Reset the flag after the operation
  //     deactivateVitals = false;
  // }

  // set_motor_speed(500);
  // start_motor();

  // delay(5000);

  stop_motor();
  // delay(5000);

  // if (!deactivateVitals) {
  //   unsigned long currentMillis = millis(); 
  //   // Check if the interval has passed
  //     if (currentMillis - previousMillis >= interval) {
  //         // Save the last time you ran the code
  //         previousMillis = currentMillis;

  //         // Your code to run periodically here
  //         fetch_vitals();
  //   }
  // }
}