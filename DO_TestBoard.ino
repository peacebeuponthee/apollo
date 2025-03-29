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
#define MQTT_BROKER "YOUR_BROKER_IP"
#define MQTT_PORT 1883
#define MODULE_ID "do-board-1"
#define MQTT_TOPIC MODULE_ID "/#"

// PIN definitions
#define DO_1 16
#define DO_2 13
#define DO_3 26
#define DO_4 25
#define DO_5 27

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
        vTaskDelay(pdMS_TO_TICKS(1));
    }
  }
  Serial.println("Exit mqttLoopTask");
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200); 

  pinMode(DO_1, OUTPUT);
  pinMode(DO_2, OUTPUT);
  pinMode(DO_3, OUTPUT);
  pinMode(DO_4, OUTPUT);
  pinMode(DO_5, OUTPUT);
  
  xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(mqttLoopTask, "MQTTLoopTask", 4096, NULL, 1, NULL, 1);
}

void loop() {

}

// Callback for handling MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "do-board-1/set/do1") == 0) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    int state = atoi(message);
    if (state == 0) {
      // digitalWrite(35, LOW);
      GPIO.out_w1tc = (1 << DO_1);; // Set DO_1 Low
    } else {
      // digitalWrite(35, HIGH);
      GPIO.out_w1ts = (1 << DO_1);; // Set DO_1 High
    }
  }
  else if (strcmp(topic, "do-board-1/set/do2") == 0) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    int state = atoi(message);
    if (state == 0) {
      GPIO.out_w1tc = (1 << DO_2);; // Set DO_1 Low
    } else {
      GPIO.out_w1ts = (1 << DO_2);; // Set DO_1 High
    }
  }
  else if (strcmp(topic, "do-board-1/set/do3") == 0) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    int state = atoi(message);
    if (state == 0) {
      GPIO.out_w1tc = (1 << DO_3);; // Set DO_1 Low
    } else {
      GPIO.out_w1ts = (1 << DO_3);; // Set DO_1 High
    }
  }
  else if (strcmp(topic, "do-board-1/set/do4") == 0) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    int state = atoi(message);
    if (state == 0) {
      GPIO.out_w1tc = (1 << DO_4);; // Set DO_1 Low
    } else {
      GPIO.out_w1ts = (1 << DO_4);; // Set DO_1 High
    }
  }
  else if (strcmp(topic, "do-board-1/set/do5") == 0) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    int state = atoi(message);
    if (state == 0) {
      GPIO.out_w1tc = (1 << DO_5);; // Set DO_1 Low
    } else {
      GPIO.out_w1ts = (1 << DO_5);; // Set DO_1 High
    }
  }
}