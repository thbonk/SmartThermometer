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

#include <M5ez.h>
#include <Wire.h> //The DHT12 uses I2C comunication.
#include "DHT12.h"
#include "PaHUB.h"

#define PaHub_I2C_ADDRESS  0x70
#define ENV_I2C_ADDRESS    0x5C

struct SensorValues {
  float temperature;
  int   humidity;
};

struct Sensor {
  uint8_t channel;
  DHT12   device;
  char*   name;
};

int textWidth(char* str) {
  m5.lcd.setFont(&FreeMonoBold24pt7b);
  return m5.lcd.textWidth(str);
}

int currentTerrarium = 0;
int currentSensor = -1;
int numberOfSensors = 6;
int sensorsPerTerrarium = 3;
int numberOfTerrariums = numberOfSensors / sensorsPerTerrarium;

int temperatureTextWidth = textWidth("99.9'C");
int humidityTextWidth  = textWidth("100%");

char *terrariumName[] = {
  "Girls", "Boy"
};

PaHUB pahub;

struct Sensor sensors[] = {
  {0, DHT12(), "Left"},
  {1, DHT12(), "Hotspot"},
  {2, DHT12(), "Right"},
  {3, DHT12(), "Left"},
  {4, DHT12(), "Hotspot"},
  {5, DHT12(), "Right"}
};

int nextDrawTime = 0;
int updateFrequencySeconds = 30;
long updateFrequency = updateFrequencySeconds * 1000;


// Initialization
void setup() {
  ez.begin();
  Wire.begin();
  pahub.address(PaHub_I2C_ADDRESS);
}

void loop() {
  dispatchButtonPresses();

  if (nextDrawTime < millis()) {
    nextDrawTime = millis() + updateFrequency;

    showNextSensor();
  }
}

void dispatchButtonPresses() {
  String btnPressed = ez.buttons.poll();

  if (btnPressed == "Settings") {
    showSettings();
    showSensor();
  } else if (btnPressed == "Next") {
    showNextSensor();
  }
}

void showSettings() {
  ez.settings.menu();
}

void showNextSensor() {
  currentSensor++;

  Serial.println(" ---------- showNextSensor() ----------");
  Serial.print("Current Sensor: "); Serial.println(currentSensor);
  
  if (currentSensor >= numberOfSensors) {
    currentSensor = 0;
  }

  Serial.print("Next Sensor: "); Serial.println(currentSensor);
  Serial.print("Current Terrarium: "); Serial.println(currentTerrarium);
  
  currentTerrarium = currentSensor / sensorsPerTerrarium;

  Serial.print("Next Terrarium: "); Serial.println(currentTerrarium);
  
  showSensor();

  Serial.println(" ---------- showNextSensor() ----------");
}

void showSensor() {
  char buffer[50];
  struct Sensor sensor = sensors[currentSensor];
  struct SensorValues sensorValues = readSensor(currentSensor);

  ez.canvas.reset();

  Serial.println(" ---------- showSensor() ----------");

  Serial.print("Current Sensor: "); Serial.println(currentSensor);
  Serial.print("Canvas Width: "); Serial.println(ez.canvas.width());

  sprintf(buffer, "%s - %s", terrariumName[currentTerrarium], sensor.name);
  ez.header.show(buffer);
  ez.buttons.show("Next# #Settings");

  ez.canvas.clear();
  ez.canvas.font(&FreeMonoBold24pt7b);

  if (sensorValues.temperature > 40.0) {
    ez.canvas.color(RED);
  }
  sprintf(buffer, "%2.1f'C", sensorValues.temperature);
  ez.canvas.pos(160 - temperatureTextWidth / 2, 65);
  ez.canvas.print(buffer);

  ez.canvas.color(BLACK);
  sprintf(buffer, "%3d%%", sensorValues.humidity);
  ez.canvas.pos(160 - humidityTextWidth / 2, 135);
  ez.canvas.print(buffer);

  if (sensorValues.temperature > 40.0) {
    alertTone();
  }

  Serial.println(" ---------- showSensor() ----------");
}

struct SensorValues readSensor(int channel) {
  struct SensorValues sensorValues;
  struct Sensor sensor = sensors[channel];
  uint8_t returnCode = pahub.selectChannel(sensor.channel);

  Serial.println(" ---------- readSensor() ----------");
  Serial.print("pahub.selectChannel(");
  Serial.print(sensor.channel);
  Serial.print(") = ");
  Serial.println(returnCode);

  if (returnCode == 0) {
    sensorValues.temperature = sensor.device.readTemperature();
    sensorValues.humidity = sensor.device.readHumidity(); 
  }

  Serial.println(" ---------- readSensor() ----------");

  return sensorValues;
}

void alertTone() {
  int freq[] = { 294, 330, 350, 393, 441, 495, 556, 589, 661, 700, 786, 882, 990 };

  for (int i = 0; i < sizeof(freq)/sizeof(int); i++) {
    M5.Speaker.tone(freq[i], 25);
    delay(25);
  }
}
