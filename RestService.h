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

#ifndef __RESTSERVICE_H__
#define __RESTSERVICE_H__

#include <WebServer.h>
#include "SensorList.h"

class RestService {
    public:
        typedef enum {
            CREATED,
            INITIALIZING,
            STARTED,
            ERROR
        } Status;

        RestService();

        static RestService * shared() {
            return RestService::_shared;
        }

        void loop() {
            _server.handleClient();
        }

        void readSensors();
        void readSensorCount();
        void readSensor();

    private:
        void initializeService();

        Status              _status;
        WebServer           _server;

        static RestService * _shared;
};

#endif
