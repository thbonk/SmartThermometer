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
#define PREF_IS_CONFIGURED        "isConfigured"
#define PREF_UI_UPDATE_FREQUENCY  "uiUpdFreq"
#define PREF_HOSTNAME             "hostname"

Settings * Settings::_shared = NULL;

Settings::Settings() 
  : _preferences()
  , _isConfigured(NULL)
  , _hostname(NULL)
  , _updateFreqency(NULL) {

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

  if (_hostname != NULL) {
    delete _hostname;
    _hostname = NULL;
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

String & Settings::getHostname() {
  if (_hostname != NULL) {
    return *_hostname;
  }

  _hostname = new String();
  _preferences.begin(APP_KEY_NAME, true);
  *_hostname = _preferences.getString(PREF_HOSTNAME, APPLICATION_NAME);
  _preferences.end();

  return *_hostname;
}

void Settings::setHostname(String & newHostname) {
  if (_hostname == NULL) {
    _hostname = new String();
  }

  *_hostname = newHostname;
  _preferences.begin(APP_KEY_NAME, false);
  _preferences.putBool(PREF_HOSTNAME, newHostname);
  _preferences.end();
}

String Settings::getSsid() {
  Preferences wifiPrefs;
  String ssid;

  wifiPrefs.begin("wifi-config");
  ssid = wifiPrefs.getString("WIFI_SSID");
  wifiPrefs.end();

  return ssid;
}

void Settings::setSsid(String & ssid) {
  Preferences wifiPrefs;

  wifiPrefs.begin("wifi-config");
  wifiPrefs.putString("WIFI_SSID", ssid);
  wifiPrefs.end();
}

String Settings::getPsk() {
  Preferences wifiPrefs;
  String psk;

  wifiPrefs.begin("wifi-config");
  psk = wifiPrefs.getString("WIFI_PASSWD");
  wifiPrefs.end();

  return psk;
}

void Settings::setPsk(String & psk) {
  Preferences wifiPrefs;

  wifiPrefs.begin("wifi-config");
  wifiPrefs.putString("WIFI_PASSWD", psk);
  wifiPrefs.end();
}

uint8_t Settings::getUiUpdateFrequency() {
  if (_updateFreqency != NULL) {
    return *_updateFreqency;
  }

  _updateFreqency = new uint8_t();
  _preferences.begin(APP_KEY_NAME, true);
  *_updateFreqency = _preferences.getUChar(PREF_UI_UPDATE_FREQUENCY, 30);
  _preferences.end();

  return *_updateFreqency;
}

void Settings::setUiUpdateFrequency(uint8_t frequency) {
  if (_updateFreqency == NULL) {
    _updateFreqency = new uint8_t();
  }

  *_updateFreqency = frequency;
  _preferences.begin(APP_KEY_NAME, false);
  _preferences.putUChar(PREF_UI_UPDATE_FREQUENCY, frequency);
  _preferences.end();
}
