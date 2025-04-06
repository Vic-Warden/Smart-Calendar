#include "NetworkManager.h"
#include "DisplayManager.h"
#include "TimeManager.h"

WiFiManager wifiManager;
unsigned long lastReconnectAttempt = 0;
static unsigned long wifiTimeout = 0;

void connectToWiFi() {
  updateDisplay("Connecting WiFi", "", SHOWING_INITIALIZATION);

  // Custom SSID & Password portal name
  wifiManager.setTimeout(180);  // 3 minutes timeout
  bool connected = wifiManager.autoConnect("SmartCalendarAP", "warhammer42");

  if (connected) {
    Serial.println("WiFi connected via WiFiManager!");
    updateDisplay("WiFi Connected", "Syncing time...", SHOWING_INITIALIZATION);
    initializeTime();
  } else {
    Serial.println("Failed to connect and timeout expired.");
    updateDisplay("WiFi Failed", "AP Timeout", SHOWING_INITIALIZATION);
    // Optionally restart or go to deep sleep
    delay(5000);
    ESP.restart();
  }
}

void manageWiFi(unsigned long currentMillis) 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    if (currentMillis - lastReconnectAttempt >= WIFI_RECONNECT_DELAY) 
    {
      if (wifiTimeout == 0) wifiTimeout = currentMillis + 30000;

      if (currentMillis < wifiTimeout) 
      {
        lastReconnectAttempt = currentMillis;
        connectToWiFi();
      } 
      
      else 
      {
        Serial.println("WiFi connection timeout, continuing offline");
        wifiTimeout = 0;
      }
    }
  } 
  
  else 
  {
    wifiTimeout = 0;
  }
}
