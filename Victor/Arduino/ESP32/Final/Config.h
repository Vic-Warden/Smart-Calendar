#ifndef CONFIG_H
#define CONFIG_H

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

// LCD
#define LCD_SDA 21
#define LCD_SCL 22

// 7-segment
#define CLK_PIN 18
#define DIO_PIN 19

// Servo
#define SERVO_PIN 27
#define SERVO_SPEED 1
#define SERVO_UPDATE_INTERVAL 20
#define HOURLY_SERVO_DURATION 60000

// API
#define APPOINTMENT_API_URL "http://145.92.189.59/Database/Appointment/recover_appointment.php"
#define SENSOR_API_URL "http://145.92.189.59/Database/SensorData/insert_sensor_data.php"

// Device
#define DEVICE_ID 1
#define BUTTON_SENSOR_ID 1
#define PIR_SENSOR_ID 2
#define LDR_SENSOR_ID 3

// Sensors
#define BUTTON_PIN 14
#define PIR_PIN 35
#define LDR_PIN 34

// Thresholds
#define NIGHT_MODE_ENTER_THRESHOLD 650
#define NIGHT_MODE_EXIT_THRESHOLD 750

// Timing
#define APPOINTMENT_REFRESH_INTERVAL 5000
#define DISPLAY_INTERVAL 3000
#define SENSOR_CHECK_INTERVAL 50
#define TIME_UPDATE_INTERVAL 60000
#define NTP_RETRY_INTERVAL 30000
#define BUTTON_DEBOUNCE_TIME 50
#define WIFI_RECONNECT_DELAY 1000
#define CHARACTER_NOTIFICATION_DURATION 2000
#define PIR_INTERVAL 300000

// Sound Tracks
#define COMMISSAR_TRACK 31
#define LEMAN_RUSS_TRACK 42
#define BANEBLADE_TRACK 53
#define DARK_TRACK 35
#define LIGHT_TRACK 36
const int PIR_TRACKS[] = {37, 38, 39, 40, 41};

const int COMMISSAR_ACTIVE_TRACKS[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 28, 33, 34};
const int COMMISSAR_DELETED_TRACKS[] = {11, 20, 23, 24, 25, 26, 27, 29, 30, 32};
const int LEMAN_RUSS_ACTIVE_TRACKS[] = {43, 44, 45, 46, 47, 48};
const int LEMAN_RUSS_DELETED_TRACKS[] = {49, 50, 51};
const int BANEBLADE_ACTIVE_TRACKS[] = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 67, 68, 69, 70, 71, 72, 73, 74};
const int BANEBLADE_DELETED_TRACKS[] = {64, 65, 66};

// Enums
enum CharacterMode { COMMISSAR, LEMAN_RUSS, BANEBLADE };
enum DisplayState { SHOWING_CHARACTER_NOTIFICATION, SHOWING_APPOINTMENTS, SHOWING_NO_APPOINTMENTS, SHOWING_INITIALIZATION };

#endif
