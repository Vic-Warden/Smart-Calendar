// Includes
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

// Objects 
LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;

const char* SSID = "DeKey-Fraijlemaborg";
const char* PASSWORD = "i3xHp*ag";
const char* API_URL = "http://100.74.252.69/Database/Appointment/recover_appointment.php";

const unsigned long REFRESH_INTERVAL = 5000;
const unsigned long DISPLAY_INTERVAL = 3000;

const int ACTIVE_TRACKS[] = {1, 2, 3, 6, 7, 8, 9, 10, 31, 33, 34, 35};
const int UPDATED_TRACKS[] = {4, 5, 12, 17, 20, 28, 29, 30};
const int DELETED_TRACKS[] = {24, 25, 26, 27, 23, 13, 14, 32};

DynamicJsonDocument appointments(2048);
std::vector<String> previousAppointmentIds;
unsigned long lastRefreshTime = 0;
unsigned long lastDisplayChangeTime = 0;
int currentAppointmentIndex = 0;
bool dfPlayerInitialized = false;
bool dataRefreshed = false;

void setup() 
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

  if (dfPlayer.begin(mySoftwareSerial)) 
  {
    dfPlayer.volume(10);
    dfPlayerInitialized = true;
    Serial.println("DFPlayer initialized");
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  connectToWiFi();
}

void loop() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    connectToWiFi();
    return;
  }

  if (!dataRefreshed) 
  {
    fetchAppointments();
    dataRefreshed = true;
    lastRefreshTime = millis();
  }

  if (millis() - lastRefreshTime >= REFRESH_INTERVAL) 
  {
    dataRefreshed = false;
  }

  displayCurrentAppointment();
}

void connectToWiFi() 
{
  Serial.print("Connecting to WiFi...");
  WiFi.begin(SSID, PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) 
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println("\nConnected!");
    lcd.clear();
    lcd.print("WiFi Connected");
    delay(2000);
  } 
  
  else 
  {
    Serial.println("\nFailed to connect");
    lcd.clear();
    lcd.print("WiFi Failed");
  }
}

void fetchAppointments() 
{
  HTTPClient http;
  http.begin(API_URL);
  int httpCode = http.GET();

  if (httpCode > 0) 
  {
    String payload = http.getString();
    DynamicJsonDocument newAppointments(2048);
    DeserializationError error = deserializeJson(newAppointments, payload);

    if (!error) 
    {
      checkForAppointmentChanges(newAppointments);
    }
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
    lastDisplayChangeTime = millis();
  }
}

void displayCurrentAppointment() 
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

  if (millis() - lastDisplayChangeTime >= DISPLAY_INTERVAL) 
  {
    currentAppointmentIndex = (currentAppointmentIndex + 1) % appointments.size();
    lastDisplayChangeTime = millis();
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
    lcd.print(task.substring(0, 8));

    lcd.setCursor(0, 1);
    lcd.print(dateTime.substring(8, 10));
    lcd.print("/");
    lcd.print(dateTime.substring(5, 7));
    lcd.print(" ");
    lcd.print(dateTime.substring(11, 16));
  }
}

void playRandomSound(const int tracks[], int count) 
{
  if (!dfPlayerInitialized) return;
  int randomIndex = random(0, count);
  dfPlayer.play(tracks[randomIndex]);
}