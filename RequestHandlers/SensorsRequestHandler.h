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

#ifndef __SENSORSREQUESTHANDLER_H__
#define __SENSORSREQUESTHANDLER_H__

#include <WebServer.h>
#include "PathArgsRequestHandler.h"

class SensorsRequestHandler : public PathArgsRequestHandler {
    public:
        SensorsRequestHandler() : PathArgsRequestHandler("/sensors", HTTP_GET) {}
        ~SensorsRequestHandler() {}

        virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) { 
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

            server.send(200, "application/json", json);

            return true;
        }
};

#endif
