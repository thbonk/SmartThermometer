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
#include "SettingsMenu.h"
#include "ThermometerCanvas.h"

#define BTN_PREVIOUS    "up"
#define BTN_NEXT        "down"
#define BTN_SETTINGS    "Settings"

// Completely draws the whole canves
void ThermometerCanvas::draw() {
    ez.header.show("Sensorname");
    ez.buttons.show(BTN_PREVIOUS "#" BTN_NEXT "#" BTN_SETTINGS);
}

// update the canvas
void ThermometerCanvas::update() {

}

// handle button presses etc.
void ThermometerCanvas::loop(){
    String btnPressed = ez.buttons.poll();

    if (btnPressed == BTN_SETTINGS) {
        SettingsMenu settingsMenu;

        settingsMenu.run();
        draw();
    }
}