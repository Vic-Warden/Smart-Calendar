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
WebServer server(80); 
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
bool timeInitialized = false;

// DFPlayer
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;

// Character modes
enum CharacterMode 
{
  COMMISSAR,
  LEMAN_RUSS,
  BANEBLADE
};
CharacterMode currentCharacter = COMMISSAR;

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
const unsigned long CHARACTER_NOTIFICATION_DURATION = 2000;
unsigned long lastHourCheck = 0;
unsigned long characterNotificationStart = 0;
bool showingCharacterNotification = false;
String currentCharacterName = "Commissar";

// Sound 
const int COMMISSAR_TRACK = 31;
const int LEMAN_RUSS_TRACK = 42;
const int BANEBLADE_TRACK = 53;
const int DARK_TRACK = 35;
const int LIGHT_TRACK = 36;
const int PIR_TRACKS[] = {37, 38, 39, 40, 41};

// Appointment tracks
const int COMMISSAR_ACTIVE_TRACKS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 28, 33, 34};
const int COMMISSAR_DELETED_TRACKS[] = {11, 20, 23, 24, 25, 26, 27, 29, 30, 32};
const int LEMAN_RUSS_ACTIVE_TRACKS[] = {43, 44, 45, 46, 47, 48};
const int LEMAN_RUSS_DELETED_TRACKS[] = {49, 50, 51};
const int BANEBLADE_ACTIVE_TRACKS[] = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 67, 68, 69, 70, 71, 72, 73, 74};
const int BANEBLADE_DELETED_TRACKS[] = {64, 65, 66};

// Night Mode 
const int NIGHT_MODE_ENTER_THRESHOLD = 650;  
const int NIGHT_MODE_EXIT_THRESHOLD = 750;   

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
bool nightMode = false;

// Sensor states
int lastButtonState = HIGH;
int lastPIRState = LOW;
int lastLDRValue = -1;
bool pirLocked = false;
unsigned long lastPIRTime = 0;
const long pirInterval = 300000;

// Display states
enum DisplayState {
  SHOWING_CHARACTER_NOTIFICATION,
  SHOWING_APPOINTMENTS,
  SHOWING_NO_APPOINTMENTS,
  SHOWING_INITIALIZATION
};
DisplayState currentDisplayState = SHOWING_INITIALIZATION;

// Function declarations
void updateDisplay(String line1, String line2, DisplayState newState);
void initializeTime();
void connectToWiFi();
void update7SegmentDisplay();
void handleAppointments(unsigned long currentMillis);
void checkHourlyChime(unsigned long currentMillis);
void fetchAppointments();
void checkForAppointmentChanges(DynamicJsonDocument& newAppointments);
void playAppropriateSound(bool isActiveOrUpdated);
void displayCurrentAppointment(unsigned long currentMillis);
void handleSensors(unsigned long currentMillis);
void sendSensorData(int sensor_id, float value);
String getCurrentTime();
void playRandomSound(const int tracks[], int count);
void setupServer();

String reverseString(String str) {
  String reversed = "";
  for (int i = str.length() - 1; i >= 0; i--) {
    reversed += str[i];
  }
  return reversed;
}

void updateDisplay(String line1, String line2, DisplayState newState) {
  static String lastLine1 = "";
  static String lastLine2 = "";
  static DisplayState lastState = SHOWING_INITIALIZATION;

  if (newState != lastState || line1 != lastLine1 || line2 != lastLine2) {
    lcd.clear();

    // Inverser les chaînes pour affichage tête en bas
    String rev1 = reverseString(line1);
    String rev2 = reverseString(line2);

    // Afficher ligne 1 en bas, ligne 2 en haut (car écran retourné)
    for (int i = 0; i < rev1.length() && i < 16; i++) {
      lcd.setCursor(15 - i, 1);  // Ligne du bas
      lcd.print(rev1[i]);
    }

    for (int i = 0; i < rev2.length() && i < 16; i++) {
      lcd.setCursor(15 - i, 0);  // Ligne du haut
      lcd.print(rev2[i]);
    }

    lastLine1 = line1;
    lastLine2 = line2;
    lastState = newState;
    currentDisplayState = newState;
  }
}

