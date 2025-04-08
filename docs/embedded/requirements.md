# 📜 Requirements

To build your embedded device, you need to have a clear idea of the requirements. On this page, you can describe the requirements of your embedded device. This includes the requirements from DLO, but also your own requirements.

Add some images! 😉

| Requirement ID# | Requirement                                                                                                                                       | MoSCoW | Compliant |
|-----------------|---------------------------------------------------------------------------------------------------------------------------------------------------|--------|-----------|
| EMBRQ#01        | The embedded device acts as a client and sends measured sensor data to the application backend over http or https.                              | MUST   | YES       |
| EMBRQ#02        | The embedded device also acts as a server and receives status messages from the application backend over http or https.                         | MUST   | YES        |
| EMBRQ#03        | The embedded device contains at least two types of input sensors (e.g. LDR, buttons, joystick, capacitive touch, etc.).                         | MUST   | YES        |
| EMBRQ#04        | The embedded device contains at least two types of visual and/or sensory outputs (e.g. LED, LED Matrix, 7-segment display, motor, servo, etc.). | MUST   | YES        |
| EMBRQ#05        | The embedded device uses the WifiManager for configuration of SSID and password (PWD) for connecting to the network.                            | MUST   | YES        |



### EMBRQ#01

The embedded device acts as a client and sends measured sensor data to the application backend over http or https.

Code:
```c++

#include <WiFiManager.h>
#include <HTTPClient.h>

// The rest of the includes...

const char* SENSOR_API_URL = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";
const int DEVICE_ID = 1;

// The rest of the constants...

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

// The rest of the functions...
```

### EMBRQ#02

The embedded device also acts as a server and receives status messages from the application backend over http or https.

Code:
```c++

#include <WebServer.h>

// The rest of the includes...

WebServer server(80);
const char* SERVER_ROOT_RESPONSE = "Smart Calendar is online!";
const char* CONTENT_TYPE_TEXT = "text/plain";
const char* CONTENT_TYPE_JSON = "application/json";
const int HTTP_OK_CODE = 200;
const int HTTP_SERVICE_UNAVAILABLE_CODE = 503;

// The rest of the constants...

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
     currentCharacter = static_cast<CharacterMode>((currentCharacter + 1) % CHARACTER_MODE_COUNT);
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

// The rest of the functions...
```

### EMBRQ#03

The embedded device contains three or more types of input sensors (e.g. LDR, buttons, joystick, capacitive touch).

Code:
```c++

#include <Wire.h>     
#include <ArduinoJson.h> 

// The rest of the includes...

 // Input 
 const int BUTTON_PIN = 14;             
 const int PIR_PIN = 35;                
 const int LDR_PIN = 34;

  enum SensorIds 
 {
   BUTTON_SENSOR_ID = 1, 
   PIR_SENSOR_ID = 2,        
   LDR_SENSOR_ID = 3           
 };

// The rest of the constants...

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
       currentCharacter = static_cast<CharacterMode>((currentCharacter + 1) % CHARACTER_MODE_COUNT);
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

 // The rest of the functions...
```

### EMBRQ#04

The embedded device contains three or more types of correctly working visual and/or sensory outputs.

Code:
```c++

#include <LiquidCrystal_I2C.h>   
#include <TM1637Display.h>       
#include <DFRobotDFPlayerMini.h> 
#include <ESP32Servo.h>       

// The rest of the includes...

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

 // The rest of the constants...

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
   int hour = timeClient.getHours() + 1; // Adjust for timezone
   // Handle 24-hour rollover
   if (hour >= HOURS_PER_DAY) hour -= HOURS_PER_DAY;
   int minute = timeClient.getMinutes();
   // Display time with colon (e.g., 12:34)
   int timeDisplay = hour * DISPLAY_TIME_SCALING + minute;
   display.showNumberDecEx(timeDisplay, DISPLAY_COLON_MASK, true);
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

 // Handles hourly servo movement 
 void handleHourlyServo(unsigned long currentMillis) 
 {
   static int lastTriggeredHour = -1; // Track last hour servo was activated
   
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
       int direction = (servoTargetPosition > servoCurrentPosition) ? 1 : -1;
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

// The rest of the functions...
```

### EMBRQ#05

The embedded device uses the WifiManager for configuration of SSID and password (PWD) for connecting to the network.

Code:
```c++

#include <WiFiManager.h>

// The rest of the includes...

const char* WIFI_AP_NAME = "SmartCalendarAP"; 
const char* LCD_MSG_WIFI_START = "Starting WiFi";
const char* LCD_MSG_WIFI_FAIL = "WiFi Failed";
const char* LCD_MSG_WIFI_CONNECTED = "WiFi Connected";

 // The rest of the constants...

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

// The rest of the functions...
```