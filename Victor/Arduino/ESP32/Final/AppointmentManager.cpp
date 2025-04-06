#include "AppointmentManager.h"
#include "DisplayManager.h"
#include "DFPlayerHandler.h"

DynamicJsonDocument appointments(2048);
std::vector<String> previousAppointmentIds;

unsigned long lastAppointmentRefresh = 0;
int currentAppointmentIndex = 0;
bool dataRefreshed = false;

void refreshAppointments(unsigned long currentMillis) 
{
  if (!dataRefreshed) 
  {
    fetchAppointments();
    dataRefreshed = true;
    lastAppointmentRefresh = currentMillis;
  }

  if (currentMillis - lastAppointmentRefresh >= APPOINTMENT_REFRESH_INTERVAL) 
  {
    dataRefreshed = false;
  }
}

void fetchAppointments() 
{
  HTTPClient http;
  http.begin(APPOINTMENT_API_URL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) 
  {
    String payload = http.getString();
    DynamicJsonDocument newAppointments(2048);
    deserializeJson(newAppointments, payload);
    checkForAppointmentChanges(newAppointments);
  } 
  
  else 
  {
    updateDisplay("Fetch failed", "Retrying...", SHOWING_INITIALIZATION);
  }

  http.end();
}

void checkForAppointmentChanges(DynamicJsonDocument& newAppointments) 
{
  std::vector<String> currentIds;
  bool changesDetected = false;

  for (JsonObject newApp : newAppointments.as<JsonArray>()) 
  {
    String newAppId = newApp["appointment_id"].as<String>();
    currentIds.push_back(newAppId);

    bool isNew = true;
    bool isModified = false;

    for (String prevId : previousAppointmentIds) 
    {
      if (prevId == newAppId) 
      {
        isNew = false;

        for (JsonObject oldApp : appointments.as<JsonArray>()) 
        {
          if (oldApp["appointment_id"].as<String>() == newAppId) 
          {
            if (oldApp["task"] != newApp["task"] || oldApp["date_hour"] != newApp["date_hour"]) 
            {
              isModified = true;
            }

            break;
          }
        }

        break;
      }
    }

    if ((isNew || isModified) && !nightMode) 
    {
      playAppropriateSound(true);
      changesDetected = true;
    }
  }

  for (String prevId : previousAppointmentIds) 
  {
    bool stillExists = false;
    for (JsonObject newApp : newAppointments.as<JsonArray>()) 
    {
      if (newApp["appointment_id"].as<String>() == prevId) 
      {
        stillExists = true;
        break;
      }
    }

    if (!stillExists && !nightMode) 
    {
      playAppropriateSound(false);
      changesDetected = true;
    }
  }

  if (changesDetected || previousAppointmentIds.empty()) 
  {
    appointments = newAppointments;
    previousAppointmentIds = currentIds;
    currentAppointmentIndex = 0;
  }
}
