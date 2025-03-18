#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

#define SCREEN_WIDTH 128   
#define SCREEN_HEIGHT 64   
#define OLED_RESET    -1   
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ECG_CYCLE_WIDTH 50  
#define ECG_TOTAL_WIDTH (ECG_CYCLE_WIDTH * 2 + 30)  

int ecgWave[ECG_TOTAL_WIDTH];  
int offset = 0;  

LiquidCrystal_I2C lcd(0x27, 16, 2);

HardwareSerial mySoftwareSerial(2); 
DFRobotDFPlayerMini dfPlayer;

const char* apiUrl = "http://100.74.252.69/Database/Appointment/recover_appointment.php";

WiFiClient wifiClient;
DynamicJsonDocument doc(1024);
JsonArray appointments;
int currentAppointment = 0;
unsigned long lastUpdate = 0;

void setup() 
{
    Serial.begin(115200);
    mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);

    if (!dfPlayer.begin(mySoftwareSerial)) 
    {
        Serial.println("❌ DFPlayer Mini non détecté !");
    } 
    
    else 
    {
        dfPlayer.volume(10);
        Serial.println("✅ DFPlayer Mini initialisé !");
    }

    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("ESP32-Setup")) 
    {
        ESP.restart();
    }

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi Connecte!");
    delay(2000);

    fetchAppointments();
}

void loop() 
{
    display.clearDisplay();
    drawECG();
    scrollECG();
    display.display();
    delay(30);

    if (millis() - lastUpdate > 60000) 
    {
        fetchAppointments();
        lastUpdate = millis();
    }

    if (millis() - lastUpdate > 5000 && appointments.size() > 0) 
    {
        displayNextAppointment();
        lastUpdate = millis();
    }
}

void fetchAppointments() 
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("⚠️ Pas de connexion Wi-Fi !");
        return;
    }

    HTTPClient http;
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) 
    {
        String response = http.getString();
        Serial.println("📩 Réponse de l'API : ");
        Serial.println(response);

        doc.clear();
        DeserializationError error = deserializeJson(doc, response);
        if (error) 
        {
            Serial.println("❌ Erreur de parsing JSON !");
            return;
        }

        appointments = doc.as<JsonArray>();
        if (appointments.isNull()) 
        {
            Serial.println("❌ JSON vide ou mal formé !");
            return;
        }

        Serial.println("✅ Appointments mis à jour !");
        currentAppointment = 0;
        checkAppointmentsForActions();
        displayNextAppointment();
    } 
    
    else 
    {
        Serial.print("❌ Erreur HTTP : ");
        Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();
}

void checkAppointmentsForActions() 
{
    for (JsonObject appointment : appointments) 
    {
        if (!appointment.containsKey("task") || !appointment.containsKey("date_hour") || !appointment.containsKey("note")) 
        {
            Serial.println("⚠️ Données JSON invalides !");
            continue;
        }

        String task = appointment["task"].as<String>();
        String note = appointment["note"].as<String>();

        Serial.print("📌 Note détectée : ");
        Serial.println(note);
        
        if (note == "active") 
        {
            playRandomTrack({1, 2, 3, 6, 7, 8, 9, 10, 31, 33, 34});
        } 
        
        else if (note == "updated") 
        {
            playRandomTrack({4, 5, 12, 14, 17, 20, 26, 28, 29, 30, 32});
        } 
        
        else if (note == "deleted") 
        {
            playRandomTrack({11, 13, 15, 16, 18, 19, 21, 22, 23, 24, 25, 27});
        }
    }
}

void playRandomTrack(std::initializer_list<int> tracks) 
{
    if (!dfPlayer.available()) {
        Serial.println("⚠️ DFPlayer Mini non prêt, tentative ignorée !");
        return;
    }

    int trackArray[tracks.size()];
    std::copy(tracks.begin(), tracks.end(), trackArray);
    int trackNumber = trackArray[random(0, tracks.size())];

    Serial.print("🎵 Lecture de la piste ");
    Serial.println(trackNumber);

    dfPlayer.play(trackNumber);
    delay(100);
}

void displayNextAppointment() {
    if (appointments.size() == 0) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Aucun RDV !");
        return;
    }

    if (currentAppointment >= appointments.size()) 
    {
        currentAppointment = 0;
    }

    JsonObject appointment = appointments[currentAppointment];

    if (!appointment.containsKey("task") || !appointment.containsKey("date_hour")) 
    {
        Serial.println("⚠️ Données RDV invalides !");
        return;
    }

    String task = appointment["task"].as<String>();
    String date_hour = appointment["date_hour"].as<String>();
    String formatted_date = date_hour.substring(8, 10) + "-" + date_hour.substring(5, 7) + " " + date_hour.substring(11, 16);

    lcd.clear();
    scrollText(0, task);
    scrollText(1, formatted_date);

    Serial.println("\n📅 RDV affiché : ");
    Serial.print("Tâche: "); Serial.println(task);
    Serial.print("Date/Heure: "); Serial.println(formatted_date);

    currentAppointment++;
    if (currentAppointment >= appointments.size()) 
    {
        currentAppointment = 0;
    }
}

void scrollText(int row, String text) 
{
    if (text.length() <= 16) 
    {
        lcd.setCursor(0, row);
        lcd.print(text);
        return;
    }

    for (int i = 0; i <= text.length() - 16; i++) 
    {
        lcd.setCursor(0, row);
        lcd.print(text.substring(i, i + 16));
        delay(300);
    }

    delay(1000);
}
