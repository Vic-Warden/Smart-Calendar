#include "Config.h"
#include "DisplayManager.h"
#include "NetworkManager.h"
#include "TimeManager.h"
#include "AppointmentManager.h"
#include "DFPlayerHandler.h"
#include "SensorHandler.h"
#include "ServoHandler.h"
#include "WebServerHandler.h"

void setup() 
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);
  Wire.begin(LCD_SDA, LCD_SCL);
  
  setupDisplay();
  setup7Segment();
  setupPins();
  setupServo();
  testServo();
  setupDFPlayer();
  connectToWiFi();
  setupNTP();
  setupWebServer();
}

void loop() 
{
  unsigned long currentMillis = millis();

  server.handleClient();
  manageWiFi(currentMillis);
  updateTime(currentMillis);
  refreshAppointments(currentMillis);
  displayAppointments(currentMillis);
  handleSensors(currentMillis);
  handleHourlyServo(currentMillis);
  handleCharacterNotification(currentMillis);
  handleWatchdog(currentMillis);
}
