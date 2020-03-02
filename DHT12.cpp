/*
  DHT12.cpp - Library for DHT12 sensor.
  v0.0.1 Beta
  Created by Bobadas, July 30,2016.
  Released into the public domain.
*/
#include "DHT12.h"

DHT12::DHT12(uint8_t scale,uint8_t dht12_id, uint8_t bmp280_id)
{
  if (dht12_id == 0 || dht12_id > 126) {
    _dht12_id = 0x5c;
  } else {
    _dht12_id = dht12_id;
  }
  if (bmp280_id == 0 || bmp280_id > 126) {
    _bmp280_id = 0x76;
  } else {
    _bmp280_id = bmp280_id;
  }
  if (scale == 0 || scale > 3) {
    _scale = CELSIUS;
  }
  else {
    _scale = scale;
  }
}

uint8_t DHT12::read(uint8_t address)
{ 
  Wire.beginTransmission(address);
  Wire.write(0);
  if (Wire.endTransmission() != 0) {
    return 1;  
  }
  Wire.requestFrom(address, (uint8_t)5);
  for (int i = 0; i < 5; i++) {
    datos[i] = Wire.read();
  };
  delay(50);
  if (Wire.available() != 0) {
    return 2;
  }
  if (datos[4] != (datos[0] + datos[1] + datos[2] + datos[3])) {
    return 3;
  }
  return 0;
}

float DHT12::readTemperature(uint8_t scale)
{
  float resultado=0;
  uint8_t error = read(_dht12_id);

  Serial.println("---------- DHT12::readTemperature ----------");
  Serial.print("error = "); Serial.println(error);
  if (error!=0) return (float)error/100;
  if (scale==0) scale=_scale;
  switch(scale) {
    case CELSIUS:
      resultado=(datos[2]+(float)datos[3]/10);
      break;
    case FAHRENHEIT:
      resultado=((datos[2]+(float)datos[3]/10)*1.8+32);
      break;
    case KELVIN:
      resultado=(datos[2]+(float)datos[3]/10)+273.15;
      break;
  };
  return resultado;
}

float DHT12::readHumidity()
{
  float resultado;
  uint8_t error=read(_bmp280_id);

  Serial.println("---------- DHT12::readHumidity ----------");
  Serial.print("error = "); Serial.println(error);
  
  if (error!=0) return (float)error/100;
  resultado=(datos[0]+(float)datos[1]/10);
  return resultado;
}