#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_wpa2.h"
WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* ssid = "Tai";
const char* password = "tranmanhtai";
const char* mqtt_server = "app.coreiot.io";
const int mqtt_port = 1883;
const char* mqtt_user = "iot_device_2";
const char* mqtt_password = "YqQEjWizbSAAU5KxIKgB";

static const int led_pin = 2;

int temp = 30;
int humi = 50;
int light_intensity = 100;
float longi = 106.65789107082472;
float lat = 10.772175109674038;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char* method = doc["method"];
  if (strcmp(method, "setValue") == 0) {
    bool value = doc["params"];
    DynamicJsonDocument response(256);
    response["value"] = value;
    char buffer[256];
    size_t n = serializeJson(response, buffer);
    client.publish("v1/devices/me/attributes", buffer, n);
  }
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.disconnect(true);
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_password, nullptr)) {
      Serial.println("connected");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}
void networkTask(void * parameter) {
  for(;;) {
    client.loop();
    DynamicJsonDocument doc(1024);
    doc["temperature"] = temp;
    doc["humidity"] = humi;
    doc["light"] = light_intensity;
    doc["long"] = longi;
    doc["lat"] = lat;

    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    client.publish("v1/devices/me/telemetry", buffer, n);

    temp++;
    humi++;
    light_intensity++;
    if (temp > 30) temp = 20;
    if (humi > 70) humi = 60;
    Serial.println("Publishing data");
    vTaskDelay(1000);
  }
}

void ledtoggleTask(void * parameter) {
  for(;;) {
    if(digitalRead(led_pin) == HIGH) {
      digitalWrite(led_pin, LOW);
      Serial.println("LED is off");
    } else {
      digitalWrite(led_pin, HIGH);
      Serial.println("LED is on");
    }
    vTaskDelay(1000);
  }
}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  
  reconnect();

  xTaskCreate(
    networkTask,
    "Task Network",
    2048,
    NULL,
    1,
    NULL
  );
  xTaskCreate(
    ledtoggleTask,
    "Task LED",
    2048,
    NULL,
    1,
    NULL
  );
}

void loop() {
  delay(10000);
}
