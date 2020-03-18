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

#ifndef __SENSORLIST_H__
#define __SENSORLIST_H__

#include <vector>
#include <ArduinoJson.h>
#include "PaHUB.h"
#include "DHT12.h"

struct SensorValues {
  float temperature;
  int   humidity;
};

class Sensor {
    friend class SensorList;

    public:
        ~Sensor();

    protected:
        Sensor(PaHUB * pahub, uint8_t channel, const char * name);

        struct SensorValues read();
        uint8_t      getChannel() { return _channel; }
        const char * getName();
        void         setName(const char * name);
        const char * getTemperatureUrl();
        void         setTemperatureUrl(const char * url);
        const char * getHumidityUrl();
        void         setHumidityUrl(const char * url);

    private:
        PaHUB   * _pahub;
        uint8_t   _channel;
        char    * _name;
        char    * _temperatureUrl;
        char    * _humidityUrl;
        DHT12     _dht12;
};

class SensorList {
    public:
        SensorList();
        ~SensorList();

        const int count();

        struct SensorValues read(int sensorNum);

        String getSensorName(int sensorNum);
        void setSensorName(int sensorNum, String name);

        String getTemperatureUrl(int sensorNum);
        void setTemperatureUrl(int sensorNum, String url);

        String getHumidityUrl(int sensorNum);
        void setHumidityUrl(int sensorNum, String url);

        void saveSettings();

    private:
        void     probeSensors();
        Sensor * getSensor(int index);

        std::vector<Sensor *> _sensors;
        PaHUB                 _pahub;
};

#endif