void initializeTime() {
  if (timeClient.forceUpdate()) {
    timeInitialized = true;
    Serial.println("Time initialized");
    updateDisplay("Time initialized", "Fetching data...", SHOWING_INITIALIZATION);
  } else {
    Serial.println("Time init failed");
    updateDisplay("Time init failed", "Retrying...", SHOWING_INITIALIZATION);
  }
}

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi...");
  updateDisplay("Connecting WiFi", "", SHOWING_INITIALIZATION);
  
  WiFi.begin(SSID, PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    updateDisplay("WiFi Connected", "Syncing time...", SHOWING_INITIALIZATION);
    initializeTime();
  } else {
    Serial.println("\nWiFi failed");
    updateDisplay("WiFi Failed", "Retrying...", SHOWING_INITIALIZATION);
  }
}

void update7SegmentDisplay() {
  if (!timeInitialized) {
    display.showNumberDec(9999);
    return;
  }
  
  timeClient.update();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  display.showNumberDecEx(hour * 100 + minute, 0b01000000, true);
}

void handleAppointments(unsigned long currentMillis) {
  if (!dataRefreshed) {
    fetchAppointments();
    dataRefreshed = true;
    lastAppointmentRefresh = currentMillis;
  }

  if (currentMillis - lastAppointmentRefresh >= APPOINTMENT_REFRESH_INTERVAL) {
    dataRefreshed = false;
  }

  displayCurrentAppointment(currentMillis);
}

void checkHourlyChime(unsigned long currentMillis) {
  static unsigned long lastCheckTime = 0;
  static int lastChimeHour = -1;
  
  if (currentMillis - lastCheckTime >= 60000) {
    lastCheckTime = currentMillis;
    
    // Silent during night mode
    if (nightMode || !timeInitialized) return;
    
    timeClient.update();
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    
    if (currentMinute == 0 && currentHour != lastChimeHour) {
      if (dfPlayerInitialized) {
        dfPlayer.play(14);
      }
      lastChimeHour = currentHour;
    } else if (currentMinute != 0) {
      lastChimeHour = -1;
    }
  }
}

void fetchAppointments() {
  HTTPClient http;
  http.begin(APPOINTMENT_API_URL);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument newAppointments(2048);
    deserializeJson(newAppointments, payload);
    checkForAppointmentChanges(newAppointments);
  } else {
    updateDisplay("Fetch failed", "Retrying...", SHOWING_INITIALIZATION);
  }
  http.end();
}

void checkForAppointmentChanges(DynamicJsonDocument& newAppointments) {
  std::vector<String> currentIds;
  bool changesDetected = false;

  for (JsonObject newApp : newAppointments.as<JsonArray>()) {
    String newAppId = newApp["appointment_id"].as<String>();
    currentIds.push_back(newAppId);

    bool isNew = true;
    for (String prevId : previousAppointmentIds) {
      if (prevId == newAppId) {
        isNew = false;
        break;
      }
    }

    if (isNew && !nightMode) {
      playAppropriateSound(true);
      changesDetected = true;
    }
  }

  for (String prevId : previousAppointmentIds) {
    bool stillExists = false;
    for (JsonObject newApp : newAppointments.as<JsonArray>()) {
      if (newApp["appointment_id"].as<String>() == prevId) {
        stillExists = true;
        break;
      }
    }

    if (!stillExists && !nightMode) {
      playAppropriateSound(false);
      changesDetected = true;
    }
  }

  if (changesDetected || previousAppointmentIds.empty()) {
    appointments = newAppointments;
    previousAppointmentIds = currentIds;
    currentAppointmentIndex = 0;
    lastDisplayChange = millis();
  }
}

