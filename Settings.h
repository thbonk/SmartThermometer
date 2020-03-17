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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <Preferences.h>

// This class provides access to the settings
class Settings {
  public:
    Settings();

    static Settings * shared();

    void invalidate();

    bool isConfigured();
    void setConfigured(bool configured);

    String & getHostname();
    void     setHostname(String & newHostname);

    String getSsid();
    void   setSsid(String & ssid);

    String getPsk();
    void   setPsk(String & psk);

    uint8_t getUiUpdateFrequency();
    void    setUiUpdateFrequency(uint8_t frequency);

  private:
    static Settings * _shared;

    Preferences   _preferences;
    bool        * _isConfigured;
    String      * _hostname;
    uint8_t     * _updateFreqency; 
};

#endif
