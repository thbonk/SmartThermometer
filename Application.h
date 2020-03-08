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

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Canvas.h"

class Application {
    public:
        Application();

        static Application * shared();

        void setup();
        void loop();

        void showCanvas(Canvas * canvas);

    private:
        static Application * _shared;

        Canvas   * _currentCanvas;
};

#endif
