#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// LCD Configuration
#define LCD_SDA 21
#define LCD_SCL 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 7-segment Display
#define CLK_PIN 18
#define DIO_PIN 19
TM1637Display display(CLK_PIN, DIO_PIN);

// NTP Client
WiFiUDP ntpUDP;
WebServer server(80);
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
bool timeInitialized = false;

// DFPlayer Mini
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;

// Servo Motor
static const int servoPin = 27;
Servo servo1;
int servoCurrentPosition = 180;      
int servoTargetPosition = 180;       
const int SERVO_SPEED = 1;          
const unsigned long SERVO_UPDATE_INTERVAL = 20;
unsigned long lastServoUpdate = 0;
unsigned long servoReturnTime = 0;
const unsigned long HOURLY_SERVO_DURATION = 60000;

// Character Modes
enum CharacterMode 
{
  COMMISSAR,
  LEMAN_RUSS,
  BANEBLADE
};

CharacterMode currentCharacter = COMMISSAR;
String currentCharacterName = "Commissar";

// WiFi Credentials
const char* SSID = "DeKey-Fraijlemaborg";
const char* PASSWORD = "i3xHp*ag";

// API Endpoints
const char* APPOINTMENT_API_URL = "http://100.74.252.69/Database/Appointment/recover_appointment.php";
const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";

// Device Configuration
const int DEVICE_ID = 1;
const int BUTTON_SENSOR_ID = 1;
const int PIR_SENSOR_ID = 2;
const int LDR_SENSOR_ID = 3;

// Timing Constants
const unsigned long APPOINTMENT_REFRESH_INTERVAL = 5000;
const unsigned long DISPLAY_INTERVAL = 3000;
const unsigned long SENSOR_CHECK_INTERVAL = 50;
const unsigned long TIME_UPDATE_INTERVAL = 60000;
const unsigned long NTP_RETRY_INTERVAL = 30000;
const unsigned long BUTTON_DEBOUNCE_TIME = 50;
const unsigned long WIFI_RECONNECT_DELAY = 1000;
const unsigned long CHARACTER_NOTIFICATION_DURATION = 2000;
const long pirInterval = 300000;

// Sound Tracks
const int COMMISSAR_TRACK = 31;
const int LEMAN_RUSS_TRACK = 42;
const int BANEBLADE_TRACK = 53;
const int DARK_TRACK = 35;
const int LIGHT_TRACK = 36;
const int PIR_TRACKS[] = {37, 38, 39, 40, 41};

// Appointment Sound Tracks
const int COMMISSAR_ACTIVE_TRACKS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 28, 33, 34};
const int COMMISSAR_DELETED_TRACKS[] = {11, 20, 23, 24, 25, 26, 27, 29, 30, 32};
const int LEMAN_RUSS_ACTIVE_TRACKS[] = {43, 44, 45, 46, 47, 48};
const int LEMAN_RUSS_DELETED_TRACKS[] = {49, 50, 51};
const int BANEBLADE_ACTIVE_TRACKS[] = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 67, 68, 69, 70, 71, 72, 73, 74};
const int BANEBLADE_DELETED_TRACKS[] = {64, 65, 66};

// Night Mode Thresholds
const int NIGHT_MODE_ENTER_THRESHOLD = 650;
const int NIGHT_MODE_EXIT_THRESHOLD = 750;

// Sensor Pins
const int BUTTON_PIN = 14;
const int PIR_PIN = 35;
const int LDR_PIN = 34;

// Global Variables
DynamicJsonDocument appointments(2048);
std::vector<String> previousAppointmentIds;
unsigned long lastAppointmentRefresh = 0;
unsigned long lastDisplayChange = 0;
unsigned long lastSensorCheck = 0;
unsigned long lastTimeUpdate = 0;
unsigned long lastNtpRetry = 0;
unsigned long lastButtonPress = 0;
unsigned long lastReconnectAttempt = 0;
unsigned long lastHourCheck = 0;
unsigned long characterNotificationStart = 0;
unsigned long lastPIRTime = 0;
int currentAppointmentIndex = 0;
int lastButtonState = HIGH;
int lastPIRState = LOW;
int lastLDRValue = -1;
bool dfPlayerInitialized = false;
bool dataRefreshed = false;
bool nightMode = false;
bool showingCharacterNotification = false;
bool pirLocked = false;

