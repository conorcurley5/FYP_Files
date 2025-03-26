#include <Arduino.h>
// Include async libraries for Websockets and servers

#ifndef SLAVE_H
#define SLAVE_H

    int parseInt (char buffer[]);

    void handle_request(char* action, char* payload);

    int indexOfChar (char* arr, int size);

    void onRequest();

    void onReceive(int howMany);

    void setup_wire();

    void handle_next_action ();

    void send_vitals (); 

#endif