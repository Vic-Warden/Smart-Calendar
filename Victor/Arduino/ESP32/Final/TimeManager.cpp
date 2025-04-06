#include "TimeManager.h"
#include "DisplayManager.h"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
bool timeInitialized = false;

unsigned long lastTimeUpdate = 0;
unsigned long lastNtpRetry = 0;

void setupNTP() 
{
  timeClient.begin();
  initializeTime();
}

void updateTime(unsigned long currentMillis) 
{
  if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL) 
  {
    if (timeInitialized) 
    {
      timeClient.update();
      int hour = timeClient.getHours() + 1;
      if (hour >= 24) hour -= 24;
      int minute = timeClient.getMinutes();
      display.showNumberDecEx(hour * 100 + minute, 0b01000000, true);
    }
    lastTimeUpdate = currentMillis;
  }

  if (!timeInitialized && currentMillis - lastNtpRetry >= NTP_RETRY_INTERVAL) 
  {
    initializeTime();
    lastNtpRetry = currentMillis;
  }
}

void initializeTime() 
{
  if (timeClient.forceUpdate()) 
  {
    timeInitialized = true;
    Serial.println("Time initialized");
    updateDisplay("Time initialized", "Fetching data...", SHOWING_INITIALIZATION);
  } 
  
  else 
  {
    Serial.println("Time init failed");
    updateDisplay("Time init failed", "Retrying...", SHOWING_INITIALIZATION);
  }
}

String getCurrentTime() 
{
  if (!timeInitialized && !timeClient.forceUpdate()) 
  {
    return "1970-01-01 00:00:00";
  }

  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime(&epochTime);
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
  return String(timeString);
}
