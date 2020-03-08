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
#include <Wire.h>
#include "Application.h"
#include "Settings.h"
#include "SettingsMenu.h"
#include "ThermometerCanvas.h"

Application * Application::_shared = NULL;

Application::Application() : _currentCanvas(NULL) {
    Application::_shared = this;
}

Application * Application::shared() {
    return Application::_shared;
}

void Application::setup() {
    ez.begin();
    Wire.begin();

    if (!Settings::shared()->isConfigured()) {
        SettingsMenu settingsMenu;

        settingsMenu.run();
        Settings::shared()->setConfigured(true);
    }

    showCanvas(new ThermometerCanvas());
}

void Application::loop() {
    if (_currentCanvas != NULL) {
        _currentCanvas->update();
        _currentCanvas->loop();
    }
}

void Application::showCanvas(Canvas * canvas) {
    if (_currentCanvas) {
        delete _currentCanvas;
        _currentCanvas = NULL;
    }

    _currentCanvas = canvas;
    _currentCanvas->draw();
}
