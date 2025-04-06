#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "Config.h"
#include <WiFiManager.h>

extern WebServer server;
extern WiFiManager wifiManager;


void connectToWiFi();
void manageWiFi(unsigned long currentMillis);

#endif
