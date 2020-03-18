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

#include <HTTPClient.h>
#include <M5ez.h>
#include "Application.h"
#include "Settings.h"
#include "ThermometerCanvas.h"

#define BTN_PREVIOUS    "up"
#define BTN_NEXT        "down"
#define BTN_SETTINGS    "Settings"

static int textWidth(char* str) {
    m5.lcd.setFont(&FreeMonoBold24pt7b);
    return m5.lcd.textWidth(str);
}

int ThermometerCanvas::_temperatureTextWidth = textWidth("99.9'C");
int ThermometerCanvas::_humidityTextWidth    = textWidth("100%%");

ThermometerCanvas::ThermometerCanvas() 
    : Canvas(), _nextDrawTime(0), _currentSensor(-1) {

    Serial.print("_temperatureTextWidth = "); Serial.println(_temperatureTextWidth);
    Serial.print("_humidityTextWidth = "); Serial.println(_humidityTextWidth);
}

// Completely draws the whole canves
void ThermometerCanvas::draw() {
    ez.header.show("");
    ez.buttons.show(BTN_PREVIOUS "#" BTN_NEXT "#" BTN_SETTINGS);
    _nextDrawTime = 0;
    update();
}

// update the canvas
void ThermometerCanvas::update() {
    if (_nextDrawTime < millis()) {
        showNextSensor();
    }
}

// handle button presses etc.
void ThermometerCanvas::loop(){
    String btnPressed = ez.buttons.poll();

    if (btnPressed == BTN_SETTINGS) {
        ez.settings.menu();
        draw();
    } else if (btnPressed == BTN_PREVIOUS) {
        showPreviousSensor();
    } else if (btnPressed == BTN_NEXT) {
        showNextSensor();
    }
}

void ThermometerCanvas::showPreviousSensor() {
    _nextDrawTime = millis() + (uint64_t)Settings::shared()->getUiUpdateFrequency() * 1000;

    _currentSensor--;

    if (_currentSensor < 0) {
        _currentSensor = Application::shared()->getSensors()->count() - 1;
    }

    showSensor();
}

void ThermometerCanvas::showNextSensor() {
    _nextDrawTime = millis() + (uint64_t)Settings::shared()->getUiUpdateFrequency() * 1000;

    _currentSensor++;

    if (_currentSensor >= Application::shared()->getSensors()->count()) {
        _currentSensor = 0;
    }

    showSensor();
}

void ThermometerCanvas::showSensor() {
    char buffer[50];
    struct SensorValues sensorValues = Application::shared()->getSensors()->read(_currentSensor);

    ez.canvas.reset();

    Serial.println(" ---------- showSensor() ----------");

    Serial.print("Current Sensor: "); Serial.println(_currentSensor);
    Serial.print("Canvas Width: "); Serial.println(ez.canvas.width());

    ez.header.show(Application::shared()->getSensors()->getSensorName(_currentSensor));

    ez.canvas.clear();
    ez.canvas.font(&FreeMonoBold24pt7b);

    if (sensorValues.temperature > 40.0) {
        ez.canvas.color(RED);
    }
    sprintf(buffer, "%2.1f'C", sensorValues.temperature);
    ez.canvas.pos((ez.canvas.width() - _temperatureTextWidth) / 2, 65);
    ez.canvas.print(buffer);

    ez.canvas.color(BLACK);
    sprintf(buffer, "%3d%%", sensorValues.humidity);
    ez.canvas.pos((ez.canvas.width() - _humidityTextWidth) / 2, 135);
    ez.canvas.print(buffer);

    uploadMeasurements();

    Serial.println(" ---------- showSensor() ----------");
}

void ThermometerCanvas::uploadMeasurements() {
  xTaskCreatePinnedToCore(
        [](void *params) {      /* Function to implement the task */
          Serial.println(" ---------- uploadMeasurements() TASK ----------");
          
          if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi is connected");

            SensorList          * sensors = Application::shared()->getSensors();
            HTTPClient          httpClient;

            for (int i = 0; i < sensors->count(); i++) {
              struct SensorValues sensorValues = sensors->read(i);
              String              temperatureUrl = sensors->getTemperatureUrl(i);
              String              humidityUrl = sensors->getHumidityUrl(i);

              if (temperatureUrl.length() > 0) {
                temperatureUrl.replace("${temperature}", String(sensorValues.temperature));

                httpClient.begin(temperatureUrl);
                int status = httpClient.POST("");
                httpClient.end();

                Serial.printf("Posted to URL %s, HTTP Status = %d", temperatureUrl.c_str(), status);
              }

              if (humidityUrl.length() > 0) {
                humidityUrl.replace("${humidity}", String(sensorValues.humidity));
                
                httpClient.begin(humidityUrl);
                int status = httpClient.POST("");
                httpClient.end();

                Serial.printf("Posted to URL %s, HTTP Status = %d", humidityUrl.c_str(), status);
              }
            }
          }

          Serial.println(" ---------- uploadMeasurements() TASK ----------");
          vTaskDelete(NULL);
        },
        "uploadMeasurements",   /* Name of the task */
        4096,                   /* Stack size in words */
        NULL,                   /* Task input parameter */
        3,                      /* Priority of the task */
        NULL,                   /* Task handle. */
        0);                     /* Core where the task should run */
}
