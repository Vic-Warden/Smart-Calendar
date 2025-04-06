#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include "Config.h"

void handleSensors(unsigned long currentMillis);
void setupPins();
void sendSensorData(int sensor_id, float value);

extern bool nightMode;

#endif
