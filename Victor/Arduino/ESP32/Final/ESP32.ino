/*
 Smart Calendar System for ESP32

  Author: Victor HOEGY
  Date: 2025-04
  MIT License
  copyright: (c) 2025, Victor HOEGY
  
 This program controls a smart calendar device with LCD display, 7-segment clock,
 DFPlayer for audio, servo motor, and various sensors. It connects to WiFi,
 fetches appointments from a server, and provides interactive features through
 physical buttons and web interface.

 */

 #include <Wire.h>
 #include <LiquidCrystal_I2C.h>
 #include <WiFiManager.h>
 #include <HTTPClient.h>
 #include <ArduinoJson.h>
 #include <DFRobotDFPlayerMini.h>
 #include <HardwareSerial.h>
 #include <NTPClient.h>
 #include <WiFiUdp.h>
 #include <TM1637Display.h>
 #include <WebServer.h>
 #include <ESP32Servo.h>

 // Hardware
 
// LCD
 const int LCD_I2C_ADDRESS = 0x27;     
 const int LCD_COLUMNS = 16;            
 const int LCD_ROWS = 2;               
 const int LCD_SDA_PIN = 21;           
 const int LCD_SCL_PIN = 22; 
 const int LCD_START_COL = 0;
 const int LCD_FIRST_LINE = 0;
 const int LCD_SECOND_LINE = 1;
           
 
