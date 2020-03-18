#include <Wire.h>
#include "PaHUB.h"

PaHUB::PaHUB() {
}

PaHUB::PaHUB(uint8_t address):_address(address) {    
}

void PaHUB::address(uint8_t address) {    
  _address = address;
}

uint8_t PaHUB::getChannel() {
  return _currentChannel;
}

uint8_t PaHUB::selectChannel(uint8_t channel) {
  uint8_t result = 0xff;
  if (channel >= 0 && channel < PaHUB_MAX_CHANNELS) {
    Wire.beginTransmission(_address);
    Wire.write( ((uint8_t)1) << (channel));
    
    _currentChannel = channel;
    result = Wire.endTransmission();
  } 
  return result;
}

uint8_t PaHUB::nextChannel() {
  uint8_t nextChannel = _currentChannel + 1;
  if (nextChannel > (PaHUB_MAX_CHANNELS-1)) {
    nextChannel = 0;  
  }

  return selectChannel(nextChannel);
}
 