void playAppropriateSound(bool isActiveOrUpdated) {
  if (!dfPlayerInitialized || nightMode) return;
  
  switch(currentCharacter) {
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

void displayCurrentAppointment(unsigned long currentMillis) {
  if (showingCharacterNotification) {
    return; // Don't update appointments while showing character notification
  }
  
  if (appointments.size() == 0) {
    if (currentDisplayState != SHOWING_NO_APPOINTMENTS) {
      updateDisplay("No appointments", "", SHOWING_NO_APPOINTMENTS);
    }
    return;
  }

  if (currentMillis - lastDisplayChange >= DISPLAY_INTERVAL) {
    currentAppointmentIndex = (currentAppointmentIndex + 1) % appointments.size();
    lastDisplayChange = currentMillis;
  }

  JsonObject appointment = appointments[currentAppointmentIndex];
  String task = appointment["task"].as<String>();
  String dateTime = appointment["date_hour"].as<String>();

  String line2 = "";
  
  if (dateTime.length() >= 16) {
    line2 = dateTime.substring(8, 10) + "/" + 
            dateTime.substring(5, 7) + " " + 
            dateTime.substring(11, 16);
  } else {
    line2 = "Invalid date";
  }

  // Truncate task if too long for display
  if (task.length() > 16) {
    task = task.substring(0, 16);
  }

  updateDisplay(task, line2, SHOWING_APPOINTMENTS);
}

void handleSensors(unsigned long currentMillis) {
  if (currentMillis - lastSensorCheck >= SENSOR_CHECK_INTERVAL) {
    lastSensorCheck = currentMillis;
    
    int buttonState = digitalRead(BUTTON_PIN);
    int pirState = digitalRead(PIR_PIN);
    int ldrValue = analogRead(LDR_PIN);
    static bool wasInNightMode = nightMode;
    
    // Handle night mode transition
    bool newNightMode = (ldrValue < NIGHT_MODE_ENTER_THRESHOLD);
    if (newNightMode != nightMode) {
      nightMode = newNightMode;
      if (dfPlayerInitialized) {
        dfPlayer.play(nightMode ? DARK_TRACK : LIGHT_TRACK);
      }
      wasInNightMode = nightMode;
      sendSensorData(LDR_SENSOR_ID, ldrValue);
    }

    // Handle button press
    if (buttonState == LOW && lastButtonState == HIGH && 
        (currentMillis - lastButtonPress) >= BUTTON_DEBOUNCE_TIME) {
      sendSensorData(BUTTON_SENSOR_ID, 1);
      
      switch(currentCharacter) {
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
      
      if (dfPlayerInitialized && !nightMode) {
        switch(currentCharacter) {
          case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
          case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
          case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
        }
      }
      
      showingCharacterNotification = true;
      characterNotificationStart = currentMillis;
      lastButtonPress = currentMillis;
    }

    // Handle PIR sensor
    if (pirState == HIGH && lastPIRState == LOW && !pirLocked) {
      sendSensorData(PIR_SENSOR_ID, 1);
      
      if (dfPlayerInitialized && !nightMode) {
        playRandomSound(PIR_TRACKS, sizeof(PIR_TRACKS)/sizeof(int));
      }

      lastPIRTime = currentMillis;
      pirLocked = true;
    }

    if (pirLocked && currentMillis - lastPIRTime >= pirInterval) {
      pirLocked = false;
    }

    lastButtonState = buttonState;
    lastPIRState = pirState;
    lastLDRValue = ldrValue;
  }
}

void sendSensorData(int sensor_id, float value) {
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

String getCurrentTime() {
  if (!timeInitialized && !timeClient.forceUpdate()) {
    return "1970-01-01 00:00:00";
  }
  
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime(&epochTime);
  char timeString[25];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
  return String(timeString);
}

void playRandomSound(const int tracks[], int count) {
  if (!dfPlayerInitialized || count <= 0 || nightMode) return;
  
  static unsigned long lastPlayTime = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastPlayTime > 100) {
    int randomIndex = random(0, count);
    dfPlayer.play(tracks[randomIndex]);
    lastPlayTime = currentMillis;
  }
}

void setupServer() {
  // Test de base
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Smart Calendar is online!");
  });

  // Changement de personnage via navigateur
  server.on("/switch_character", HTTP_GET, []() {
    switch (currentCharacter) {
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
    if (dfPlayerInitialized && !nightMode) {
      switch (currentCharacter) {
        case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
        case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
        case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
      }
    }

    showingCharacterNotification = true;
    characterNotificationStart = millis();

    server.send(200, "text/plain", "Character changed to " + currentCharacterName);
  });

  // Fait parler le personnage courant (random)
  server.on("/play_random", HTTP_GET, []() {
    if (dfPlayerInitialized && !nightMode) {
      switch (currentCharacter) {
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
    } else {
      server.send(503, "text/plain", "Night mode active or DFPlayer unavailable.");
    }
  });

  server.begin();
  Serial.println("HTTP server started!");
}

void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

  Wire.begin(LCD_SDA, LCD_SCL);
  
  if (dfPlayer.begin(mySoftwareSerial)) {
    dfPlayer.volume(10);
    dfPlayerInitialized = true;
    Serial.println("DFPlayer initialized");
  } else {
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

void loop() {
  unsigned long currentMillis = millis();

  // Always handle HTTP requests
  server.handleClient();

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    if (currentMillis - lastReconnectAttempt >= WIFI_RECONNECT_DELAY) {
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
  checkHourlyChime(currentMillis);

  if (showingCharacterNotification && 
      currentMillis - characterNotificationStart >= CHARACTER_NOTIFICATION_DURATION) {
    showingCharacterNotification = false;
    currentDisplayState = SHOWING_APPOINTMENTS;
    lastDisplayChange = 0;
  }
}