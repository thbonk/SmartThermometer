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

#ifndef __THERMOMETERCANVAS_H__
#define __THERMOMETERCANVAS_H__

#include <M5Stack.h>
#include "Canvas.h"

class ThermometerCanvas: public Canvas {
    public:
        ThermometerCanvas();

        // Completely draws the whole canves
        virtual void draw();

        // update the canvas
        virtual void update();

        // handle button presses etc.
        virtual void loop();

    private:
        void showPreviousSensor();
        void showNextSensor();
        void showSensor();

        unsigned long   _nextDrawTime;
        int16_t         _currentSensor;

        static int      _temperatureTextWidth;
        static int      _humidityTextWidth;
};

#endif
