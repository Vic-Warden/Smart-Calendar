#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Config.h"

extern LiquidCrystal_I2C lcd;
extern TM1637Display display;
extern DisplayState currentDisplayState;

void setupDisplay();
void setup7Segment();
void updateDisplay(String line1, String line2, DisplayState newState);
void displayAppointments(unsigned long currentMillis);
void handleCharacterNotification(unsigned long currentMillis);

#endif
