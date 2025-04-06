#include "ServoHandler.h"
#include "DisplayManager.h"
#include "NetworkManager.h"
#include "WebServerHandler.h"
#include "TimeManager.h"

Servo servo1;
int servoCurrentPosition = 180;
int servoTargetPosition = 180;
bool servoMoving = false;
unsigned long lastServoUpdate = 0;
unsigned long servoReturnTime = 0;

void setupServo() 
{
  if (!servo1.attach(SERVO_PIN, 500, 2500)) 
  {
    Serial.println("Failed to attach servo!");
  } 
  
  else 
  {
    servo1.write(servoCurrentPosition);
    Serial.println("Servo attached");
  }
}

void testServo() 
{
  updateDisplay("Testing servo...", "", SHOWING_INITIALIZATION);
  Serial.println("Testing servo: 180 -> 90 -> pause -> 180");

  unsigned long testStart = millis();
  unsigned long lastMove = millis();
  int testPhase = 0;
  bool testComplete = false;

  while (!testComplete) 
  {
    unsigned long now = millis();

    server.handleClient();
    if (WiFi.status() != WL_CONNECTED) connectToWiFi();

    switch (testPhase) 
    {
      case 0:
        if (now - lastMove >= SERVO_UPDATE_INTERVAL && servoCurrentPosition > 90) 
        {
          servoCurrentPosition -= SERVO_SPEED;
          servoCurrentPosition = max(servoCurrentPosition, 90);
          servo1.write(servoCurrentPosition);
          lastMove = now;
        } 
        
        else if (servoCurrentPosition <= 90) 
        {
          testPhase = 1;
          testStart = now;
        }

        break;

      case 1:
        if (now - testStart >= 1000) 
        {
          testPhase = 2;
          lastMove = now;
        }

        break;

      case 2:
        if (now - lastMove >= SERVO_UPDATE_INTERVAL && servoCurrentPosition < 180) 
        {
          servoCurrentPosition += SERVO_SPEED;
          servoCurrentPosition = min(servoCurrentPosition, 180);
          servo1.write(servoCurrentPosition);
          lastMove = now;
        } 
        
        else if (servoCurrentPosition >= 180) 
        {
          testComplete = true;
          Serial.println("Servo test complete");
        }

        break;
    }
  }
}

void handleHourlyServo(unsigned long currentMillis) 
{
  static int lastTriggeredHour = -1;

  if (!timeInitialized) return;

  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  if (currentMinute == 0 && currentHour != lastTriggeredHour) 
  {
    servoTargetPosition = 90;
    servoMoving = true;
    Serial.println("Servo moving slowly to 90°");
    lastTriggeredHour = currentHour;
    servoReturnTime = currentMillis + HOURLY_SERVO_DURATION;
  }

  if (servoTargetPosition == 90 && currentMillis >= servoReturnTime) 
  {
    servoTargetPosition = 180;
    servoMoving = true;
    Serial.println("Servo returning slowly to 180°");
  }

  if (currentMillis - lastServoUpdate >= SERVO_UPDATE_INTERVAL) 
  {
    if (servoCurrentPosition != servoTargetPosition) 
    {
      int direction = (servoTargetPosition > servoCurrentPosition) ? 1 : -1;
      servoCurrentPosition += direction * SERVO_SPEED;
      servoCurrentPosition = constrain(servoCurrentPosition, 90, 180);
      servo1.write(servoCurrentPosition);
      lastServoUpdate = currentMillis;
    } 
    
    else if (servoMoving) 
    {
      servoMoving = false;
      Serial.println("Servo reached target position");
    }
  }
}

void handleWatchdog(unsigned long currentMillis) 
{
  static unsigned long lastWatchdogCheck = 0;
  
  if (currentMillis - lastWatchdogCheck >= 60000) 
  {
    lastWatchdogCheck = currentMillis;
    if (millis() > 86400000) 
    {
      ESP.restart();
    }
  }
}
