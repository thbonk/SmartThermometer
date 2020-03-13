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

#include "SensorList.h"

#define PaHub_I2C_ADDRESS  0x70
#define ENV_I2C_ADDRESS    0x5C

Sensor::Sensor(PaHUB * pahub, uint8_t channel, char * name)
    : _pahub(pahub), _channel(channel), _name(NULL), _dht12() {

    if (name) {
        _name = (char *)malloc(strlen(name) + 1);
        strcpy(_name, name);
    }
}

Sensor::~Sensor() {
    if (_name) {
        free(_name);
    }
}

struct SensorValues Sensor::read() {
    struct SensorValues values;

    _pahub->selectChannel(_channel);
    values.temperature = _dht12.readTemperature();
    values.humidity = _dht12.readHumidity();

    return values;
}

const char * Sensor::getName() {
    return _name;
}

void Sensor::setName(const char * name) {
    if (_name) {
        free(_name);
    }

    _name = (char *)malloc(strlen(name) + 1);
    strcpy(_name, name);
}

SensorList::SensorList() : _pahub(PaHub_I2C_ADDRESS), _sensors() {
    probeSensors();
}

SensorList::~SensorList() {
    // clear 
    while (!_sensors.empty()) {
        Sensor * sensor = _sensors.back();

        _sensors.pop_back();
        delete sensor;
    }
}

const int SensorList::count() {
    return _sensors.size();
}

struct SensorValues SensorList::read(int sensorNum) {
    return getSensor(sensorNum)->read();
}

const char * SensorList::getSensorName(int sensorNum) {
    return getSensor(sensorNum)->getName();
}

Sensor * SensorList::getSensor(int index) {
    return _sensors.at(index);
}

void SensorList::probeSensors() {
    for (int channel = 0; channel < PaHUB_MAX_CHANNELS; channel++) {
        DHT12 dht12;
        uint8_t error;

        _pahub.selectChannel(channel);
        dht12.readTemperature(CELSIUS, &error);

        Serial.printf("Probing sensor at channel %d, error = %d\n", channel, error);

        if (error == 0) {
            char name[32];

            sprintf(name, "Sensor %d", channel + 1);
            _sensors.push_back(new Sensor(&_pahub, channel, name));
        }
    }
}
