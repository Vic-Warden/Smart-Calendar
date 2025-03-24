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

// LCD 
#define LCD_SDA 21
#define LCD_SCL 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 7-segment
#define CLK_PIN 18
#define DIO_PIN 19
TM1637Display display(CLK_PIN, DIO_PIN);

// NTP 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
bool timeInitialized = false;

// DFPlayer
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;

// WiFi
const char* SSID = "DeKey-Fraijlemaborg";
const char* PASSWORD = "i3xHp*ag";

// API Endpoints
const char* APPOINTMENT_API_URL = "http://100.74.252.69/Database/Appointment/recover_appointment.php";
const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";

// Device and Sensor ID
const int DEVICE_ID = 1;
const int BUTTON_SENSOR_ID = 1;
const int PIR_SENSOR_ID = 2;
const int LDR_SENSOR_ID = 3;

// Timing
const unsigned long APPOINTMENT_REFRESH_INTERVAL = 5000;
const unsigned long DISPLAY_INTERVAL = 3000;
const unsigned long SENSOR_CHECK_INTERVAL = 50;      
const unsigned long TIME_UPDATE_INTERVAL = 60000;
const unsigned long NTP_RETRY_INTERVAL = 30000;
const unsigned long BUTTON_DEBOUNCE_TIME = 50;      
const unsigned long WIFI_RECONNECT_DELAY = 1000;

// Sound 
const int BUTTON_TRACKS_MIN = 1;
const int BUTTON_TRACKS_MAX = 37;
const int PIR_TRACK = 2;
const int DARK_TRACK = 3;
const int LIGHT_TRACK = 4;
const int ACTIVE_TRACKS[] = {5, 6, 7};
const int UPDATED_TRACKS[] = {8, 9, 10};
const int DELETED_TRACKS[] = {11, 12, 13};

// Sensor
const int BUTTON_PIN = 14;
const int PIR_PIN = 35;
const int LDR_PIN = 34;

// Variables
DynamicJsonDocument appointments(2048);
std::vector<String> previousAppointmentIds;
unsigned long lastAppointmentRefresh = 0;
unsigned long lastDisplayChange = 0;
unsigned long lastSensorCheck = 0;
unsigned long lastTimeUpdate = 0;
unsigned long lastNtpRetry = 0;
unsigned long lastButtonPress = 0;
unsigned long lastReconnectAttempt = 0;
int currentAppointmentIndex = 0;
bool dfPlayerInitialized = false;
bool dataRefreshed = false;

// Sensor 
int lastButtonState = HIGH;
int lastPIRState = LOW;
int lastLDRValue = -1;
bool pirLocked = false;
unsigned long lastPIRTime = 0;
const long pirInterval = 300000;

void setup() 
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

  Wire.begin(LCD_SDA, LCD_SCL);
  
  if (dfPlayer.begin(mySoftwareSerial)) 
  {
    dfPlayer.volume(5);
    dfPlayerInitialized = true;
    Serial.println("DFPlayer initialized");
  } else {
    Serial.println("DFPlayer initialization failed!");
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  display.setBrightness(0x0a);
  display.clear();
  display.showNumberDec(8888);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  lastLDRValue = analogRead(LDR_PIN);
  lastPIRState = digitalRead(PIR_PIN);

  connectToWiFi();

  timeClient.begin();
  initializeTime();
}

void loop() 
{
  unsigned long currentMillis = millis();

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

  handleAppointments(currentMillis);

  handleSensors(currentMillis);
}

void initializeTime() 
{
  Serial.println("Initializing NTP time...");
  
  if (timeClient.forceUpdate()) 
  {
    timeInitialized = true;
    Serial.println("Time initialized successfully");
  } 
  
  else 
  {
    Serial.println("Failed to initialize time");
    timeInitialized = false;
  }
}

