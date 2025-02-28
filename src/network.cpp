#include "network.h"
#include "Logcat.h"
WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* ssid = "Tai";
const char* password = "tranmanhtai";
const char* mqtt_server = "app.coreiot.io";
const int mqtt_port = 1883;
const char* mqtt_user = "iot_device_2";
const char* mqtt_password = "8qixx7tv7gqd0iw800vz";

int light_intensity = 100;
float longi = 106.65789107082472;
float lat = 10.772175109674038;

String message;

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

  void connect_server(){
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
  }

  void network_run(){
    client.loop();
    // if(!message.isEmpty())
    // {
    //     handle_message();
    // }
    LogI(__func__, "Network Running");
  }
