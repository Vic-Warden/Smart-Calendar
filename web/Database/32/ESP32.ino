// Includes
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>
#include <vector>

// Objects 
LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;
WiFiClient wifiClient;
DynamicJsonDocument doc(2048);

// Variables 
std::vector<String> knownAppointments;
JsonArray appointments;

const char* ssid = "DeKey-Fraijlemaborg";
const char* password = "i3xHp*ag";
const char* apiUrl = "http://100.74.252.69/Database/Appointment/recover_appointment.php";

int currentAppointment = 0;
unsigned long lastCheckTime = 0;
unsigned long lastScrollTime = 0;
unsigned long lastDisplayTime = 0;
int scrollIndex = 0;
bool dfPlayerInitialized = false;
bool isScrolling = false;
bool isShortDisplayed = false;

// Function
void setup();
void loop();
void checkWifiConnection();
void fetchAppointments();
void checkAppointmentsForActions(JsonArray newAppointments);
void displayCurrentAppointment();
void scrollTaskOnLCD(String task, String dateLine);
void playRandomTrackActive();
void playRandomTrackUpdated();
void playRandomTrackDeleted();
void advanceToNextAppointment();

// Setup 
void setup() 
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

  if (dfPlayer.begin(mySoftwareSerial)) 
  {
    dfPlayer.volume(10);
    dfPlayerInitialized = true;
  }

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) 
  {
    delay(200);
    attempts++;
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.status() == WL_CONNECTED ? "Wi-Fi Connecte!" : "Wi-Fi ECHEC");
  delay(2000);
  lastCheckTime = millis();
  lastDisplayTime = millis();
}

// Loop
void loop() 
{
  checkWifiConnection();

  if (millis() - lastCheckTime >= 5000) 
  {
    fetchAppointments();
    lastCheckTime = millis();
  }

  displayCurrentAppointment();
}

// Check Wifi
void checkWifiConnection() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    WiFi.begin(ssid, password);
    delay(5000);
  }
}

// Fetch Appointment
void fetchAppointments() 
{
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(apiUrl);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) 
  {
    String response = http.getString();
    DynamicJsonDocument newDoc(2048);
    DeserializationError error = deserializeJson(newDoc, response);
    if (!error) 
    {
      JsonArray newAppointments = newDoc.as<JsonArray>();
      checkAppointmentsForActions(newAppointments);
    }
  }
  http.end();
}

// Check appointments
void checkAppointmentsForActions(JsonArray newAppointments) 
{
  std::vector<String> newKnownAppointments;

  for (JsonObject newApp : newAppointments) 
  {
    String newId = newApp["appointment_id"].as<String>();
    String newTask = newApp["task"].as<String>();
    String newNote = newApp["note"].as<String>();
    bool found = false;

    for (JsonObject oldApp : doc.as<JsonArray>()) 
    {
      if (oldApp["appointment_id"].as<String>() == newId) 
      {
        found = true;
        if (oldApp["task"].as<String>() != newTask || oldApp["note"].as<String>() != newNote) 
        {
          playRandomTrackUpdated();
        }

        break;
      }
    }

    if (!found) 
    {
      playRandomTrackActive();
    }

    newKnownAppointments.push_back(newId);
  }

  for (String oldId : knownAppointments) 
  {
    bool stillExists = false;
    for (JsonObject newApp : newAppointments) 
    {
      if (newApp["appointment_id"].as<String>() == oldId) 
      {
        stillExists = true;
        break;
      }
    }
    if (!stillExists) 
    {
      playRandomTrackDeleted();
    }
  }

  knownAppointments = newKnownAppointments;
  doc.clear();
  doc.set(newAppointments);
  appointments = doc.as<JsonArray>();
  currentAppointment = 0;
  scrollIndex = 0;
  isScrolling = false;
  isShortDisplayed = false;
}

// Display appointment
void displayCurrentAppointment() 
{
  if (appointments.size() == 0) return;

  JsonObject app = appointments[currentAppointment];
  String task = app["task"].as<String>();
  String dateHour = app["date_hour"].as<String>();
  String moisJour = dateHour.substring(5, 10);
  String heureMinute = dateHour.substring(11, 16);
  String dateLine = moisJour + " a " + heureMinute;

  if (task.length() <= 16) 
  {
    if (!isShortDisplayed) 
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tache: " + task);
      lcd.setCursor(0, 1);
      lcd.print(dateLine);
      lastDisplayTime = millis();
      isShortDisplayed = true;
    }

    if (millis() - lastDisplayTime >= 5000) 
    {
      advanceToNextAppointment();
    }
  } 
  
  else 
  {
    isScrolling = true;
    scrollTaskOnLCD(task, dateLine);
  }
}

// Scroll
void scrollTaskOnLCD(String task, String dateLine) 
{
  String scrollText = "Tache: " + task + "   ";
  int maxIndex = scrollText.length() - 16;

  if (millis() - lastScrollTime >= 300) 
  {
    lcd.setCursor(0, 0);
    lcd.print(scrollText.substring(scrollIndex, scrollIndex + 16));
    lcd.setCursor(0, 1);
    lcd.print(dateLine);

    scrollIndex++;
    lastScrollTime = millis();

    if (scrollIndex > maxIndex) 
    {
      scrollIndex = 0;
      isScrolling = false;
      lastDisplayTime = millis();
      advanceToNextAppointment();
    }
  }
}

// Next appointment
void advanceToNextAppointment() 
{
  currentAppointment = (currentAppointment + 1) % appointments.size();
  scrollIndex = 0;
  isScrolling = false;
  isShortDisplayed = false;
}

// Audio
void playRandomTrackActive() 
{
  if (!dfPlayerInitialized) return;
  int tracks[] = {1, 2, 3, 6, 7, 8, 9, 10, 31, 33, 34, 35};
  int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
  dfPlayer.play(trackNumber);
}

void playRandomTrackUpdated() 
{
  if (!dfPlayerInitialized) return;
  int tracks[] = {4, 5, 12, 17, 20, 28, 29, 30};
  int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
  dfPlayer.play(trackNumber);
}

void playRandomTrackDeleted() 
{
  if (!dfPlayerInitialized) return;
  int tracks[] = {24, 25, 26, 27, 23, 13, 14, 32};
  int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
  dfPlayer.play(trackNumber);
}
