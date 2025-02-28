#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient wifiClient;
extern PubSubClient client;
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;

extern int light_intensity;
extern float longi;
extern float lat;

void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void handle_message();
void network_run();
void connect_server();