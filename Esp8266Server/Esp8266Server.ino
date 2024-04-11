#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// Configure wifi setup
const char *con_ssid = "";
const char *con_password = "";
IPAddress ip;

// Configure access point
const char *ap_ssid = "keyboard-server";
const char *ap_password = "KeyboardNodemcu"; // o as 0 and s as $
const char *host = "nodemcu";
bool isPublic = false;

ESP8266WebServer server(80);
SoftwareSerial espSerial(D2, D3);  // (D2 = 4) RX, (D3 = 0) TX pins for ESP8266

void setup() {
  Serial.begin(9600);     // Initialize the Arduino serial port
  espSerial.begin(9600);  // Initialize the ESP8266 serial port

  Serial.println("");
  Serial.println("[Keybaord Server] - Starting...");

  startAP();
  //connectWifi(con_ssid, con_password);
  setMDNS();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/send-hid", HTTP_POST, handleHID);
  server.begin();

  Serial.println("[Keybaord Server] - HTTP server started");
}

void loop() {
    server.handleClient();
}

// --------------------------------- Access Point

void startAP() {
    WiFi.mode(WIFI_AP);
    if (isPublic) {
        WiFi.softAP(ap_ssid);
    } else {
        WiFi.softAP(ap_ssid, ap_password);
    }
    Serial.println("[Wifi] ----------------------- Access Point");
    Serial.println("[Wifi] SSID: " + String(ap_ssid));
    Serial.println("[Wifi] Password: " + String(ap_password));
    Serial.println("[Wifi] IP: " + WiFi.softAPIP().toString());
}

// --------------------------------- MDNS

void setMDNS(){
    int totalTry = 5;
    while (!MDNS.begin(host) && totalTry > 0) {
        Serial.println(".");
        delay(1000);
        totalTry--;
    }

    Serial.println("[Wifi] mDNS responder started");
    Serial.print("[Wifi] You can now connect to: http://");
    Serial.print(host);
    Serial.println(".local");
}

// --------------------------------- WIFI

void connectWifi(const char* ssid, const char* pwd){
    Serial.print("[Wifi] Connection found on: ");
    Serial.print(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    Serial.print("\n[Wifi] Connecting to WiFi ..");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\n[Wifi] Wifi Connected");

    Serial.print("[Wifi] RRSI: ");
    Serial.println(WiFi.RSSI());
    ip = WiFi.localIP();
    Serial.print("[Wifi] IP ADDRESS: ");
    Serial.println(ip);
}

void disconnectWifi(){
    WiFi.disconnect();
    Serial.println("[Wifi] Disconnected\n");
}

// --------------------------------------- Server

void handleRoot() {
  server.send(200, "text/plain", "Hello from Keyboard Server!");
}

void handleHID() {
    String message = server.arg("plain");
    StaticJsonDocument<200> doc;
    deserializeJson(doc, message);
    const char* text = doc["text"];
    espSerial.print(text);

    StaticJsonDocument<200> response;
    response["status"] = "OK";
    response["received_data"] = text;
    String jsonResponse;
    serializeJson(response, jsonResponse);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(200, "application/json", jsonResponse);
}
