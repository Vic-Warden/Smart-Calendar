#ifndef SERVO_HANDLER_H
#define SERVO_HANDLER_H

#include "Config.h"

void setupServo();
void testServo();
void handleHourlyServo(unsigned long currentMillis);
void handleWatchdog(unsigned long currentMillis);

#endif