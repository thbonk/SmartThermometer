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

#ifndef __SENSORREQUESTHANDLER_H__
#define __SENSORREQUESTHANDLER_H__

#include <WebServer.h>
#include "PathArgsRequestHandler.h"

class SensorRequestHandler : public PathArgsRequestHandler {
    public:
        SensorRequestHandler() : PathArgsRequestHandler("/sensors/{}", (HTTPMethod)(HTTP_GET | HTTP_POST)) {}
        ~SensorRequestHandler() {}

        virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) { 
            if (requestMethod == HTTP_GET) {
                return handleGet(server, requestUri);
            } else if (requestMethod == HTTP_POST) {
                return handlePost(server, requestUri);
            }
            
            return false;
        }

    private:
        bool handleGet(WebServer& server, String requestUri) {
            const int sensors = Application::shared()->getSensors()->count();
            int sensor = server.pathArg(0).toInt();

            if (sensor >= 0 && sensor < sensors) {
                server.send(200, "application/json", sensorJson(sensor));
                return true;
            }
                
            return false;
        }

        bool handlePost(WebServer& server, String requestUri) {
            /*if (!server.header("Content-Type:").startsWith("application/json")) {
                server.send(400, "text/plain", "Content-Type must be application/json.");
                return true;
            }*/

            if (server.args() < 1) {
                server.send(400, "text/plain", "No payload given.");
                return true;
            }

            /*if (!server.header("Accept:").startsWith("application/json")) {
                server.send(406, "text/plain", "Can only generate content of type application/json.");
                return true;
            }*/

            const int sensors = Application::shared()->getSensors()->count();
            int sensor = server.pathArg(0).toInt();

            if (sensor >= 0 && sensor < sensors) {
                String body = server.arg(0);
                DynamicJsonDocument doc(2048);
                DeserializationError error = deserializeJson(doc, body);
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());

                    server.send(400, "text/plain", String("Invalid body, error = ") + String(error.c_str()));
                    return true;
                }

                if (doc.containsKey("name") && doc["name"].is<String>()) {
                    String val = doc["name"].as<String>();
                    Application::shared()->getSensors()->setSensorName(sensor, val);
                }
                if (doc.containsKey("temperatureUrl") && doc["temperatureUrl"].is<String>()) {
                    String val = doc["temperatureUrl"].as<String>();
                    Application::shared()->getSensors()->setTemperatureUrl(sensor, val);
                }
                if (doc.containsKey("humidityUrl") && doc["humidityUrl"].is<String>()) {
                    String val = doc["humidityUrl"].as<String>();
                    Application::shared()->getSensors()->setHumidityUrl(sensor, val);
                }

                Application::shared()->getSensors()->saveSettings();
                
                server.send(200, "application/json", sensorJson(sensor));
                return true;
            }

            return false;
        }

        String sensorJson(int sensor) {
            DynamicJsonDocument doc(2048);
            struct SensorValues values = Application::shared()->getSensors()->read(sensor);

            doc["name"] = Application::shared()->getSensors()->getSensorName(sensor);
            doc["temperature"] = values.temperature;
            doc["humidity"] = values.humidity;

            String json = "";
            serializeJson(doc, json);

            return json;
        }
};

#endif
