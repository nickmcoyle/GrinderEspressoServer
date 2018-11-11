#ifndef TemperatureSensing_h
#define TemperatureSensing_h

/////for Temperature Sensor
//Data wire is plugged into pin 2 on the Arduino
#include <OneWire.h>
#include <DallasTemperature.h>
const int ONE_WIRE_BUS = D6;
//Setup a oneWire instance to communicate with any OneWire devices(not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
//Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void initTemperatureSensor() {
  sensors.begin();
}

#endif
