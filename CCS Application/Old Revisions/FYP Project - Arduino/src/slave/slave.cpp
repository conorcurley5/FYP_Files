#include <Wire.h>
#include <motor.h>

volatile char* queue[10];
volatile int queue_index = 0;

volatile int motorModeState = 0;
volatile int motorSpeedState = 0;
volatile int motorOnState = 0;

const int num_vitals = 5;

// Parse integer from character string
int parseInt (char buffer[]) {
  int num = 0;

  for (int i = 0; buffer[i] != '\0'; i++) {
    num = num * 10 + (buffer[i] - '0');  // Convert each character to an integer and accumulate
  }

  return num;
}

void handle_request(char* action, char* payload) {
  if (queue_index > 9) {
    Serial.println("Action denied: Queue Full.");
    return;
  }

  if (strcmp(action, "SET_MTR_SPD") == 0) {
    int speed = parseInt(payload);

    queue[queue_index] = action;
    queue_index++;

    motorSpeedState = speed;
  } else if (strcmp(action, "TGL_MTR_ON") == 0) {
    queue[queue_index] = action;

    if (strcmp(payload, "1") == 0) {
      motorOnState = 1;
    } else {
      motorOnState = 0;
    }

    queue_index++;
  } else if (strcmp(action, "TGL_MTR_MODE") == 0) {
    queue[queue_index] = action;

    if (strcmp(payload, "0") == 0) {
      motorModeState = 0;
    } else {
      motorModeState = 1;
    }

    queue_index++;
  } else {
    Serial.println("unknown command");
  }
  
  return;
}

int indexOfChar (char* arr, int size) {
  int index = -1;

  for (int i = 0; i < size; i++) {
    if (arr[i] == '=') {
      index = i;
      break;
    }
  }

  return index;
}

void onRequest() {
  // Get vitals and send them over wire
  // Possibly check for a vitals action.
  queue[queue_index] = (char*)"VITALS";
  queue_index++;

  return;
}

// Handle an incoming request from the master.
void onReceive(int howMany) {
  char buffer[howMany];

  while (Wire.available()) {
    int index = howMany - Wire.available();

    buffer[index] = (char)Wire.read();
  }

  buffer[howMany] = '\0'; 

  int index = indexOfChar((char*)buffer, howMany);

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

  handle_request((char*)action, (char*)payload);

  return;
}

void send_vitals () {
  uint16_t* vitals = fetch_motor_vitals();

  for (int i = 0; i < num_vitals; i++) {
    uint16_t b = vitals[i];

    uint8_t msb = b >> 8;
    uint8_t lsb = b;

    Wire.write(msb);
    Wire.write(lsb);
  }

  return;
}

void handle_next_action () {
  if (queue_index == 0) return;

  if (
    strcmp((const char*)queue[queue_index-1], "VITALS") == 0
  ) {
    send_vitals();
  } else if (
    strcmp((const char*)queue[queue_index-1], "SET_MTR_SPD") == 0
  ) {
    set_motor_speed((uint8_t)motorSpeedState);
  } else if (
    strcmp((const char*)queue[queue_index-1], "TGL_MTR_ON") == 0
  ) {
    if (motorOnState == 1) {
      start_motor();
    } else {
      stop_motor();
    }
  } else if (
    strcmp((const char*)queue[queue_index-1], "TGL_MTR_MODE") == 0
  ) {
    if (motorModeState == 1) {
      set_motor_mode_gpio();
    } else {
      set_motor_mode_modbus();
    }
  }

  queue[queue_index - 1] = nullptr;
  queue_index--;

  return;
}

void setup_wire() {
  Wire.begin(8);
  Wire.onRequest(onRequest);
  Wire.onReceive(onReceive);
}