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
 
 // =============================================
 // HARDWARE CONFIGURATION CONSTANTS
 // =============================================
 
 /* LCD Configuration Constants */
 const int LCD_I2C_ADDRESS = 0x27;       // I2C address of the LCD
 const int LCD_COLUMNS = 16;             // Number of columns in LCD
 const int LCD_ROWS = 2;                 // Number of rows in LCD
 const int LCD_SDA_PIN = 21;             // SDA pin for I2C communication
 const int LCD_SCL_PIN = 22;             // SCL pin for I2C communication
 
 /* 7-segment Display Constants */
 const int DISPLAY_CLK_PIN = 18;         // Clock pin for TM1637 display
 const int DISPLAY_DIO_PIN = 19;         // Data pin for TM1637 display
 const uint8_t DISPLAY_BRIGHTNESS = 0x0a;// Brightness level (0-7)
 const uint8_t DISPLAY_COLON_MASK = 0b01000000; // Mask for colon in time display
 const int DISPLAY_ERROR_CODE = 9999;    // Displayed when time not initialized
 const int DISPLAY_DEFAULT_CODE = 8888;  // Default display during initialization
 
 /* DFPlayer Mini Audio Player Constants */
 const int DFPLAYER_RX_PIN = 16;         // RX pin for DFPlayer communication
 const int DFPLAYER_TX_PIN = 17;         // TX pin for DFPlayer communication
 const int DFPLAYER_VOLUME = 7;          // Default volume level (0-30)
 const int DFPLAYER_MIN_PLAY_INTERVAL_MS = 100; // Minimum delay between tracks
 const int DFPLAYER_BAUD = 9600;         // Baud rate for DFPlayer communication
 
 /* Servo Motor Configuration */
 const int SERVO_PIN = 27;               // GPIO pin connected to servo
 const int SERVO_MIN_POSITION = 90;      // Minimum servo position (degrees)
 const int SERVO_MAX_POSITION = 180;     // Maximum servo position (degrees)
 const int SERVO_SPEED = 1;              // Degrees change per update interval
 const unsigned long SERVO_UPDATE_INTERVAL_MS = 20; // Servo position update interval
 const unsigned long SERVO_TEST_PAUSE_MS = 1000;    // Pause during servo test
 const unsigned long HOURLY_SERVO_DURATION_MS = 60000; // Duration of hourly servo movement
 const int SERVO_PULSE_MIN = 500;        // Minimum pulse width for servo (μs)
 const int SERVO_PULSE_MAX = 2500;       // Maximum pulse width for servo (μs)
 
 /* Sensor Pin Assignments */
 const int BUTTON_PIN = 14;              // GPIO pin for mode switch button
 const int PIR_PIN = 35;                 // GPIO pin for PIR motion sensor
 const int LDR_PIN = 34;                 // GPIO pin for light sensor (LDR)
 
 // =============================================
 // COMMUNICATION AND NETWORK CONSTANTS
 // =============================================
 
 /* NTP Client Configuration */
 const char* NTP_SERVER = "pool.ntp.org";// NTP server for time synchronization
 const long NTP_UTC_OFFSET_SECONDS = 3600; // UTC offset in seconds
 const char* DEFAULT_TIME_STRING = "1970-01-01 00:00:00"; // Fallback time string
 const size_t TIME_STRING_BUFFER_SIZE = 25; // Buffer size for time strings
 const char* TIME_STRING_FORMAT = "%Y-%m-%d %H:%M:%S"; // Time format string
 const int HOURS_PER_DAY = 24;           // Number of hours in a day
 
 /* API Endpoints */
 const char* APPOINTMENT_API_URL = "http://100.74.252.69/Database/Appointment/recover_appointment.php";
 const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";
 
 /* HTTP Server Configuration */
 const char* WIFI_AP_NAME = "SmartCalendarAP"; // WiFi AP name for configuration
 const char* SERVER_ROOT_RESPONSE = "Smart Calendar is online!"; // Default web response
 const char* CONTENT_TYPE_TEXT = "text/plain"; // MIME type for text responses
 const char* CONTENT_TYPE_JSON = "application/json"; // MIME type for JSON responses
 const int HTTP_OK_CODE = 200;            // HTTP 200 OK status code
 const int HTTP_SERVICE_UNAVAILABLE_CODE = 503; // HTTP 503 status code
 
 /* Serial Communication */
 const int SERIAL_BAUD = 115200;         // Baud rate for serial monitor
 
 // =============================================
 // STATE AND LOGIC CONSTANTS
 // =============================================
 
 /* Device Configuration */
 const int DEVICE_ID = 1;                // Unique identifier for this device
 
 /* Sensor IDs for API Communication */
 enum SensorIds {
   BUTTON_SENSOR_ID = 1,                 // ID for button press events
   PIR_SENSOR_ID = 2,                    // ID for motion detection events
   LDR_SENSOR_ID = 3                     // ID for light level readings
 };
 
 /* Timing and Interval Constants */
 const unsigned long APPOINTMENT_REFRESH_INTERVAL_MS = 5000; // Appointment update interval
 const unsigned long DISPLAY_INTERVAL_MS = 3000;    // Time between display updates
 const unsigned long SENSOR_CHECK_INTERVAL_MS = 50; // Interval between sensor checks
 const unsigned long TIME_UPDATE_INTERVAL_MS = 60000; // Time sync interval
 const unsigned long NTP_RETRY_INTERVAL_MS = 30000; // Retry interval for NTP sync
 const unsigned long BUTTON_DEBOUNCE_TIME_MS = 50;  // Debounce time for button
 const unsigned long WIFI_RECONNECT_DELAY_MS = 1000; // WiFi reconnect delay
 const unsigned long DELAY_WIFI_FAIL_MS = 3000;     // Delay before restart on WiFi fail
 const unsigned long DELAY_AFTER_RESTART_MS = 5000; // Delay after restart
 const unsigned long CHARACTER_NOTIFICATION_DURATION_MS = 2000; // Character switch message duration
 const unsigned long PIR_INTERVAL_MS = 300000;      // Minimum interval between PIR triggers
 const unsigned long WIFI_TIMEOUT_MS = 30000;       // WiFi connection timeout
 const unsigned long WATCHDOG_INTERVAL_MS = 60000;  // Watchdog timer interval
 const unsigned long MAX_UPTIME_MS = 86400000;      // Maximum uptime before restart
 
 /* Night Mode Thresholds */
 const int NIGHT_MODE_ENTER_THRESHOLD = 650; // LDR value to enter night mode
 const int NIGHT_MODE_EXIT_THRESHOLD = 750;  // LDR value to exit night mode
 
 /* Character Modes */
 enum CharacterMode { 
   COMMISSAR,                       // First character mode
   LEMAN_RUSS,                      // Second character mode
   BANEBLADE,                       // Third character mode
   CHARACTER_MODE_COUNT             // Total number of character modes
 };
 
 /* Servo Test Phases */
 const int INITIAL_PHASE = 0;        // Initial phase - moving to min position
 const int PAUSE_PHASE = 1;          // Pause phase at min position
 const int RETURN_PHASE = 2;         // Return phase to max position
 
 /* Logic Constants */
 const int VALUE_TRUE = 1;           // Representation of true value
 const int VALUE_FALSE = 0;          // Representation of false value
 
 /* Date String Processing Constants */
 const int DATE_STRING_LENGTH_THRESHOLD = 16; // Minimum valid date string length
 const int INDEX_DAY_START = 8;      // Start index of day in date string
 const int INDEX_DAY_END = 10;       // End index of day in date string
 const int INDEX_MONTH_START = 5;    // Start index of month in date string
 const int INDEX_MONTH_END = 7;      // End index of month in date string
 const int INDEX_TIME_START = 11;    // Start index of time in date string
 const int INDEX_TIME_END = 16;      // End index of time in date string
 
 // =============================================
 // MESSAGE CONSTANTS
 // =============================================
 
 /* LCD Display Messages */
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
 
 /* Serial Monitor Messages */
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
 
 /* Web Server Responses */
 const char* SERVER_SWITCH_RESPONSE = "Character changed to ";
 
 // =============================================
 // GLOBAL VARIABLES
 // =============================================
 
 /* Hardware Objects */
 LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
 TM1637Display display(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);
 WiFiUDP ntpUDP;
 WebServer server(80);
 NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_UTC_OFFSET_SECONDS);
 HardwareSerial mySoftwareSerial(2);
 DFRobotDFPlayerMini dfPlayer;
 Servo servo1;
 
 /* State Variables */
 CharacterMode currentCharacter = COMMISSAR; // Current selected character mode
 String currentCharacterNames[CHARACTER_MODE_COUNT] = {
   "Commissar",                 // Name for COMMISSAR mode
   "Leman Russ",                // Name for LEMAN_RUSS mode
   "Baneblade"                  // Name for BANEBLADE mode
 };
 
 DynamicJsonDocument appointments(2048);    // Stores current appointments
 std::vector<String> previousAppointmentIds; // Tracks previously seen appointments
 
 /* Timing Trackers */
 unsigned long lastAppointmentRefresh = 0;  // Last time appointments were fetched
 unsigned long lastDisplayChange = 0;       // Last time display was updated
 unsigned long lastSensorCheck = 0;         // Last time sensors were checked
 unsigned long lastTimeUpdate = 0;          // Last time clock was updated
 unsigned long lastNtpRetry = 0;            // Last NTP sync attempt
 unsigned long lastButtonPress = 0;         // Last time button was pressed
 unsigned long lastReconnectAttempt = 0;    // Last WiFi reconnect attempt
 unsigned long lastHourCheck = 0;           // Last hourly check
 unsigned long characterNotificationStart = 0; // When character notification started
 unsigned long lastPIRTime = 0;             // Last PIR sensor trigger time
 
 /* State Trackers */
 int currentAppointmentIndex = 0;           // Index of currently displayed appointment
 int lastButtonState = HIGH;                // Previous button state (for debouncing)
 int lastPIRState = LOW;                    // Previous PIR sensor state
 int lastLDRValue = -1;                     // Previous LDR sensor reading
 
 /* System Status Flags */
 bool timeInitialized = false;              // Whether NTP time has been synced
 bool dfPlayerInitialized = false;          // Whether DFPlayer is ready
 bool dataRefreshed = false;                // Whether appointments need refresh
 bool nightMode = false;                    // Whether device is in night mode
 bool showingCharacterNotification = false;  // Whether showing character switch message
 bool pirLocked = false;                    // Whether PIR is in cooldown period
 
 /* Servo Control Variables */
 int servoCurrentPosition = SERVO_MAX_POSITION; // Current servo position
 int servoTargetPosition = SERVO_MAX_POSITION;  // Target servo position
 unsigned long lastServoUpdate = 0;         // Last servo position update
 unsigned long servoReturnTime = 0;         // Time when servo should return
 bool servoMoving = false;                  // Whether servo is currently moving
 
 /* Sound Track Numbers */
 const int COMMISSAR_TRACK = 31;            // Main track for Commissar mode
 const int LEMAN_RUSS_TRACK = 42;           // Main track for Leman Russ mode
 const int BANEBLADE_TRACK = 53;            // Main track for Baneblade mode
 const int DARK_TRACK = 35;                 // Track played when entering night mode
 const int LIGHT_TRACK = 36;                // Track played when exiting night mode
 
 /* Character-specific Sound Tracks */
 const int COMMISSAR_ACTIVE_TRACKS[] = {1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,21,22,28,33,34};
 const int COMMISSAR_DELETED_TRACKS[] = {11,20,23,24,25,26,27,29,30,32};
 const int LEMAN_RUSS_ACTIVE_TRACKS[] = {43,44,45,46,47,48};
 const int LEMAN_RUSS_DELETED_TRACKS[] = {49,50,51};
 const int BANEBLADE_ACTIVE_TRACKS[] = {52,53,54,55,56,57,58,59,60,61,62,63,67,68,69,70,71,72,73,74};
 const int BANEBLADE_DELETED_TRACKS[] = {64,65,66};
 const int PIR_TRACKS[] = {37,38,39,40,41}; // Tracks played on motion detection
 
 /* Display States */
 enum DisplayState {
   SHOWING_CHARACTER_NOTIFICATION,    // Displaying character switch message
   SHOWING_APPOINTMENTS,              // Displaying appointment information
   SHOWING_NO_APPOINTMENTS,           // Displaying "no appointments" message
   SHOWING_INITIALIZATION             // Displaying initialization messages
 };
 DisplayState currentDisplayState = SHOWING_INITIALIZATION; // Current display state
 
 // =============================================
 // FUNCTION DEFINITIONS
 // =============================================
 
 /**
  * Updates the LCD display with new content if it has changed
  * 
  * @param line1 The text to display on the first line
  * @param line2 The text to display on the second line
  * @param newState The new display state to transition to
  */
 void updateDisplay(String line1, String line2, DisplayState newState) {
   static String lastLine1 = "";      // Previously displayed first line
   static String lastLine2 = "";      // Previously displayed second line
   static DisplayState lastState = SHOWING_INITIALIZATION; // Previous state
 
   // Only update display if content or state has changed
   if (newState != lastState || line1 != lastLine1 || line2 != lastLine2) {
     lcd.clear();  // Clear the display before showing new content
     
     // Display first line (truncated to LCD column count)
     lcd.setCursor(0, 0);
     lcd.print(line1.substring(0, LCD_COLUMNS));
 
     // Display second line (truncated to LCD column count)
     lcd.setCursor(0, 1);
     lcd.print(line2.substring(0, LCD_COLUMNS));
 
     // Update tracking variables
     lastLine1 = line1;
     lastLine2 = line2;
     lastState = newState;
     currentDisplayState = newState;
   }
 }
 
 /**
  * Updates the 7-segment display with current time or error code
  */
 void update7SegmentDisplay() {
   // Show error code if time hasn't been initialized
   if (!timeInitialized) {
     display.showNumberDec(DISPLAY_ERROR_CODE);
     return;
   }
   
   // Update time from NTP server and display it
   timeClient.update();
   int hour = timeClient.getHours() + 1; // Adjust for timezone
   // Handle 24-hour rollover
   if (hour >= HOURS_PER_DAY) hour -= HOURS_PER_DAY;
   int minute = timeClient.getMinutes();
   // Display time with colon (e.g., 12:34)
   display.showNumberDecEx(hour * 100 + minute, DISPLAY_COLON_MASK, true);
 }
 
 /**
  * Initializes time synchronization with NTP server
  */
 void initializeTime() {
   // Attempt to force an NTP time update
   if (timeClient.forceUpdate()) {
     timeInitialized = true;
     Serial.println(SERIAL_MSG_TIME_INIT);
     updateDisplay(LCD_MSG_TIME_INIT, LCD_MSG_FETCHING, SHOWING_INITIALIZATION);
   } else {
     Serial.println(SERIAL_MSG_TIME_FAIL);
     updateDisplay(LCD_MSG_TIME_FAIL, LCD_MSG_RETRYING, SHOWING_INITIALIZATION);
   }
 }
 
 /**
  * Connects to WiFi using WiFiManager
  * 
  * If connection fails, restarts the device after a delay
  */
 void connectToWiFi() {
   WiFiManager wifiManager;
   updateDisplay(LCD_MSG_WIFI_START, "", SHOWING_INITIALIZATION);
   
   // Attempt to connect to WiFi or start configuration AP
   if (!wifiManager.autoConnect(WIFI_AP_NAME)) {
     // Connection failed - show message and restart
     updateDisplay(LCD_MSG_WIFI_FAIL, LCD_MSG_RESTARTING, SHOWING_INITIALIZATION);
     delay(DELAY_WIFI_FAIL_MS);
     ESP.restart();
     delay(DELAY_AFTER_RESTART_MS);
   }
   
   // Connection successful - proceed to time sync
   updateDisplay(LCD_MSG_WIFI_CONNECTED, LCD_MSG_SYNCING, SHOWING_INITIALIZATION);
   initializeTime();
 }
 
 /**
  * Fetches appointments from the API server
  */
 void fetchAppointments() {
   HTTPClient http;
   http.begin(APPOINTMENT_API_URL);
 
   // Make GET request to appointment API
   int httpCode = http.GET();
   
   // Process response if successful
   if (httpCode == HTTP_CODE_OK) {
     String payload = http.getString();
     DynamicJsonDocument newAppointments(2048);
     deserializeJson(newAppointments, payload);
     checkForAppointmentChanges(newAppointments);
   } else {
     // Show error if fetch failed
     updateDisplay(LCD_MSG_FETCH_FAIL, LCD_MSG_RETRYING, SHOWING_INITIALIZATION);
   }
 
   http.end(); // Free resources
 }
 
 /**
  * Compares new appointments with previous ones and triggers appropriate actions
  * 
  * @param newAppointments JSON document containing newly fetched appointments
  */
 void checkForAppointmentChanges(DynamicJsonDocument& newAppointments) {
   std::vector<String> currentIds;    // Track IDs of current appointments
   bool changesDetected = false;      // Flag for any detected changes
 
   // Check for new or modified appointments
   for (JsonObject newApp : newAppointments.as<JsonArray>()) {
     String newAppId = newApp["appointment_id"].as<String>();
     currentIds.push_back(newAppId);
 
     bool isNew = true;
     bool isModified = false;
     
     // Compare with previous appointments
     for (String prevId : previousAppointmentIds) {
       if (prevId == newAppId) {
         isNew = false;
 
         // Check if existing appointment has changed
         for (JsonObject oldApp : appointments.as<JsonArray>()) {
           if (oldApp["appointment_id"].as<String>() == newAppId) {
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
     if ((isNew || isModified) && !nightMode) {
       playAppropriateSound(VALUE_TRUE);
       changesDetected = true;
     }
   }
 
   // Check for deleted appointments
   for (String prevId : previousAppointmentIds) {
     bool stillExists = false;
     for (JsonObject newApp : newAppointments.as<JsonArray>()) {
       if (newApp["appointment_id"].as<String>() == prevId) {
         stillExists = true;
         break;
       }
     }
 
     // Play sound for deleted appointments (unless in night mode)
     if (!stillExists && !nightMode) {
       playAppropriateSound(VALUE_FALSE);
       changesDetected = true;
     }
   }
 
   // Update stored appointments if changes detected or first load
   if (changesDetected || previousAppointmentIds.empty()) {
     appointments = newAppointments;
     previousAppointmentIds = currentIds;
     currentAppointmentIndex = 0;
     lastDisplayChange = millis();
   }
 }
 
 /**
  * Plays an appropriate sound based on current character mode and event type
  * 
  * @param isActiveOrUpdated Whether the sound is for an active/updated (true) 
  *                          or deleted (false) appointment
  */
 void playAppropriateSound(bool isActiveOrUpdated) {
   // Don't play sounds if DFPlayer isn't ready or in night mode
   if (!dfPlayerInitialized || nightMode) return;
   
   // Select sound set based on current character mode
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
 
 /**
  * Displays the current appointment on the LCD
  * 
  * @param currentMillis Current time in milliseconds (for timing display rotation)
  */
 void displayCurrentAppointment(unsigned long currentMillis) {
   // Don't update display if showing character notification
   if (showingCharacterNotification) return;
   
   // Show "no appointments" message if none are available
   if (appointments.size() == 0) {
     if (currentDisplayState != SHOWING_NO_APPOINTMENTS) {
       updateDisplay(LCD_MSG_NO_APPOINTMENTS, "", SHOWING_NO_APPOINTMENTS);
     }
     return;
   }
 
   // Rotate to next appointment at defined interval
   if (currentMillis - lastDisplayChange >= DISPLAY_INTERVAL_MS) {
     currentAppointmentIndex = (currentAppointmentIndex + 1) % appointments.size();
     lastDisplayChange = currentMillis;
   }
 
   // Get current appointment data
   JsonObject appointment = appointments[currentAppointmentIndex];
   String task = appointment["task"].as<String>();
   String dateTime = appointment["date_hour"].as<String>();
 
   // Format date string (DD/MM HH:MM)
   String line2 = "";
   if (dateTime.length() >= DATE_STRING_LENGTH_THRESHOLD) {
     line2 = dateTime.substring(INDEX_DAY_START, INDEX_DAY_END) + "/" + 
             dateTime.substring(INDEX_MONTH_START, INDEX_MONTH_END) + " " + 
             dateTime.substring(INDEX_TIME_START, INDEX_TIME_END);
   } else {
     line2 = LCD_MSG_INVALID_DATE;
   }
 
   // Truncate task if too long for display
   if (task.length() > LCD_COLUMNS) {
     task = task.substring(0, LCD_COLUMNS);
   }
 
   // Update display with appointment info
   updateDisplay(task, line2, SHOWING_APPOINTMENTS);
 }
 
 /**
  * Checks and handles sensor inputs
  * 
  * @param currentMillis Current time in milliseconds (for timing)
  */
 void handleSensors(unsigned long currentMillis) {
   // Only check sensors at defined interval
   if (currentMillis - lastSensorCheck >= SENSOR_CHECK_INTERVAL_MS) {
     lastSensorCheck = currentMillis;
     
     // Read all sensor values
     int buttonState = digitalRead(BUTTON_PIN);
     int pirState = digitalRead(PIR_PIN);
     int ldrValue = analogRead(LDR_PIN);
     static bool wasInNightMode = nightMode;
     
     // Check for night mode transition
     bool newNightMode = (ldrValue < NIGHT_MODE_ENTER_THRESHOLD);
     if (newNightMode != nightMode) {
       nightMode = newNightMode;
 
       // Play appropriate day/night sound if DFPlayer is ready
       if (dfPlayerInitialized) {
         dfPlayer.play(nightMode ? DARK_TRACK : LIGHT_TRACK);
       }
 
       wasInNightMode = nightMode;
       sendSensorData(LDR_SENSOR_ID, ldrValue);
     }
 
     // Check for button press (with debouncing)
     if (buttonState == LOW && lastButtonState == HIGH && 
         (currentMillis - lastButtonPress) >= BUTTON_DEBOUNCE_TIME_MS) {
       sendSensorData(BUTTON_SENSOR_ID, VALUE_TRUE);
       
       // Cycle to next character mode
       currentCharacter = static_cast<CharacterMode>((currentCharacter + 1) % CHARACTER_MODE_COUNT);
       String currentCharacterName = currentCharacterNames[currentCharacter];
       
       // Show character switch notification
       updateDisplay(LCD_MSG_SWITCHED, currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);
       
       // Play character-specific sound if not in night mode
       if (dfPlayerInitialized && !nightMode) {
         switch(currentCharacter) {
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
 
     // Check for motion detection (with cooldown period)
     if (pirState == HIGH && lastPIRState == LOW && !pirLocked) {
       sendSensorData(PIR_SENSOR_ID, VALUE_TRUE);
       
       // Play random motion sound if not in night mode
       if (dfPlayerInitialized && !nightMode) {
         playRandomSound(PIR_TRACKS, sizeof(PIR_TRACKS)/sizeof(int));
       }
 
       // Start cooldown period
       lastPIRTime = currentMillis;
       pirLocked = true;
     }
 
     // Reset PIR lock after cooldown period
     if (pirLocked && currentMillis - lastPIRTime >= PIR_INTERVAL_MS) {
       pirLocked = false;
     }
 
     // Update previous sensor states
     lastButtonState = buttonState;
     lastPIRState = pirState;
     lastLDRValue = ldrValue;
   }
 }
 
 /**
  * Sends sensor data to the API server
  * 
  * @param sensor_id Which sensor is reporting data
  * @param value The sensor reading to send
  */
 void sendSensorData(int sensor_id, float value) {
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
 
 /**
  * Gets current time as formatted string
  * 
  * @return String containing current time in "YYYY-MM-DD HH:MM:SS" format
  *         or default time if NTP not initialized
  */
 String getCurrentTime() {
   // Return default time if NTP not ready
   if (!timeInitialized && !timeClient.forceUpdate()) {
     return DEFAULT_TIME_STRING;
   }
   
   // Format current time
   time_t epochTime = timeClient.getEpochTime();
   struct tm *ptm = gmtime(&epochTime);
   char timeString[TIME_STRING_BUFFER_SIZE];
   strftime(timeString, sizeof(timeString), TIME_STRING_FORMAT, ptm);
   return String(timeString);
 }
 
 /**
  * Plays a random sound from the specified track list
  * 
  * @param tracks Array of track numbers to choose from
  * @param count Number of tracks in the array
  */
 void playRandomSound(const int tracks[], int count) {
   // Don't play if DFPlayer not ready, no tracks, or in night mode
   if (!dfPlayerInitialized || count <= 0 || nightMode) return;
   
   static unsigned long lastPlayTime = 0; // Track last play time for debouncing
   unsigned long currentMillis = millis();
   
   // Only play if minimum interval has passed
   if (currentMillis - lastPlayTime > DFPLAYER_MIN_PLAY_INTERVAL_MS) {
     int randomIndex = random(0, count);
     dfPlayer.play(tracks[randomIndex]);
     lastPlayTime = currentMillis;
   }
 }
 
 /**
  * Sets up the web server with all required endpoints
  */
 void setupServer() {
   // Root endpoint - simple status response
   server.on("/", HTTP_GET, []() {
     server.send(HTTP_OK_CODE, CONTENT_TYPE_TEXT, SERVER_ROOT_RESPONSE);
   });
 
   // Appointments endpoint - returns current appointments as JSON
   server.on("/appointments", HTTP_GET, []() {
     String json;
     serializeJson(appointments, json);
     server.send(HTTP_OK_CODE, CONTENT_TYPE_JSON, json);
   });
 
   // Character switch endpoint - cycles to next character mode
   server.on("/switch_character", HTTP_GET, []() {
     currentCharacter = static_cast<CharacterMode>((currentCharacter + 1) % CHARACTER_MODE_COUNT);
     String currentCharacterName = currentCharacterNames[currentCharacter];
 
     // Update display and play sound (if not in night mode)
     updateDisplay(LCD_MSG_SWITCHED, currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);
     if (dfPlayerInitialized && !nightMode) {
       switch (currentCharacter) {
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
 
   // Play random endpoint - plays random voice line for current character
   server.on("/play_random", HTTP_GET, []() {
     if (dfPlayerInitialized && !nightMode) {
       // Play random sound based on current character
       switch (currentCharacter) {
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
     } else {
       // Return error if conditions not met
       server.send(HTTP_SERVICE_UNAVAILABLE_CODE, CONTENT_TYPE_TEXT, 
                  "Night mode active or DFPlayer unavailable.");
     }
   });
 
   server.begin();
   Serial.println(SERIAL_MSG_HTTP_STARTED);
 }
 
 /**
  * Handles hourly servo movement (for visual clock function)
  * 
  * @param currentMillis Current time in milliseconds (for timing)
  */
 void handleHourlyServo(unsigned long currentMillis) {
   static int lastTriggeredHour = -1; // Track last hour servo was activated
   
   // Don't activate if time not initialized
   if (!timeInitialized) return;
   
   // Get current time
   timeClient.update();
   int currentHour = timeClient.getHours();
   int currentMinute = timeClient.getMinutes();
   
   // Trigger at top of each new hour
   if (currentMinute == 0 && currentHour != lastTriggeredHour) {
     servoTargetPosition = SERVO_MIN_POSITION;
     servoMoving = true;
     Serial.println(SERIAL_MSG_SERVO_MOVING_MIN);
     lastTriggeredHour = currentHour;
     servoReturnTime = currentMillis + HOURLY_SERVO_DURATION_MS;
   }
   
   if (servoTargetPosition == SERVO_MIN_POSITION && currentMillis >= servoReturnTime) {
     servoTargetPosition = SERVO_MAX_POSITION;
     servoMoving = true;
     Serial.println(SERIAL_MSG_SERVO_MOVING_MAX);
   }
 
   if (currentMillis - lastServoUpdate >= SERVO_UPDATE_INTERVAL_MS) {
     if (servoCurrentPosition != servoTargetPosition) {
       int direction = (servoTargetPosition > servoCurrentPosition) ? 1 : -1;
       servoCurrentPosition += direction * SERVO_SPEED;
       servoCurrentPosition = constrain(servoCurrentPosition, SERVO_MIN_POSITION, SERVO_MAX_POSITION);
       
       servo1.write(servoCurrentPosition);
       lastServoUpdate = currentMillis;
     }
     else if (servoMoving) {
       servoMoving = false;
       Serial.println(SERIAL_MSG_SERVO_REACHED);
     }
   }
 }
 /**
  * Performs a self-test of the servo motor by moving it through its full range
  * of motion and back to verify proper operation.
  */
 void performServoSelfTest() {
   updateDisplay(LCD_MSG_TEST_SERVO, "", SHOWING_INITIALIZATION);
   Serial.println(SERIAL_MSG_TEST_SERVO);
 
   unsigned long testStart = millis();
   unsigned long lastMove = millis();
   int testPhase = INITIAL_PHASE;
   bool testComplete = false;
 
   // Continue testing until all phases are complete
   while (!testComplete) {
     unsigned long now = millis();
 
     // Handle web server and WiFi during test to maintain responsiveness
     server.handleClient();
     if (WiFi.status() != WL_CONNECTED) connectToWiFi();
 
     // Control servo movement based on current test phase
     switch (testPhase) {
       case INITIAL_PHASE:
         // Move servo to minimum position in steps
         if (now - lastMove >= SERVO_UPDATE_INTERVAL_MS && 
             servoCurrentPosition > SERVO_MIN_POSITION) {
           servoCurrentPosition -= SERVO_SPEED;
           servoCurrentPosition = max(servoCurrentPosition, SERVO_MIN_POSITION);
           servo1.write(servoCurrentPosition);
           lastMove = now;
         } else if (servoCurrentPosition <= SERVO_MIN_POSITION) {
           // Transition to pause phase when minimum position reached
           testPhase = PAUSE_PHASE;
           testStart = now;
         }
         break;
 
       case PAUSE_PHASE:
         // Pause at minimum position for defined duration
         if (now - testStart >= SERVO_TEST_PAUSE_MS) {
           testPhase = RETURN_PHASE;
           lastMove = now;
         }
         break;
 
       case RETURN_PHASE:
         // Return servo to maximum position in steps
         if (now - lastMove >= SERVO_UPDATE_INTERVAL_MS && 
             servoCurrentPosition < SERVO_MAX_POSITION) {
           servoCurrentPosition += SERVO_SPEED;
           servoCurrentPosition = min(servoCurrentPosition, SERVO_MAX_POSITION);
           servo1.write(servoCurrentPosition);
           lastMove = now;
         } else if (servoCurrentPosition >= SERVO_MAX_POSITION) {
           // Mark test complete when maximum position reached
           testComplete = true;
           Serial.println(SERIAL_MSG_SERVO_TEST_COMPLETE);
         }
         break;
     }
   }
 }
 
 /**
  * Main program setup - initializes all hardware components and services
  */
 void setup() {
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
 
   // Initialize and configure 7-segment display
   display.setBrightness(DISPLAY_BRIGHTNESS);
   display.clear();
   display.showNumberDec(DISPLAY_DEFAULT_CODE);
 
   // Configure sensor pins
   pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button with internal pull-up
   pinMode(PIR_PIN, INPUT);            // PIR motion sensor
   pinMode(LDR_PIN, INPUT);            // Light sensor
 
   // Read initial sensor values
   lastLDRValue = analogRead(LDR_PIN);
   lastPIRState = digitalRead(PIR_PIN);
   nightMode = (lastLDRValue < NIGHT_MODE_ENTER_THRESHOLD);
 
   // Initialize and test servo motor
   if (!servo1.attach(SERVO_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX)) {
     Serial.println(SERIAL_MSG_SERVO_FAIL);
   } else {
     servo1.write(servoCurrentPosition);
     Serial.println(SERIAL_MSG_SERVO_ATTACHED);
   }
 
   // Perform servo self-test routine
   performServoSelfTest();
 
   // Initialize DFPlayer audio module
   if (dfPlayer.begin(mySoftwareSerial)) {
     dfPlayer.volume(DFPLAYER_VOLUME);
     dfPlayerInitialized = true;
     Serial.println(SERIAL_MSG_DFPLAYER_INIT);
   } else {
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
 
 /**
  * Main program loop - handles all periodic tasks and event processing
  */
 void loop() {
   unsigned long currentMillis = millis();
 
   // Handle any incoming web server requests
   server.handleClient();
 
   // Manage WiFi connection state with timeout
   static unsigned long wifiTimeout = 0;
   if (WiFi.status() != WL_CONNECTED) {
     // Attempt to reconnect if delay period has passed
     if (currentMillis - lastReconnectAttempt >= WIFI_RECONNECT_DELAY_MS) {
       // Initialize timeout on first attempt
       if (wifiTimeout == 0) wifiTimeout = currentMillis + WIFI_TIMEOUT_MS; 
       
       // Only try to reconnect if within timeout window
       if (currentMillis < wifiTimeout) {
         lastReconnectAttempt = currentMillis;
         connectToWiFi();
       } else {
         // Continue operation offline after timeout
         Serial.println(SERIAL_MSG_WIFI_TIMEOUT);
         wifiTimeout = 0;
       }
     }
   } else {
     // Reset timeout when connected
     wifiTimeout = 0;
   }
 
   // Update 7-segment display clock at regular interval
   if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL_MS) {
     if (timeInitialized) {
       update7SegmentDisplay();
     }
     lastTimeUpdate = currentMillis;
   }
 
   // Retry NTP time synchronization if not yet initialized
   if (!timeInitialized && currentMillis - lastNtpRetry >= NTP_RETRY_INTERVAL_MS) {
     initializeTime();
     lastNtpRetry = currentMillis;
   }
 
   // Refresh appointment data if needed
   if (!dataRefreshed) {
     fetchAppointments();
     dataRefreshed = true;
     lastAppointmentRefresh = currentMillis;
   }
 
   // Mark data as needing refresh when interval elapses
   if (currentMillis - lastAppointmentRefresh >= APPOINTMENT_REFRESH_INTERVAL_MS) {
     dataRefreshed = false;
   }
 
   // Update display with current appointment
   displayCurrentAppointment(currentMillis);
   
   // Process all sensor inputs
   handleSensors(currentMillis);
   
   // Handle hourly servo movement (visual clock function)
   handleHourlyServo(currentMillis);
 
   // Clear character notification after display duration
   if (showingCharacterNotification && 
       currentMillis - characterNotificationStart >= CHARACTER_NOTIFICATION_DURATION_MS) {
     showingCharacterNotification = false;
     currentDisplayState = SHOWING_APPOINTMENTS;
     lastDisplayChange = 0;
   }
 
   // Watchdog timer to restart after maximum uptime
   static unsigned long lastWatchdogCheck = 0;
   if (currentMillis - lastWatchdogCheck >= WATCHDOG_INTERVAL_MS) { 
     lastWatchdogCheck = currentMillis;
     if (millis() > MAX_UPTIME_MS) { 
       ESP.restart(); 
     }
   }
 }