void connectToWiFi() 
{
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi...");
  lcd.clear();
  lcd.print("Connecting WiFi");
  
  WiFi.disconnect(true);
  WiFi.begin(SSID, PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) 
  {
    if (millis() - startAttemptTime > 500) 
    {
      Serial.print(".");
      startAttemptTime = millis();
    }
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.print("WiFi Connected");
    timeInitialized = false;
    initializeTime();
  } 
  
  else 
  {
    Serial.println("\nFailed to connect");
    lcd.clear();
    lcd.print("WiFi Failed");
  }
}

void update7SegmentDisplay() 
{
  if (!timeInitialized) {
    display.showNumberDec(9999);
    return;
  }
  
  timeClient.update();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  display.showNumberDecEx(hour * 100 + minute, 0b01000000, true);
}

void handleAppointments(unsigned long currentMillis) 
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

  displayCurrentAppointment(currentMillis);
}

void fetchAppointments() 
{
  HTTPClient http;
  http.setTimeout(5000);
  
  if (!http.begin(APPOINTMENT_API_URL)) 
  {
    Serial.println("Failed to begin HTTP connection");
    return;
  }

  int httpCode = http.GET();
  
  if (httpCode > 0) 
  {
    if (httpCode == HTTP_CODE_OK) 
    {
      String payload = http.getString();
      DynamicJsonDocument newAppointments(2048);
      DeserializationError error = deserializeJson(newAppointments, payload);

      if (error) 
      {
        Serial.print("JSON deserialization failed: ");
        Serial.println(error.c_str());
      } 
      
      else 
      {
        checkForAppointmentChanges(newAppointments);
      }
    } 
    
    else 
    {
      Serial.printf("HTTP error code: %d\n", httpCode);
    }
  } 
  
  else 
  {
    Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
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
          if (oldApp["appointment_id"] == newAppId) 
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

    if (isNew) 
    {
      playRandomSound(ACTIVE_TRACKS, sizeof(ACTIVE_TRACKS)/sizeof(int));
      changesDetected = true;
    } 
    
    else if (isModified) 
    {
      playRandomSound(UPDATED_TRACKS, sizeof(UPDATED_TRACKS)/sizeof(int));
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

    if (!stillExists) 
    {
      playRandomSound(DELETED_TRACKS, sizeof(DELETED_TRACKS)/sizeof(int));
      changesDetected = true;
    }
  }

  if (changesDetected || previousAppointmentIds.size() == 0) 
  {
    appointments = newAppointments;
    previousAppointmentIds = currentIds;
    currentAppointmentIndex = 0;
    lastDisplayChange = millis();
  }
}

void displayCurrentAppointment(unsigned long currentMillis) 
{
  static int lastDisplayedIndex = -1;
  
  if (appointments.size() == 0) 
  {
    if (lastDisplayedIndex != -1) 
    {
      lcd.clear();
      lastDisplayedIndex = -1;
    }
    lcd.setCursor(0, 0);
    lcd.print("No appointments");
    return;
  }

  if (currentMillis - lastDisplayChange >= DISPLAY_INTERVAL) 
  {
    currentAppointmentIndex = (currentAppointmentIndex + 1) % appointments.size();
    lastDisplayChange = currentMillis;
  }

  if (currentAppointmentIndex != lastDisplayedIndex) 
  {
    lcd.clear();
    lastDisplayedIndex = currentAppointmentIndex;
    
    JsonObject appointment = appointments[currentAppointmentIndex];
    String task = appointment["task"].as<String>();
    String dateTime = appointment["date_hour"].as<String>();

    lcd.setCursor(0, 0);
    lcd.print("RDV ");
    lcd.print(currentAppointmentIndex + 1);
    lcd.print("/");
    lcd.print(appointments.size());
    lcd.print(" ");
    
    int displayLength = min(8, (int)task.length());
    lcd.print(task.substring(0, displayLength));

    lcd.setCursor(0, 1);
    if (dateTime.length() >= 16) 
    {
      lcd.print(dateTime.substring(8, 10));
      lcd.print("/");
      lcd.print(dateTime.substring(5, 7));
      lcd.print(" ");
      lcd.print(dateTime.substring(11, 16));
    } 
    
    else 
    {
      lcd.print("Invalid date");
    }
  }
}

void handleSensors(unsigned long currentMillis) 
{
  if (currentMillis - lastSensorCheck >= SENSOR_CHECK_INTERVAL) 
  {
    lastSensorCheck = currentMillis;
    
    int buttonState = digitalRead(BUTTON_PIN);
    int pirState = digitalRead(PIR_PIN);
    int ldrValue = analogRead(LDR_PIN);

    if (buttonState == LOW && lastButtonState == HIGH && (currentMillis - lastButtonPress) >= BUTTON_DEBOUNCE_TIME) 
      {
      Serial.println("Button pressed!");
      sendSensorData(BUTTON_SENSOR_ID, 1);
      
      if (dfPlayerInitialized) 
      {
        dfPlayer.play(random(BUTTON_TRACKS_MIN, BUTTON_TRACKS_MAX + 1));
      }

      lastButtonPress = currentMillis;
    }

    lastButtonState = buttonState;

    if (pirState == HIGH && lastPIRState == LOW && !pirLocked) 
    {
      Serial.println("Motion detected!");
      sendSensorData(PIR_SENSOR_ID, 1);
      
      if (dfPlayerInitialized) 
      {
        dfPlayer.play(PIR_TRACK);
      }
      lastPIRTime = currentMillis;
      pirLocked = true;
    }

    if (pirLocked && currentMillis - lastPIRTime >= pirInterval) 
    {
      pirLocked = false;
    }

    lastPIRState = pirState;

    static bool wasDark = (lastLDRValue < 500);
    bool isDarkNow = (ldrValue < 500);
    
    if (isDarkNow != wasDark) 
    {
      Serial.print("Light threshold crossed: ");
      Serial.println(ldrValue);
      sendSensorData(LDR_SENSOR_ID, ldrValue);
      
      if (dfPlayerInitialized) 
      {
        dfPlayer.play(isDarkNow ? DARK_TRACK : LIGHT_TRACK);
      }

      wasDark = isDarkNow;
    }

    lastLDRValue = ldrValue;
  }
}

void sendSensorData(int sensor_id, float value) 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi not connected - skipping send");
    return;
  }

  HTTPClient http;
  http.setTimeout(5000);
  
  String postData = "device_id=" + String(DEVICE_ID);
  postData += "&time_stamp=" + getCurrentTime();
  
  for (int i = 1; i <= 3; i++) 
  {
    postData += "&sensor_id_" + String(i) + "=" + String(i);
    postData += "&value_" + String(i) + "=";
    postData += (i == sensor_id) ? String(value) : "0.0";
  }

  Serial.println("Sending: " + postData);
  
  if (http.begin(SENSOR_API_URL)) 
  {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) 
    {
      Serial.printf("HTTP code: %d\n", httpResponseCode);
      if (httpResponseCode == HTTP_CODE_OK) 
      {
        String response = http.getString();
        Serial.println("Response: " + response);
      }
    } 
    
    else 
    {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } 
  
  else 
  {
    Serial.println("Unable to begin HTTP connection");
  }
}

String getCurrentTime() 
{
  if (!timeInitialized) 
  {
    if (!timeClient.forceUpdate()) 
    {
      return "1970-01-01 00:00:00";
    }

    timeInitialized = true;
  }
  
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime(&epochTime);
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
  return String(timeString);
}

void playRandomSound(const int tracks[], int count) 
{
  if (!dfPlayerInitialized || count <= 0) return;
  
  static unsigned long lastPlayTime = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastPlayTime > 100) 
  {
    int randomIndex = random(0, count);
    dfPlayer.play(tracks[randomIndex]);
    lastPlayTime = currentMillis;
  }
}