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
#include "Settings.h"

#define PaHub_I2C_ADDRESS  0x70
#define ENV_I2C_ADDRESS    0x5C

Sensor::Sensor(PaHUB * pahub, uint8_t channel, const char * name)
    : _pahub(pahub), _channel(channel), _name(NULL), _dht12()
    , _temperatureUrl(NULL), _humidityUrl(NULL) {

    if (name) {
        _name = (char *)malloc(strlen(name) + 1);
        strcpy(_name, name);
    }

    setTemperatureUrl("");
    setHumidityUrl("");
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

const char * Sensor::getTemperatureUrl() {
    return _temperatureUrl;
}
void Sensor::setTemperatureUrl(const char * url) {
    if (_temperatureUrl) {
        free(_temperatureUrl);
    }

    _temperatureUrl = (char *)malloc(strlen(url) + 1);
    strcpy(_temperatureUrl, url);
}

const char * Sensor::getHumidityUrl() {
    return _humidityUrl;
}

void Sensor::setHumidityUrl(const char * url) {
    if (_humidityUrl) {
        free(_humidityUrl);
    }

    _humidityUrl = (char *)malloc(strlen(url) + 1);
    strcpy(_humidityUrl, url);
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

String SensorList::getSensorName(int sensorNum) {
    return String(getSensor(sensorNum)->getName());
}

void SensorList::setSensorName(int sensorNum, String name) {
    getSensor(sensorNum)->setName(name.c_str());
}

String SensorList::getTemperatureUrl(int sensorNum) {
    return String(getSensor(sensorNum)->getTemperatureUrl());
}

void SensorList::setTemperatureUrl(int sensorNum, String url) {
    getSensor(sensorNum)->setTemperatureUrl(url.c_str());
}

String SensorList::getHumidityUrl(int sensorNum) {
    return String(getSensor(sensorNum)->getHumidityUrl());
}

void SensorList::setHumidityUrl(int sensorNum, String url) {
    getSensor(sensorNum)->setHumidityUrl(url.c_str());
}

Sensor * SensorList::getSensor(int index) {
    return _sensors.at(index);
}

void SensorList::saveSettings() {
    Settings::shared()->removeSensorConfigurations();

    Settings::shared()->setConfiguredSensors(count());

    for (int i = 0; i < count(); i++) {
        Sensor * sensor = getSensor(i);

        Settings::shared()->setSensorName(sensor->getChannel(), getSensorName(i));
        Settings::shared()->setSensorTemperatureUrl(sensor->getChannel(), String(sensor->getTemperatureUrl()));
        Settings::shared()->setSensorHumidityUrl(sensor->getChannel(), String(sensor->getHumidityUrl()));
    }
}

void SensorList::probeSensors() {
    for (int channel = 0; channel < PaHUB_MAX_CHANNELS; channel++) {
        DHT12 dht12;
        uint8_t error;

        _pahub.selectChannel(channel);
        dht12.readTemperature(CELSIUS, &error);

        Serial.printf("Probing sensor at channel %d, error = %d\n", channel, error);

        if (error == 0) {
            Sensor * sensor = new Sensor(&_pahub, channel, Settings::shared()->getSensorName(channel).c_str());

            sensor->setTemperatureUrl(Settings::shared()->getSensorTemperatureUrl(channel).c_str());
            sensor->setHumidityUrl(Settings::shared()->getSensorHumidityUrl(channel).c_str());

            _sensors.push_back(sensor);
        }
    }
}
