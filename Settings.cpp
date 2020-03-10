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

#include "Settings.h"
#include "Constants.h"

#define APP_KEY_NAME        "smrt-thrm"

// Constants for accessing the preferences
#define PREF_IS_CONFIGURED    "isConfigured"
#define PREF_UPDATE_FREQUENCY "updFreq"

Settings * Settings::_shared = NULL;

Settings::Settings() 
  : _preferences(), _isConfigured(NULL), _updateFreqency(NULL) {

  Settings::_shared = this;
}

Settings * Settings::shared() {
    return Settings::_shared;
}

void Settings::invalidate() {
  if (_isConfigured != NULL) {
    delete _isConfigured;
    _isConfigured = NULL;
  }

  if (_updateFreqency != NULL) {
    delete _updateFreqency;
    _updateFreqency = NULL;
  }
}

bool Settings::isConfigured() {
  if (_isConfigured != NULL) {
    return *_isConfigured;
  }

  Serial.printf("_isConfigured points to %d\n", _isConfigured);

  _isConfigured = new bool();
  _preferences.begin(APP_KEY_NAME, true);
  *_isConfigured = _preferences.getBool(PREF_IS_CONFIGURED, false);
  _preferences.end();

  Serial.printf("_isConfigured points to %d\n", _isConfigured);
  Serial.printf("_isConfigured has value %d\n", *_isConfigured);

  return *_isConfigured;
}

void Settings::setConfigured(bool configured) {
  Serial.printf("_isConfigured points to %d\n", _isConfigured);

  if (_isConfigured == NULL) {
    _isConfigured = new bool();
  }

  Serial.printf("_isConfigured points to %d\n", _isConfigured);

  *_isConfigured = configured;
  _preferences.begin(APP_KEY_NAME, false);
  _preferences.putBool(PREF_IS_CONFIGURED, configured);
  _preferences.end();

  Serial.printf("_isConfigured has value %d\n", *_isConfigured);
}

uint8_t Settings::getUpdateFrequency() {
  if (_updateFreqency != NULL) {
    return *_updateFreqency;
  }

  _updateFreqency = new uint8_t();
  _preferences.begin(APP_KEY_NAME, true);
  *_updateFreqency = _preferences.getUChar(PREF_UPDATE_FREQUENCY, 30);
  _preferences.end();

  return *_updateFreqency;
}

void Settings::setUpdateFrequency(uint8_t frequency) {
  if (_updateFreqency == NULL) {
    _updateFreqency = new uint8_t();
  }

  *_updateFreqency = frequency;
  _preferences.begin(APP_KEY_NAME, false);
  _preferences.putUChar(PREF_UPDATE_FREQUENCY, frequency);
  _preferences.end();
}
