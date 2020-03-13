/*
Copyright 2020 Thomas Bonk <thomas@meandmymac.de>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <M5Stack.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Application.h"
#include "Constants.h"
#include "RestService.h"

RestService * RestService::_shared = NULL;

RestService::RestService() : _status(CREATED), _server(80) {
    _shared = this;

    xTaskCreatePinnedToCore(
        [](void *params) {      /* Function to implement the task */
            RestService::shared()->initializeService();
        },
        "initializeService",    /* Name of the task */
        4096,                   /* Stack size in words */
        NULL,                   /* Task input parameter */
        3,                      /* Priority of the task */
        NULL,                   /* Task handle. */
        0);                     /* Core where the task should run */
}

void RestService::readSensors() {
    const int sensors = Application::shared()->getSensors()->count();
    DynamicJsonDocument doc(2048);
    JsonObject sensorJson;

    for (int n = 0; n < sensors; n++) {
        sensorJson = doc.createNestedObject();

        struct SensorValues values = Application::shared()->getSensors()->read(n);

        sensorJson["name"] = Application::shared()->getSensors()->getSensorName(n);
        sensorJson["temperature"] = values.temperature;
        sensorJson["humidity"] = values.humidity;

        Serial.print("Sensor = ");
        serializeJson(sensorJson, Serial);
        Serial.println("");
    }

    String json = "";
    serializeJson(doc, json);

    _server
        .send(
            200, 
            "application/json", 
            json);
}

void RestService::readSensorCount() {
    _server
        .send(
            200, 
            "application/json", 
            String(Application::shared()->getSensors()->count()));
}

void RestService::readSensor() {
    const int sensors = Application::shared()->getSensors()->count();
    int sensor = _server.pathArg(0).toInt();

    if (sensor >= 0 && sensor < sensors) {
        DynamicJsonDocument doc(2048);
        struct SensorValues values = Application::shared()->getSensors()->read(sensor);

        doc["name"] = Application::shared()->getSensors()->getSensorName(sensor);
        doc["temperature"] = values.temperature;
        doc["humidity"] = values.humidity;

        String json = "";
        serializeJson(doc, json);

        _server
            .send(
                200, 
                "application/json", 
                json);
    } else {
        _server.send(404, "text/plain", "Sensor not found");
    }
}


void RestService::initializeService() {
    _status = INITIALIZING;

    // Wait for connection
    Serial.println("Waiting for Wifi connection");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Set up mDNS responder:
    // - first argument is the domain name, in this example
    //   the fully-qualified domain name is "esp8266.local"
    // - second argument is the IP address to advertise
    //   we send our IP address on the WiFi network
    if (!MDNS.begin(APPLICATION_NAME)) {
        Serial.println("Error setting up MDNS responder!");
        _status = ERROR;
        return;
    }
    Serial.println("mDNS responder started");

    _server.on("/sensors", HTTP_GET, []() {
        RestService::shared()->readSensors();
    });

    _server.on("/sensors/count", HTTP_GET, []() {
        RestService::shared()->readSensorCount();
    });

    _server.on("/sensors/{}", HTTP_GET, []() {
        RestService::shared()->readSensor();
    });

    _server.begin();    
    Serial.printf("HTTP server started on %s", WiFi.localIP());

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    _status = STARTED;

    vTaskDelete(NULL);
}