// 7 segment 
 const int DISPLAY_CLK_PIN = 18;        
 const int DISPLAY_DIO_PIN = 19;         
 const uint8_t DISPLAY_BRIGHTNESS = 0x0a;
 const uint8_t DISPLAY_COLON_MASK = 0b01000000;
 const int DISPLAY_ERROR_CODE = 9999;   
 const int DISPLAY_DEFAULT_CODE = 8888; 
 const int DISPLAY_TIME_SCALING = 100; 
 
 // DFPlayer Mini 
 const int DFPLAYER_RX_PIN = 16;         
 const int DFPLAYER_TX_PIN = 17;         
 const int DFPLAYER_VOLUME = 15;          
 const int DFPLAYER_MIN_PLAY_INTERVAL_MS = 100;
 const int DFPLAYER_BAUD = 9600;        
 
 // Servo Motor 
 const int SERVO_PIN = 27;               
 const int SERVO_MIN_POSITION = 90;     
 const int SERVO_MAX_POSITION = 180;     
 const int SERVO_SPEED = 1;             
 const unsigned long SERVO_UPDATE_INTERVAL_MS = 20;
 const unsigned long SERVO_TEST_PAUSE_MS = 1000;    
 const unsigned long HOURLY_SERVO_DURATION_MS = 60000; 
 const int SERVO_PULSE_MIN = 500;      
 const int SERVO_PULSE_MAX = 2500;      
 const int SERVO_DIRECTION_FORWARD = 1;
 const int SERVO_DIRECTION_BACKWARD = -1;
 
 // Input 
 const int BUTTON_PIN = 14;             
 const int PIR_PIN = 35;                
 const int LDR_PIN = 34;          

 // Network    
 
 // NTP 
 const char* NTP_SERVER = "pool.ntp.org";
 const long NTP_UTC_OFFSET_SECONDS = 3600; 
 const char* DEFAULT_TIME_STRING = "1970-01-01 00:00:00";
 const size_t TIME_STRING_BUFFER_SIZE = 25;
 const char* TIME_STRING_FORMAT = "%Y-%m-%d %H:%M:%S"; 
 const int HOURS_PER_DAY = 24; 
 const int INVALID_HOUR = -1;

 
 // API 
 const char* APPOINTMENT_API_URL = "http://100.74.252.69/Database/Appointment/recover_appointment.php";
 const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";
 
 // Server 
 const char* WIFI_AP_NAME = "SmartCalendarAP"; 
 const char* SERVER_ROOT_RESPONSE = "Smart Calendar is online!"; 
 const char* CONTENT_TYPE_TEXT = "text/plain"; 
 const char* CONTENT_TYPE_JSON = "application/json"; 
 const int HTTP_OK_CODE = 200;           
 const int HTTP_SERVICE_UNAVAILABLE_CODE = 503; 

 const size_t APPOINTMENT_JSON_DOC_SIZE = 2048;
 
 // Serial monitor
 const int SERIAL_BAUD = 115200;        

 // ID
 
 const int DEVICE_ID = 1; // ESP32's ID
 
 enum SensorIds 
 {
   BUTTON_SENSOR_ID = 1, 
   PIR_SENSOR_ID = 2,        
   LDR_SENSOR_ID = 3           
 };
 
 // Timing
 const unsigned long APPOINTMENT_REFRESH_INTERVAL_MS = 5000; 
 const unsigned long DISPLAY_INTERVAL_MS = 3000;    
 const unsigned long SENSOR_CHECK_INTERVAL_MS = 50; 
 const unsigned long TIME_UPDATE_INTERVAL_MS = 60000; 
 const unsigned long NTP_RETRY_INTERVAL_MS = 30000; 
 const unsigned long BUTTON_DEBOUNCE_TIME_MS = 50;  
 const unsigned long WIFI_RECONNECT_DELAY_MS = 1000; 
 const unsigned long DELAY_WIFI_FAIL_MS = 3000;    
 const unsigned long DELAY_AFTER_RESTART_MS = 5000; 
 const unsigned long CHARACTER_NOTIFICATION_DURATION_MS = 2000; 
 const unsigned long PIR_INTERVAL_MS = 300000;     
 const unsigned long WIFI_TIMEOUT_MS = 30000;     
 const unsigned long WATCHDOG_INTERVAL_MS = 60000; 
 const unsigned long MAX_UPTIME_MS = 86400000;  
 const int TIMEZONE_OFFSET_HOURS = 1;
 
 // Night Mode
 const int NIGHT_MODE_ENTER_THRESHOLD = 650; 
 const int NIGHT_MODE_EXIT_THRESHOLD = 750;  
 
 // Character Modes 
 enum CharacterMode 
 { 
   COMMISSAR,                       
   LEMAN_RUSS,                      
   BANEBLADE,                      
   CHARACTER_MODE_COUNT             
 };

 const int CHARACTER_CYCLE_STEP = 1;

 const int APPOINTMENT_CYCLE_STEP = 1;

 
 // Servo Test 
 const int INITIAL_PHASE = 0;        
 const int PAUSE_PHASE = 1;        
 const int RETURN_PHASE = 2;         

 // TRUE FALSE
 const int VALUE_TRUE = 1;           
 const int VALUE_FALSE = 0;         
 
 // Date 
 const int DATE_STRING_LENGTH_THRESHOLD = 16; 
 const int INDEX_DAY_START = 8;      
 const int INDEX_DAY_END = 10;      
 const int INDEX_MONTH_START = 5;  
 const int INDEX_MONTH_END = 7;      
 const int INDEX_TIME_START = 11;   
 const int INDEX_TIME_END = 16;      
 
 // Message
 
 // LCD Message 
 const char* LCD_MSG_INIT = "Initializing...";
 const char* LCD_MSG_TEST_SERVO = "Testing servo...";
 const char* LCD_MSG_NO_APPOINTMENTS = "No appointments";
 const char* LCD_MSG_INVALID_DATE = "Invalid date";
 const char* LCD_MSG_TIME_INIT = "Time initialized";
 const char* LCD_MSG_TIME_FAIL = "Time init failed";
 const char* LCD_MSG_FETCHING = "Fetching data...";
 const char* LCD_MSG_RETRYING = "Retrying...";
 const char* LCD_MSG_WIFI_START = "Starting WiFi";
 const char* LCD_MSG_WIFI_FAIL = "WiFi Failed";
 const char* LCD_MSG_WIFI_CONNECTED = "WiFi Connected";
 const char* LCD_MSG_SYNCING = "Syncing time...";
 const char* LCD_MSG_RESTARTING = "Restarting...";
 const char* LCD_MSG_FETCH_FAIL = "Fetch failed";
 const char* LCD_MSG_SWITCHED = "Switched to";
 
 // Serial Monitor Message
 const char* SERIAL_MSG_TIME_INIT = "Time initialized";
 const char* SERIAL_MSG_TIME_FAIL = "Time init failed";
 const char* SERIAL_MSG_TEST_SERVO = "Testing servo: max -> min -> pause -> max";
 const char* SERIAL_MSG_SERVO_FAIL = "Failed to attach servo!";
 const char* SERIAL_MSG_SERVO_ATTACHED = "Servo attached";
 const char* SERIAL_MSG_SERVO_TEST_COMPLETE = "Servo test complete";
 const char* SERIAL_MSG_DFPLAYER_INIT = "DFPlayer initialized";
 const char* SERIAL_MSG_DFPLAYER_FAIL = "DFPlayer initialization failed!";
 const char* SERIAL_MSG_SERVO_MOVING_MIN = "Servo moving slowly to minimum position";
 const char* SERIAL_MSG_SERVO_MOVING_MAX = "Servo returning slowly to maximum position";
 const char* SERIAL_MSG_SERVO_REACHED = "Servo reached target position";
 const char* SERIAL_MSG_WIFI_TIMEOUT = "WiFi connection timeout, continuing offline";
 const char* SERIAL_MSG_HTTP_STARTED = "HTTP server started";
 const char* SERIAL_MSG_IP_ADDRESS = "IP address: ";
 
 // Web Server Responses 
 const char* SERVER_SWITCH_RESPONSE = "Character changed to ";
 
