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
        SensorRequestHandler() : PathArgsRequestHandler("/sensors/{}", HTTP_GET) {}
        ~SensorRequestHandler() {}

        virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) { 
            const int sensors = Application::shared()->getSensors()->count();
            int sensor = server.pathArg(0).toInt();

            if (sensor >= 0 && sensor < sensors) {
                DynamicJsonDocument doc(2048);
                struct SensorValues values = Application::shared()->getSensors()->read(sensor);

                doc["name"] = Application::shared()->getSensors()->getSensorName(sensor);
                doc["temperature"] = values.temperature;
                doc["humidity"] = values.humidity;

                String json = "";
                serializeJson(doc, json);

                server.send(200, "application/json", json);

                return true;
            } else {
                //_server.send(404, "text/plain", "Sensor not found");
                return false;
            }
        }
};

#endif
