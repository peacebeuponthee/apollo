/*
 * Digital Output Board. Protoboard v0.0.1
 *
 * Date: 19 Mar 2025
 * Author: peacebeuponthee
 */

#include "soc/soc.h"
#include "soc/gpio_struct.h"
#include "soc/soc_caps.h"
#include <string>
#include <WiFi.h>
#include <PubSubClient.h>
#include <utility>

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define MQTT_BROKER "YOUR_BROCKER_IP"
#define MQTT_PORT 1883
#define MODULE_ID "di-board-1"
#define MQTT_TOPIC MODULE_ID "/get/#"
const String mqtt_response_topic = MODULE_ID + String("/di");

// PIN definitions
#define LOOP_CONTROL_PIN 16
#define NUM_DI 5
const int di_pins[NUM_DI] = {26,27,25,22,23};
volatile bool di_state[NUM_DI] = {false};
// volatile bool debounce_active[NUM_DI] = {false};
volatile unsigned long last_interrupt_time[NUM_DI] = {0};
const unsigned long debounce_delay = 10;

// MQTT setup
WiFiClient espClient;
PubSubClient client(espClient);

void wifiTask(void *pvParameters) {
  volatile bool firstWiFiConnect = true;

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setSleep(false);

  for (;;) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL: Serial.println("[WiFi] SSID not found"); break;
      case WL_CONNECT_FAILED: Serial.print("[WiFi] Failed - WiFi not connected! Reason: "); break;
      case WL_CONNECTION_LOST: Serial.println("[WiFi] Connection was lost"); break;
      case WL_SCAN_COMPLETED:  Serial.println("[WiFi] Scan is completed"); break;
      case WL_DISCONNECTED:    Serial.println("[WiFi] WiFi is disconnected"); break;
      case WL_CONNECTED:
        if (firstWiFiConnect) {
          firstWiFiConnect = false;
          Serial.println("Connected to WiFi");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());
          client.setServer(MQTT_BROKER, MQTT_PORT);
          client.setCallback(callback);
        }
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
         break;
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
  Serial.println("Exit wifiTask");
  vTaskDelete(NULL);
}

void mqttLoopTask(void *pvParameters) {
  for (;;) {
    if (!client.connected()) {
        // Reconnect if needed
        Serial.println("Connecting to MQTT...");
        if (client.connect(MODULE_ID)) {
          Serial.println("Connected to MQTT");
          client.subscribe(MQTT_TOPIC);
        } else {
          vTaskDelay(pdMS_TO_TICKS(2000));
        }
    } else {
        client.loop(); // Process incoming MQTT messages
        vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
  Serial.println("Exit mqttLoopTask");
  vTaskDelete(NULL);
}

void pinPollingTask(void *pvParameters) {
  for (;;) {
    unsigned long current_time = millis();
    uint32_t gpio_state = GPIO.in;  // Read GPIO register once
    for (int i = 0; i < NUM_DI; i++) {
      if (di_state[i] != ((gpio_state & (1 << di_pins[i])) >> di_pins[i])) {
        if (current_time - last_interrupt_time[i] > debounce_delay) {
          last_interrupt_time[i] = current_time;
          di_state[i] = ((gpio_state & (1 << di_pins[i])) >> di_pins[i]);
          String responseTopic = mqtt_response_topic + String(i + 1);
          client.publish(responseTopic.c_str(), di_state[i] ? "false" : "true");
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  Serial.println("Exit pinPollingTask");
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200); 

  for (int i = 0; i < NUM_DI; i++) {
    pinMode(di_pins[i], INPUT);
  }
  pinMode(LOOP_CONTROL_PIN, OUTPUT);
  
  xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(mqttLoopTask, "MQTTLoopTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(pinPollingTask, "PollingTask", 4096, NULL, 1, NULL, 1);
}

void loop() {

}

// Callback for handling MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("MQTT callback triggered");
}