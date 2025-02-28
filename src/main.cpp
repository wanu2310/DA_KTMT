#include <DHT.h>
#include <UI.h>
#include <network.h>
#include <Logcat.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_heap_caps.h"
#include "esp32-hal-psram.h"
#include "esp32s3/spiram.h"
#include "Hardware_Setup.h"

#define DHTPIN 4      
#define DHTTYPE DHT22  // Define the sensor 

DHT dht(DHTPIN, DHTTYPE);

TaskHandle_t Network; 
TaskHandle_t Sensor;
TaskHandle_t Led;
TaskHandle_t UI;

static const int led_pin = 2;

void dht_setup() {
  dht.begin();
  Serial.println("DHT sensor is starting...");
}

void uiTask(void * parameter) {
  for(;;) {
    lv_task_handler();
    vTaskDelay(10);
  }
}

void sensorTask(void * parameter) {
  for(;;) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.println(" *C");
    }
    DynamicJsonDocument doc(1024);
    doc["light"] = light_intensity;
    doc["long"] = longi;
    doc["lat"] = lat;
    doc["temperature"] = t;
    doc["humidity"] = h;
    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    light_intensity++;
    if (light_intensity > 140) {
      light_intensity = 100;
    }
    client.publish("v1/devices/me/telemetry", buffer, n);

    vTaskDelay(5000);
  }
}

void networkTask(void * parameter) {
  for(;;) {
    network_run();
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
  // pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  // setup_wifi();
  // dht_setup();
  // connect_server();
  // reconnect();
  check_memory();
  hardware_setup();
  setup_ui();

  // xTaskCreate(
  //   networkTask,
  //   "Task Network",
  //   2048,
  //   NULL,
  //   1,
  //   &Network
  // );

  // xTaskCreate(
  //   uiTask,
  //   "Task UI",
  //   409600,
  //   NULL,
  //   3,
  //   &UI
  // );

  // xTaskCreate(
  //   ledtoggleTask,
  //   "Task LED",
  //   2048,
  //   NULL,
  //   1,
  //   &Led
  // );

  // xTaskCreate(
  //   sensorTask,
  //   "Task Sensor",
  //   2048,
  //   NULL,
  //   2,
  //   &Sensor
  // );
}

void loop() {  
  lv_task_handler();
  delay(1000);
  Serial.println("Loop");
}
