#include "SensorHandler.h"
#include "TimeManager.h"
#include "DFPlayerHandler.h"
#include "DisplayManager.h"

int lastButtonState = HIGH;
int lastPIRState = LOW;
int lastLDRValue = -1;

unsigned long lastSensorCheck = 0;
unsigned long lastButtonPress = 0;
unsigned long lastPIRTime = 0;

bool nightMode = false;
bool pirLocked = false;

void setupPins() 
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  lastLDRValue = analogRead(LDR_PIN);
  lastPIRState = digitalRead(PIR_PIN);
  nightMode = (lastLDRValue < NIGHT_MODE_ENTER_THRESHOLD);
}

void handleSensors(unsigned long currentMillis) 
{
  if (currentMillis - lastSensorCheck >= SENSOR_CHECK_INTERVAL) 
  {
    lastSensorCheck = currentMillis;

    int buttonState = digitalRead(BUTTON_PIN);
    int pirState = digitalRead(PIR_PIN);
    int ldrValue = analogRead(LDR_PIN);
    static bool wasInNightMode = nightMode;

    bool newNightMode = (ldrValue < NIGHT_MODE_ENTER_THRESHOLD);

    if (newNightMode != nightMode) 
    {
      nightMode = newNightMode;

      if (dfPlayerInitialized) 
      {
        dfPlayer.play(nightMode ? DARK_TRACK : LIGHT_TRACK);
      }

      wasInNightMode = nightMode;
      sendSensorData(LDR_SENSOR_ID, ldrValue);
    }

    if (buttonState == LOW && lastButtonState == HIGH && (currentMillis - lastButtonPress) >= BUTTON_DEBOUNCE_TIME) 
        {
            sendSensorData(BUTTON_SENSOR_ID, 1);

            switch (currentCharacter) 
            {
                case COMMISSAR:
                currentCharacter = LEMAN_RUSS;
                currentCharacterName = "Leman Russ";
                break;
                case LEMAN_RUSS:
                currentCharacter = BANEBLADE;
                currentCharacterName = "Baneblade";
                break;
                case BANEBLADE:
                currentCharacter = COMMISSAR;
                currentCharacterName = "Commissar";
                break;
            }

            updateDisplay("Switched to", currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);

            if (dfPlayerInitialized && !nightMode) 
            {
                switch (currentCharacter) 
                {
                case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
                case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
                case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
                }
            }

            showingCharacterNotification = true;
            characterNotificationStart = currentMillis;
            lastButtonPress = currentMillis;
        }

    if (pirState == HIGH && lastPIRState == LOW && !pirLocked) 
    {
      sendSensorData(PIR_SENSOR_ID, 1);

      if (dfPlayerInitialized && !nightMode) 
      {
        playRandomSound(PIR_TRACKS, sizeof(PIR_TRACKS) / sizeof(int));
      }

      lastPIRTime = currentMillis;
      pirLocked = true;
    }

    if (pirLocked && currentMillis - lastPIRTime >= PIR_INTERVAL) 
    {
      pirLocked = false;
    }

    lastButtonState = buttonState;
    lastPIRState = pirState;
    lastLDRValue = ldrValue;
  }
}

void sendSensorData(int sensor_id, float value) 
{
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(SENSOR_API_URL);

  String postData = "device_id=" + String(DEVICE_ID) +
    "&time_stamp=" + getCurrentTime() +
    "&sensor_id_1=1&value_1=" + (sensor_id == 1 ? String(value) : "0.0") +
    "&sensor_id_2=2&value_2=" + (sensor_id == 2 ? String(value) : "0.0") +
    "&sensor_id_3=3&value_3=" + (sensor_id == 3 ? String(value) : "0.0");

  http.POST(postData);
  http.end();
}