// Variables
 LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
 TM1637Display display(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);
 WiFiUDP ntpUDP;
 WebServer server(80);
 NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_UTC_OFFSET_SECONDS);
 HardwareSerial mySoftwareSerial(2);
 DFRobotDFPlayerMini dfPlayer;
 Servo servo1;
 
 CharacterMode currentCharacter = COMMISSAR; 
 String currentCharacterNames[CHARACTER_MODE_COUNT] = 
 {
   "Commissar",                 
   "Leman Russ",                
   "Baneblade"                  
 };
 
 DynamicJsonDocument appointments(APPOINTMENT_JSON_DOC_SIZE);    
 std::vector<String> previousAppointmentIds; 
 
 // Timing Trackers 
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
 
 // State Trackers 
 int currentAppointmentIndex = 0;     
 int lastButtonState = HIGH;               
 int lastPIRState = LOW;                    
 int lastLDRValue = -1;                   
 
 bool timeInitialized = false;              
 bool dfPlayerInitialized = false;         
 bool dataRefreshed = false;                
 bool nightMode = false;                   
 bool showingCharacterNotification = false; 
 bool pirLocked = false;                   
 
 // Servo Control 
 int servoCurrentPosition = SERVO_MAX_POSITION; 
 int servoTargetPosition = SERVO_MAX_POSITION; 
 unsigned long lastServoUpdate = 0;         
 unsigned long servoReturnTime = 0;        
 bool servoMoving = false;                 
 
// Sound
 const int COMMISSAR_TRACK = 31;            
 const int LEMAN_RUSS_TRACK = 42;          
 const int BANEBLADE_TRACK = 53;          
 const int DARK_TRACK = 35;                
 const int LIGHT_TRACK = 36;              
 const int COMMISSAR_ACTIVE_TRACKS[] = {1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,21,22,28,33,34};
 const int COMMISSAR_DELETED_TRACKS[] = {11,20,23,24,25,26,27,29,30,32};
 const int LEMAN_RUSS_ACTIVE_TRACKS[] = {43,44,45,46,47,48};
 const int LEMAN_RUSS_DELETED_TRACKS[] = {49,50,51};
 const int BANEBLADE_ACTIVE_TRACKS[] = {52,53,54,55,56,57,58,59,60,61,62,63,67,68,69,70,71,72,73,74};
 const int BANEBLADE_DELETED_TRACKS[] = {64,65,66};
 const int PIR_TRACKS[] = {37,38,39,40,41}; 
 
 // Display States 
 enum DisplayState 
 {
   SHOWING_CHARACTER_NOTIFICATION,   
   SHOWING_APPOINTMENTS,             
   SHOWING_NO_APPOINTMENTS,         
   SHOWING_INITIALIZATION            
 };
 DisplayState currentDisplayState = SHOWING_INITIALIZATION; 
 
// Updates the LCD display with new content if it has changed
 void updateDisplay(String line1, String line2, DisplayState newState) 
 {
   static String lastLine1 = "";      // Previously displayed first line
   static String lastLine2 = "";      // Previously displayed second line
   static DisplayState lastState = SHOWING_INITIALIZATION; // Previous state
 
   // Only update display if content or state has changed
   if (newState != lastState || line1 != lastLine1 || line2 != lastLine2) 
   {
     lcd.clear();  // Clear the display before showing new content
     
     // Display first line
     lcd.setCursor(LCD_START_COL, LCD_FIRST_LINE);
     lcd.print(line1.substring(0, LCD_COLUMNS));
 
     // Display second line
     lcd.setCursor(LCD_START_COL, LCD_SECOND_LINE);
     lcd.print(line2.substring(0, LCD_COLUMNS));
 
     // Update tracking variables
     lastLine1 = line1;
     lastLine2 = line2;
     lastState = newState;
     currentDisplayState = newState;
   }
 }
 
// Updates the 7-segment display with current time
 void update7SegmentDisplay() 
 {
   // Show error code if time hasn't been initialized
   if (!timeInitialized) 
   {
     display.showNumberDec(DISPLAY_ERROR_CODE);
     return;
   }
   
   // Update time from NTP server and display it
   timeClient.update();
   int hour = timeClient.getHours() + TIMEZONE_OFFSET_HOURS; // Adjust for timezone
   // Handle 24-hour rollover
   if (hour >= HOURS_PER_DAY) hour -= HOURS_PER_DAY;
   int minute = timeClient.getMinutes();
   // Display time with colon (e.g., 12:34)
   int timeDisplay = hour * DISPLAY_TIME_SCALING + minute;
   display.showNumberDecEx(timeDisplay, DISPLAY_COLON_MASK, true);
 }
 