enum DisplayState 
{
  SHOWING_CHARACTER_NOTIFICATION,
  SHOWING_APPOINTMENTS,
  SHOWING_NO_APPOINTMENTS,
  SHOWING_INITIALIZATION
};

DisplayState currentDisplayState = SHOWING_INITIALIZATION;

void updateDisplay(String line1, String line2, DisplayState newState) 
{
  static String lastLine1 = "";
  static String lastLine2 = "";
  static DisplayState lastState = SHOWING_INITIALIZATION;

  if (newState != lastState || line1 != lastLine1 || line2 != lastLine2) 
  {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16));

    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));

    lastLine1 = line1;
    lastLine2 = line2;
    lastState = newState;
    currentDisplayState = newState;
  }
}

void update7SegmentDisplay() 
{
  if (!timeInitialized) {
    display.showNumberDec(9999);
    return;
  }
  
  timeClient.update();
  int hour = timeClient.getHours() + 1;
  if (hour >= 24) hour -= 24;
  int minute = timeClient.getMinutes();
  display.showNumberDecEx(hour * 100 + minute, 0b01000000, true);
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

void connectToWiFi() 
{
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi...");
  updateDisplay("Connecting WiFi", "", SHOWING_INITIALIZATION);
  
  WiFi.begin(SSID, PASSWORD);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) 
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("\nWiFi connected");
    updateDisplay("WiFi Connected", "Syncing time...", SHOWING_INITIALIZATION);
    initializeTime();
  } 
  
  else 
  {
    Serial.println("\nWiFi failed");
    updateDisplay("WiFi Failed", "Retrying...", SHOWING_INITIALIZATION);
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
    lastDisplayChange = millis();
  }
}

void playAppropriateSound(bool isActiveOrUpdated) 
{
  if (!dfPlayerInitialized || nightMode) return;
  
  switch(currentCharacter) 
  {
    case COMMISSAR:
      playRandomSound(isActiveOrUpdated ? COMMISSAR_ACTIVE_TRACKS : COMMISSAR_DELETED_TRACKS, 
                     isActiveOrUpdated ? sizeof(COMMISSAR_ACTIVE_TRACKS)/sizeof(int) : 
                                        sizeof(COMMISSAR_DELETED_TRACKS)/sizeof(int));
      break;
    case LEMAN_RUSS:
      playRandomSound(isActiveOrUpdated ? LEMAN_RUSS_ACTIVE_TRACKS : LEMAN_RUSS_DELETED_TRACKS,
                     isActiveOrUpdated ? sizeof(LEMAN_RUSS_ACTIVE_TRACKS)/sizeof(int) : 
                                        sizeof(LEMAN_RUSS_DELETED_TRACKS)/sizeof(int));
      break;
    case BANEBLADE:
      playRandomSound(isActiveOrUpdated ? BANEBLADE_ACTIVE_TRACKS : BANEBLADE_DELETED_TRACKS,
                     isActiveOrUpdated ? sizeof(BANEBLADE_ACTIVE_TRACKS)/sizeof(int) : 
                                        sizeof(BANEBLADE_DELETED_TRACKS)/sizeof(int));
      break;
  }
}

void displayCurrentAppointment(unsigned long currentMillis) 
{
  if (showingCharacterNotification) 
  {
    return;
  }
  
  if (appointments.size() == 0) 
  {
    if (currentDisplayState != SHOWING_NO_APPOINTMENTS) 
    {
      updateDisplay("No appointments", "", SHOWING_NO_APPOINTMENTS);
    }
    return;
  }

  if (currentMillis - lastDisplayChange >= DISPLAY_INTERVAL) 
  {
    currentAppointmentIndex = (currentAppointmentIndex + 1) % appointments.size();
    lastDisplayChange = currentMillis;
  }

  JsonObject appointment = appointments[currentAppointmentIndex];
  String task = appointment["task"].as<String>();
  String dateTime = appointment["date_hour"].as<String>();

  String line2 = "";
  
  if (dateTime.length() >= 16) 
  {
    line2 = dateTime.substring(8, 10) + "/" + 
            dateTime.substring(5, 7) + " " + 
            dateTime.substring(11, 16);
  } 
  
  else 
  {
    line2 = "Invalid date";
  }

  if (task.length() > 16) 
  {
    task = task.substring(0, 16);
  }

  updateDisplay(task, line2, SHOWING_APPOINTMENTS);
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
      
      switch(currentCharacter) 
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
        switch(currentCharacter) 
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
        playRandomSound(PIR_TRACKS, sizeof(PIR_TRACKS)/sizeof(int));
      }

      lastPIRTime = currentMillis;
      pirLocked = true;
    }

    if (pirLocked && currentMillis - lastPIRTime >= pirInterval) 
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

