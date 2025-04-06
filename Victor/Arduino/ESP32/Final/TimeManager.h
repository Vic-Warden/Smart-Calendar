#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "Config.h"

extern NTPClient timeClient;
extern bool timeInitialized;

void setupNTP();
void updateTime(unsigned long currentMillis);
void initializeTime();
String getCurrentTime();

#endif
