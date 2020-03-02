#ifndef CLOSEDCUBE_PaHUB_H
#define CLOSEDCUBE_PaHUB_H

#include <Arduino.h>

#define PaHUB_MAX_CHANNELS 6

class PaHUB 
{

    public:
        PaHUB();
        PaHUB(uint8_t address);

        void address(uint8_t address);

        uint8_t getChannel();

        uint8_t selectChannel(uint8_t channel);
        uint8_t nextChannel();

    private:
        uint8_t _address;
        uint8_t _currentChannel;

};


#endif //CLOSEDCUBE_PaHUB_H