void playRandomSound(const int tracks[], int count) 
{
  if (!dfPlayerInitialized || count <= 0 || nightMode) return;
  
  static unsigned long lastPlayTime = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastPlayTime > 100) 
  {
    int randomIndex = random(0, count);
    dfPlayer.play(tracks[randomIndex]);
    lastPlayTime = currentMillis;
  }
}

void setupServer() 
{
  server.on("/", HTTP_GET, []() 
  {
    server.send(200, "text/plain", "Smart Calendar is online!");
  });

  server.on("/switch_character", HTTP_GET, []() 
  {
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
    characterNotificationStart = millis();

    server.send(200, "text/plain", "Character changed to " + currentCharacterName);
  });

  server.on("/play_random", HTTP_GET, []() 
  {
    if (dfPlayerInitialized && !nightMode) 
    {
      switch (currentCharacter) 
      {
        case COMMISSAR:
          playRandomSound(COMMISSAR_ACTIVE_TRACKS, sizeof(COMMISSAR_ACTIVE_TRACKS) / sizeof(int));
          break;
        case LEMAN_RUSS:
          playRandomSound(LEMAN_RUSS_ACTIVE_TRACKS, sizeof(LEMAN_RUSS_ACTIVE_TRACKS) / sizeof(int));
          break;
        case BANEBLADE:
          playRandomSound(BANEBLADE_ACTIVE_TRACKS, sizeof(BANEBLADE_ACTIVE_TRACKS) / sizeof(int));
          break;
      }

      server.send(200, "text/plain", "Random voice line played.");
    } 
    
    else 
    {
      server.send(503, "text/plain", "Night mode active or DFPlayer unavailable.");
    }
  });

  server.begin();
  Serial.println("HTTP server started!");
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
    Serial.println("Servo moving slowly to 90°");
    lastTriggeredHour = currentHour;
    servoReturnTime = currentMillis + HOURLY_SERVO_DURATION;
  }
  
  if (servoTargetPosition == 90 && currentMillis >= servoReturnTime) 
  {
    servoTargetPosition = 180;
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
    }

    lastServoUpdate = currentMillis;
  }
}

void setup() 
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

  Wire.begin(LCD_SDA, LCD_SCL);
  
  servo1.attach(servoPin);
  servo1.write(servoCurrentPosition);
  
  if (dfPlayer.begin(mySoftwareSerial)) 
  {
    dfPlayer.volume(7);
    dfPlayerInitialized = true;
    Serial.println("DFPlayer initialized");
  } 
  
  else 
  {
    Serial.println("DFPlayer initialization failed!");
  }

  lcd.init();
  lcd.backlight();
  updateDisplay("Initializing...", "", SHOWING_INITIALIZATION);

  display.setBrightness(0x0a);
  display.clear();
  display.showNumberDec(8888);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  lastLDRValue = analogRead(LDR_PIN);
  lastPIRState = digitalRead(PIR_PIN);
  nightMode = (lastLDRValue < NIGHT_MODE_ENTER_THRESHOLD);

  connectToWiFi();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  initializeTime();
  setupServer();
}

void loop() 
{
  unsigned long currentMillis = millis();

  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) 
  {
    if (currentMillis - lastReconnectAttempt >= WIFI_RECONNECT_DELAY) 
    {
      lastReconnectAttempt = currentMillis;
      connectToWiFi();
    }
    return;
  }

  if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL) 
  {
    update7SegmentDisplay();
    lastTimeUpdate = currentMillis;
  }

  if (!timeInitialized && currentMillis - lastNtpRetry >= NTP_RETRY_INTERVAL) 
  {
    initializeTime();
    lastNtpRetry = currentMillis;
  }

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

  displayCurrentAppointment(currentMillis);
  handleSensors(currentMillis);
  handleHourlyServo(currentMillis);

  if (showingCharacterNotification && currentMillis - characterNotificationStart >= CHARACTER_NOTIFICATION_DURATION) 
  {
    showingCharacterNotification = false;
    currentDisplayState = SHOWING_APPOINTMENTS;
    lastDisplayChange = 0;
  }
}