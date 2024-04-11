#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <SoftwareSerial.h>

SoftwareSerial leonardoSerial(8, 10);
String str;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  leonardoSerial.begin(9600);
}

void loop() {
  receiveAndSend();
}

// Function to receive serial data and send a response
void receiveAndSend() {
  while (!leonardoSerial.available()) {}
  //Checking is any data is coming from Esp8266
  if (leonardoSerial.available()) {
    str = leonardoSerial.readString();
    Serial.println(str);
    Keyboard.print(str);
  }
  delay(1000);
}



