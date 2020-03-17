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

#ifndef __SETTINGSREQUESTHANDLER_H__
#define __SETTINGSREQUESTHANDLER_H__

#include <ArduinoJson.h>
#include "PathArgsRequestHandler.h"
#include "../Settings.h"

class SettingsRequestHandler : public PathArgsRequestHandler {
    public:
        SettingsRequestHandler() : PathArgsRequestHandler("/settings", (HTTPMethod)(HTTP_GET | HTTP_POST)) {}

        virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) override { 
            if (requestMethod == HTTP_GET) {
                return handleGet(server, requestUri);
            } else if (requestMethod == HTTP_POST) {
                return handlePost(server, requestUri);
            }
            
            return false;
        }

    private:
        bool handleGet(WebServer& server, String requestUri) {
            server.send(200, "application/json", settingsJson());
            return true;
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

            String body = server.arg(0);
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, body);
            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());

                server.send(400, "text/plain", String("Invalid body, error = ") + String(error.c_str()));
                return true;
            }

            if (doc.containsKey("hostname") && doc["hostname"].is<String>()) {
                String val = doc["hostname"].as<String>();
                Settings::shared()->setHostname(val);
            }
            if (doc.containsKey("ssid") && doc["ssid"].is<String>()) {
                String val = doc["ssid"].as<String>();
                Settings::shared()->setSsid(val);
            }
            if (doc.containsKey("psk") && doc["psk"].is<String>()) {
                String val = doc["psk"].as<String>();
                Settings::shared()->setPsk(val);
            }
            if (doc.containsKey("uiUpdateFrequency") && doc["uiUpdateFrequency"].is<int>()) {
                Settings::shared()->setUiUpdateFrequency(doc["uiUpdateFrequency"].as<int>());
            }

            server.send(200, "application/json", settingsJson());
            return true;
        }

        String settingsJson() {
            DynamicJsonDocument doc(2048);

            doc["hostname"] = Settings::shared()->getHostname();
            doc["ssid"] = Settings::shared()->getSsid();
            doc["psk"] = Settings::shared()->getPsk();
            doc["uiUpdateFrequency"] = Settings::shared()->getUiUpdateFrequency();

            String json = "";
            serializeJson(doc, json);

            return json;
        }
};

#endif