// Initializes time synchronization with NTP server
 void initializeTime() 
 {
   // Attempt to force an NTP time update
   if (timeClient.forceUpdate()) 
   {
     timeInitialized = true;
     Serial.println(SERIAL_MSG_TIME_INIT);
     updateDisplay(LCD_MSG_TIME_INIT, LCD_MSG_FETCHING, SHOWING_INITIALIZATION);
   } 
   
   else 
   {
     Serial.println(SERIAL_MSG_TIME_FAIL);
     updateDisplay(LCD_MSG_TIME_FAIL, LCD_MSG_RETRYING, SHOWING_INITIALIZATION);
   }
 }
 
// Connects to WiFi using WiFiManager
 void connectToWiFi() 
 {
   WiFiManager wifiManager;
   updateDisplay(LCD_MSG_WIFI_START, "", SHOWING_INITIALIZATION);
   
   // Attempt to connect to WiFi or start configuration AP
   if (!wifiManager.autoConnect(WIFI_AP_NAME)) 
   {
     // Connection failed show message and restart
     updateDisplay(LCD_MSG_WIFI_FAIL, LCD_MSG_RESTARTING, SHOWING_INITIALIZATION);
     delay(DELAY_WIFI_FAIL_MS);
     ESP.restart();
     delay(DELAY_AFTER_RESTART_MS);
   }
   
   // Connection successful proceed to time sync
   updateDisplay(LCD_MSG_WIFI_CONNECTED, LCD_MSG_SYNCING, SHOWING_INITIALIZATION);
   initializeTime();
 }
 
// Fetches appointments from the API server
 void fetchAppointments() 
 {
   HTTPClient http;
   http.begin(APPOINTMENT_API_URL);
 
   // Make GET request to appointment API
   int httpCode = http.GET();
   
   // Process response if successful
   if (httpCode == HTTP_CODE_OK) 
   {
     String payload = http.getString();
     DynamicJsonDocument newAppointments(APPOINTMENT_JSON_DOC_SIZE);
     deserializeJson(newAppointments, payload);
     checkForAppointmentChanges(newAppointments);
   } 
   
   else 
   {
     // Show error if fetch failed
     updateDisplay(LCD_MSG_FETCH_FAIL, LCD_MSG_RETRYING, SHOWING_INITIALIZATION);
   }
 
   http.end(); // Free resources
 }
 
// Compares new appointments with previous ones and triggers appropriate actions
 void checkForAppointmentChanges(DynamicJsonDocument& newAppointments) 
 {
   std::vector<String> currentIds;    // Track IDs of current appointments
   bool changesDetected = false;      // Flag for any detected changes
 
   // Check for new or modified appointments
   for (JsonObject newApp : newAppointments.as<JsonArray>()) 
   {
     String newAppId = newApp["appointment_id"].as<String>();
     currentIds.push_back(newAppId);
 
     bool isNew = true;
     bool isModified = false;
     
     // Compare with previous appointments
     for (String prevId : previousAppointmentIds) 
     {
       if (prevId == newAppId) 
       {
         isNew = false;
 
         // Check if existing appointment has changed
         for (JsonObject oldApp : appointments.as<JsonArray>()) 
         {
           if (oldApp["appointment_id"].as<String>() == newAppId) 
           {
             if (oldApp["task"] != newApp["task"] || oldApp["date_hour"] != newApp["date_hour"]) {
               isModified = true;
             }

             break;
           }
         }

         break;
       }
     }
 
     // Play sound for new or modified appointments (unless in night mode)
     if ((isNew || isModified) && !nightMode) 
     {
        playAppropriateSound(VALUE_TRUE);
        changesDetected = true;
     }
   }
 
   // Check for deleted appointments
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
 
     // Play sound for deleted appointments (unless in night mode)
     if (!stillExists && !nightMode) 
     {
       playAppropriateSound(VALUE_FALSE);
       changesDetected = true;
     }
   }
 
   // Update stored appointments if changes detected or first load
   if (changesDetected || previousAppointmentIds.empty()) 
   {
     appointments = newAppointments;
     previousAppointmentIds = currentIds;
     currentAppointmentIndex = 0;
     lastDisplayChange = millis();
   }
 }
 
// Plays an appropriate sound based on current character mode and event type
 void playAppropriateSound(bool isActiveOrUpdated) 
 {
   // Don't play sounds if DFPlayer isn't ready or in night mode
   if (!dfPlayerInitialized || nightMode) return;
   
   // Select sound set based on current character mode
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
 
// Displays the current appointment on the LCD
 void displayCurrentAppointment(unsigned long currentMillis) 
 {
   // Don't update display if showing character notification
   if (showingCharacterNotification) return;
   
   // Show "no appointments" message if none are available
   if (appointments.size() == 0) 
   {
     if (currentDisplayState != SHOWING_NO_APPOINTMENTS) 
     {
       updateDisplay(LCD_MSG_NO_APPOINTMENTS, "", SHOWING_NO_APPOINTMENTS);
     }

     return;
   }
 
   // Rotate to next appointment at defined interval
   if (currentMillis - lastDisplayChange >= DISPLAY_INTERVAL_MS) 
   {
     currentAppointmentIndex = (currentAppointmentIndex + APPOINTMENT_CYCLE_STEP) % appointments.size();
     lastDisplayChange = currentMillis;
   }
 
   // Get current appointment data
   JsonObject appointment = appointments[currentAppointmentIndex];
   String task = appointment["task"].as<String>();
   String dateTime = appointment["date_hour"].as<String>();
 
   // Format date string (DD/MM HH:MM)
   String line2 = "";
   if (dateTime.length() >= DATE_STRING_LENGTH_THRESHOLD) 
   {
     line2 = dateTime.substring(INDEX_DAY_START, INDEX_DAY_END) + "/" + 
             dateTime.substring(INDEX_MONTH_START, INDEX_MONTH_END) + " " + 
             dateTime.substring(INDEX_TIME_START, INDEX_TIME_END);
   } 
   
   else 
   {
     line2 = LCD_MSG_INVALID_DATE;
   }
 
   // Truncate task if too long for display
   if (task.length() > LCD_COLUMNS) 
   {
     task = task.substring(0, LCD_COLUMNS);
   }
 
   // Update display with appointment info
   updateDisplay(task, line2, SHOWING_APPOINTMENTS);
 }
 
// Checks and handles sensor inputs
 void handleSensors(unsigned long currentMillis) 
 {
   // Only check sensors at defined interval
   if (currentMillis - lastSensorCheck >= SENSOR_CHECK_INTERVAL_MS) 
   {
     lastSensorCheck = currentMillis;
     
     // Read all sensor values
     int buttonState = digitalRead(BUTTON_PIN);
     int pirState = digitalRead(PIR_PIN);
     int ldrValue = analogRead(LDR_PIN);
     static bool wasInNightMode = nightMode;
     
     // Check for night mode transition
     bool newNightMode = (ldrValue < NIGHT_MODE_ENTER_THRESHOLD);
     if (newNightMode != nightMode) 
     {
       nightMode = newNightMode;
 
       // Play appropriate day/night sound if DFPlayer is ready
       if (dfPlayerInitialized) 
       {
         dfPlayer.play(nightMode ? DARK_TRACK : LIGHT_TRACK);
       }
 
       wasInNightMode = nightMode;
       sendSensorData(LDR_SENSOR_ID, ldrValue);
     }
 
     // Check for button press (with debouncing)
     if (buttonState == LOW && lastButtonState == HIGH && (currentMillis - lastButtonPress) >= BUTTON_DEBOUNCE_TIME_MS) 
     {
       sendSensorData(BUTTON_SENSOR_ID, VALUE_TRUE);
       
       // Cycle to next character mode
       currentCharacter = static_cast<CharacterMode>((currentCharacter + CHARACTER_CYCLE_STEP) % CHARACTER_MODE_COUNT);
       String currentCharacterName = currentCharacterNames[currentCharacter];
       
       // Show character switch notification
       updateDisplay(LCD_MSG_SWITCHED, currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);
       
       // Play character-specific sound if not in night mode
       if (dfPlayerInitialized && !nightMode) 
       {
         switch(currentCharacter) 
         {
            case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
            case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
            case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
         }
       }
       
       // Set notification display timer
       showingCharacterNotification = true;
       characterNotificationStart = currentMillis;
       lastButtonPress = currentMillis;
     }
 
     // Check for motion detection (with cooldown period )
     if (pirState == HIGH && lastPIRState == LOW && !pirLocked) 
     {
       sendSensorData(PIR_SENSOR_ID, VALUE_TRUE);
       
       // Play random motion sound if not in night mode
       if (dfPlayerInitialized && !nightMode) 
       {
         playRandomSound(PIR_TRACKS, sizeof(PIR_TRACKS)/sizeof(int));
       }
 
       // Start cooldown period
       lastPIRTime = currentMillis;
       pirLocked = true;
     }
 
     // Reset PIR lock after cooldown period
     if (pirLocked && currentMillis - lastPIRTime >= PIR_INTERVAL_MS) 
     {
       pirLocked = false;
     }
 
     // Update previous sensor states
     lastButtonState = buttonState;
     lastPIRState = pirState;
     lastLDRValue = ldrValue;
   }
 }
 
// Sends sensor data to the API server
 void sendSensorData(int sensor_id, float value) 
 {
   // Only send if WiFi is connected
   if (WiFi.status() != WL_CONNECTED) return;
 
   HTTPClient http;
   http.begin(SENSOR_API_URL);
   
   // Format POST data with all sensor values
   String postData = "device_id=" + String(DEVICE_ID) +
                    "&time_stamp=" + getCurrentTime() +
                    "&sensor_id_1=1&value_1=" + (sensor_id == BUTTON_SENSOR_ID ? String(value) : "0.0") +
                    "&sensor_id_2=2&value_2=" + (sensor_id == PIR_SENSOR_ID ? String(value) : "0.0") +
                    "&sensor_id_3=3&value_3=" + (sensor_id == LDR_SENSOR_ID ? String(value) : "0.0");
   
   http.POST(postData);
   http.end();
 }
 
// Gets current time as formatted string
 String getCurrentTime() 
 {
   // Return default time if NTP not ready
   if (!timeInitialized && !timeClient.forceUpdate()) 
   {
     return DEFAULT_TIME_STRING;
   }
   
   // Format current time
   time_t epochTime = timeClient.getEpochTime();
   struct tm *ptm = gmtime(&epochTime);
   char timeString[TIME_STRING_BUFFER_SIZE];
   strftime(timeString, sizeof(timeString), TIME_STRING_FORMAT, ptm);
   return String(timeString);
 }
 
// Plays a random sound from the specified track list
 void playRandomSound(const int tracks[], int count) 
 {
   // Don't play if DFPlayer not ready, no tracks, or in night mode
   if (!dfPlayerInitialized || count <= 0 || nightMode) return;
   
   static unsigned long lastPlayTime = 0; // Track last play time for debouncing
   unsigned long currentMillis = millis();
   
   // Only play if minimum interval has passed
   if (currentMillis - lastPlayTime > DFPLAYER_MIN_PLAY_INTERVAL_MS) 
   {
     int randomIndex = random(0, count);
     dfPlayer.play(tracks[randomIndex]);
     lastPlayTime = currentMillis;
   }
 }
 
// Sets up the web server with all required endpoints
 void setupServer() 
 {
   // Root endpoint simple status response
   server.on("/", HTTP_GET, []() 
   {
     server.send(HTTP_OK_CODE, CONTENT_TYPE_TEXT, SERVER_ROOT_RESPONSE);
   });
 
   // Appointments endpoint returns current appointments as JSON
   server.on("/appointments", HTTP_GET, []() 
   {
     String json;
     serializeJson(appointments, json);
     server.send(HTTP_OK_CODE, CONTENT_TYPE_JSON, json);
   });
 
   // Character switch endpoint cycles to next character mode
   server.on("/switch_character", HTTP_GET, []() 
   {
     currentCharacter = static_cast<CharacterMode>((currentCharacter + CHARACTER_CYCLE_STEP) % CHARACTER_MODE_COUNT);
     String currentCharacterName = currentCharacterNames[currentCharacter];
 
     // Update display and play sound (if not in night mode)
     updateDisplay(LCD_MSG_SWITCHED, currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);
     if (dfPlayerInitialized && !nightMode) 
     {
       switch (currentCharacter) 
       {
         case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
         case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
         case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
       }
     }
 
     // Set notification timer
     showingCharacterNotification = true;
     characterNotificationStart = millis();
 
     // Return success response
     server.send(HTTP_OK_CODE, CONTENT_TYPE_TEXT, SERVER_SWITCH_RESPONSE + currentCharacterName);
   });
 
   // Play random endpoint plays random voice line for current character
   server.on("/play_random", HTTP_GET, []() 
   {
     if (dfPlayerInitialized && !nightMode) 
     {
       // Play random sound based on current character
       switch (currentCharacter) 
       {
         case COMMISSAR:
           playRandomSound(COMMISSAR_ACTIVE_TRACKS, sizeof(COMMISSAR_ACTIVE_TRACKS)/sizeof(int));
           break;
         case LEMAN_RUSS:
           playRandomSound(LEMAN_RUSS_ACTIVE_TRACKS, sizeof(LEMAN_RUSS_ACTIVE_TRACKS)/sizeof(int));
           break;
         case BANEBLADE:
           playRandomSound(BANEBLADE_ACTIVE_TRACKS, sizeof(BANEBLADE_ACTIVE_TRACKS)/sizeof(int));
           break;
       }

       server.send(HTTP_OK_CODE, CONTENT_TYPE_TEXT, "Random voice line played.");
     } 
     
     else 
     {
       // Return error if conditions not met
       server.send(HTTP_SERVICE_UNAVAILABLE_CODE, CONTENT_TYPE_TEXT, "Night mode active or DFPlayer unavailable.");
     }
   });
 
   server.begin();
   Serial.println(SERIAL_MSG_HTTP_STARTED);
 }
 
// Handles hourly servo movement 
 void handleHourlyServo(unsigned long currentMillis) 
 {
  static int lastTriggeredHour = INVALID_HOUR; // Track last hour servo was activated
   
   // Don't activate if time not initialized
   if (!timeInitialized) return;
   
   // Get current time
   timeClient.update();
   int currentHour = timeClient.getHours();
   int currentMinute = timeClient.getMinutes();
   
   // Trigger at top of each new hour
   if (currentMinute == 0 && currentHour != lastTriggeredHour) 
   {
     servoTargetPosition = SERVO_MIN_POSITION;
     servoMoving = true;
     Serial.println(SERIAL_MSG_SERVO_MOVING_MIN);
     lastTriggeredHour = currentHour;
     servoReturnTime = currentMillis + HOURLY_SERVO_DURATION_MS;
   }
   
   if (servoTargetPosition == SERVO_MIN_POSITION && currentMillis >= servoReturnTime) 
   {
     servoTargetPosition = SERVO_MAX_POSITION;
     servoMoving = true;
     Serial.println(SERIAL_MSG_SERVO_MOVING_MAX);
   }
 
   if (currentMillis - lastServoUpdate >= SERVO_UPDATE_INTERVAL_MS) 
   {
     if (servoCurrentPosition != servoTargetPosition) 
     {
      int direction = (servoTargetPosition > servoCurrentPosition) ? SERVO_DIRECTION_FORWARD  : SERVO_DIRECTION_BACKWARD;

       servoCurrentPosition += direction * SERVO_SPEED;
       servoCurrentPosition = constrain(servoCurrentPosition, SERVO_MIN_POSITION, SERVO_MAX_POSITION);
       
       servo1.write(servoCurrentPosition);
       lastServoUpdate = currentMillis;
     }
     
     else if (servoMoving) 
     {
       servoMoving = false;
       Serial.println(SERIAL_MSG_SERVO_REACHED);
     }
   }
 }

// Performs a self-test of the servo motor by moving it through its full range
 void performServoSelfTest() 
 {
   updateDisplay(LCD_MSG_TEST_SERVO, "", SHOWING_INITIALIZATION);
   Serial.println(SERIAL_MSG_TEST_SERVO);
 
   unsigned long testStart = millis();
   unsigned long lastMove = millis();
   int testPhase = INITIAL_PHASE;
   bool testComplete = false;
 
   // Continue testing until all phases are complete
   while (!testComplete) 
   {
     unsigned long now = millis();
 
     // Handle web server and WiFi during test to maintain responsiveness
     server.handleClient();
     if (WiFi.status() != WL_CONNECTED) connectToWiFi();
 
     // Control servo movement based on current test phase
     switch (testPhase) 
     {
       case INITIAL_PHASE:
         // Move servo to minimum position in steps
         if (now - lastMove >= SERVO_UPDATE_INTERVAL_MS && servoCurrentPosition > SERVO_MIN_POSITION) 
         {
            servoCurrentPosition -= SERVO_SPEED;
            servoCurrentPosition = max(servoCurrentPosition, SERVO_MIN_POSITION);
            servo1.write(servoCurrentPosition);
            lastMove = now;
         } 
         
         else if (servoCurrentPosition <= SERVO_MIN_POSITION) 
         {
           // Transition to pause phase when minimum position reached
           testPhase = PAUSE_PHASE;
           testStart = now;
         }
         break;
 
       case PAUSE_PHASE:
         // Pause at minimum position for defined duration
         if (now - testStart >= SERVO_TEST_PAUSE_MS) 
         {
            testPhase = RETURN_PHASE;
            lastMove = now;
         }
         break;
 
       case RETURN_PHASE:
         // Return servo to maximum position in steps
         if (now - lastMove >= SERVO_UPDATE_INTERVAL_MS && servoCurrentPosition < SERVO_MAX_POSITION) 
          {
            servoCurrentPosition += SERVO_SPEED;
            servoCurrentPosition = min(servoCurrentPosition, SERVO_MAX_POSITION);
            servo1.write(servoCurrentPosition);
            lastMove = now;
         } 
         
         else if (servoCurrentPosition >= SERVO_MAX_POSITION) 
         {
           // Mark test complete when maximum position reached
           testComplete = true;
           Serial.println(SERIAL_MSG_SERVO_TEST_COMPLETE);
         }
         break;
     }
   }
 }
 
// Setup
 void setup() 
 {
   // Initialize serial communication for debugging
   Serial.begin(SERIAL_BAUD);
   
   // Initialize software serial for DFPlayer communication
   mySoftwareSerial.begin(DFPLAYER_BAUD, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
 
   // Initialize I2C communication for LCD
   Wire.begin(LCD_SDA_PIN, LCD_SCL_PIN);
 
   // Initialize and configure LCD display
   lcd.init();
   lcd.backlight();
   updateDisplay(LCD_MSG_INIT, "", SHOWING_INITIALIZATION);
 
   // Initialize and configure 7 segment display
   display.setBrightness(DISPLAY_BRIGHTNESS);
   display.clear();
   display.showNumberDec(DISPLAY_DEFAULT_CODE);
 
   // Configure sensor pins
   pinMode(BUTTON_PIN, INPUT_PULLUP);  
   pinMode(PIR_PIN, INPUT);            
   pinMode(LDR_PIN, INPUT);       
 
   // Read initial sensor values
   lastLDRValue = analogRead(LDR_PIN);
   lastPIRState = digitalRead(PIR_PIN);
   nightMode = (lastLDRValue < NIGHT_MODE_ENTER_THRESHOLD);
 
   // Initialize and test servo motor
   if (!servo1.attach(SERVO_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX)) 
   {
     Serial.println(SERIAL_MSG_SERVO_FAIL);
   } 
   
   else 
   {
     servo1.write(servoCurrentPosition);
     Serial.println(SERIAL_MSG_SERVO_ATTACHED);
   }
 
   // Perform servo self-test routine
   performServoSelfTest();
 
   // Initialize DFPlayer audio module
   if (dfPlayer.begin(mySoftwareSerial)) 
   {
     dfPlayer.volume(DFPLAYER_VOLUME);
     dfPlayerInitialized = true;
     Serial.println(SERIAL_MSG_DFPLAYER_INIT);
   } 
   
   else 
   {
     Serial.println(SERIAL_MSG_DFPLAYER_FAIL);
   }
 
   // Connect to WiFi network
   connectToWiFi();
   Serial.print(SERIAL_MSG_IP_ADDRESS);
   Serial.println(WiFi.localIP());
 
   // Initialize NTP time client
   timeClient.begin();
   initializeTime();
 
   // Set up web server endpoints
   setupServer();
 }
 
// Loop
 void loop() 
 {
   unsigned long currentMillis = millis();
 
   // Handle any incoming web server requests
   server.handleClient();
 
   // Manage WiFi connection state with timeout
   static unsigned long wifiTimeout = 0;
   if (WiFi.status() != WL_CONNECTED) 
   {
     // Attempt to reconnect if delay period has passed
     if (currentMillis - lastReconnectAttempt >= WIFI_RECONNECT_DELAY_MS) 
     {
       // Initialize timeout on first attempt
       if (wifiTimeout == 0) wifiTimeout = currentMillis + WIFI_TIMEOUT_MS; 
       
       // Only try to reconnect if within timeout window
       if (currentMillis < wifiTimeout) {
         lastReconnectAttempt = currentMillis;
         connectToWiFi();
       } 
       
       else 
       {
         // Continue operation offline after timeout
         Serial.println(SERIAL_MSG_WIFI_TIMEOUT);
         wifiTimeout = 0;
       }
     }
   } 
   
   else 
   {
     // Reset timeout when connected
     wifiTimeout = 0;
   }
 
   // Update 7-segment display clock at regular interval
   if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL_MS) 
   {
     if (timeInitialized) 
     {
       update7SegmentDisplay();
     }

     lastTimeUpdate = currentMillis;
   }
 
   // Retry NTP time synchronization if not yet initialized
   if (!timeInitialized && currentMillis - lastNtpRetry >= NTP_RETRY_INTERVAL_MS) 
   {
     initializeTime();
     lastNtpRetry = currentMillis;
   }
 
   // Refresh appointment data if needed
   if (!dataRefreshed) 
   {
     fetchAppointments();
     dataRefreshed = true;
     lastAppointmentRefresh = currentMillis;
   }
 
   // Mark data as needing refresh when interval elapses
   if (currentMillis - lastAppointmentRefresh >= APPOINTMENT_REFRESH_INTERVAL_MS) 
   {
     dataRefreshed = false;
   }
 
   // Update display with current appointment
   displayCurrentAppointment(currentMillis);
   
   // Process all sensor inputs
   handleSensors(currentMillis);
   
   // Handle hourly servo movement (visual clock function)
   handleHourlyServo(currentMillis);
 
   // Clear character notification after display duration
   if (showingCharacterNotification && currentMillis - characterNotificationStart >= CHARACTER_NOTIFICATION_DURATION_MS) 
    {
      showingCharacterNotification = false;
      currentDisplayState = SHOWING_APPOINTMENTS;
      lastDisplayChange = 0;
    }
 
   // Watchdog timer to restart after maximum uptime
   static unsigned long lastWatchdogCheck = 0;
   if (currentMillis - lastWatchdogCheck >= WATCHDOG_INTERVAL_MS) 
   { 
     lastWatchdogCheck = currentMillis;
     if (millis() > MAX_UPTIME_MS) 
     { 
       ESP.restart(); 
     }
   }
 }