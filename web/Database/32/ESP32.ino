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

#include <vector>
std::vector<String> knownAppointments;


LiquidCrystal_I2C lcd(0x27, 16, 2);

HardwareSerial mySoftwareSerial(2); 
DFRobotDFPlayerMini dfPlayer;
bool dfPlayerInitialized = false;

const char* apiUrl = "http://100.74.252.69/Database/Appointment/recover_appointment.php";

WiFiClient wifiClient;
DynamicJsonDocument doc(1024);
JsonArray appointments;
int currentAppointment = 0;
unsigned long lastWifiCheck = 0;
unsigned long lastCheckTime = 0;

void checkWifiConnection();
void fetchAppointments();
void waitForNewAppointments();
void checkAppointmentsForActions(JsonArray newAppointments);
void displayNextAppointment();

void checkWifiConnection() 
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        Serial.println("❌ Wi-Fi déconnecté, tentative de reconnexion...");
        WiFi.begin();
        delay(5000);
        
        if (WiFi.status() == WL_CONNECTED) 
        {
            Serial.println("✅ Wi-Fi reconnecté !");
        } 
        
        else 
        {
            Serial.println("⚠️ Impossible de se reconnecter !");
        }
    }
}

void fetchAppointments() 
{
    Serial.println("📡 Vérification des rendez-vous...");

    if (WiFi.status() != WL_CONNECTED) 
    {
        checkWifiConnection();
    }

    HTTPClient http;
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) 
    {
        String response = http.getString();
        DynamicJsonDocument newDoc(1024);
        DeserializationError error = deserializeJson(newDoc, response);

        if (!error) 
        {
            JsonArray newAppointments = newDoc.as<JsonArray>();
            checkAppointmentsForActions(newAppointments);

        } 
        
        else 
        {
            Serial.println("❌ Erreur de parsing JSON !");
        }
    } 
    
    else 
    {
        Serial.println("❌ Erreur de requête API !");
    }

    http.end();
}

void displayNextAppointment() 
{
    display.clearDisplay();
    
    if (appointments.size() == 0) 
    {
        display.setCursor(0, 10);
        display.print("Aucun RDV");
    } 
    
    else 
    {
        JsonObject app = appointments[currentAppointment];
        String task = app["task"].as<String>();
        
        display.setCursor(0, 10);
        display.print("RDV:");
        display.setCursor(0, 30);
        display.print(task);
    }
    display.display();
}

void setup() 
{
    Serial.begin(115200);
    Serial.println("🔄 Démarrage du programme...");
    delay(1000);

    mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);
    Serial.println("✅ Initialisation du port série DFPlayer Mini...");
    delay(1000);

    if (dfPlayer.begin(mySoftwareSerial)) 
    {
        dfPlayer.volume(10);
        dfPlayerInitialized = true;
        Serial.println("✅ DFPlayer Mini initialisé !");
    } 

    else 
    {
        Serial.println("❌ DFPlayer Mini non détecté !");
    }

    delay(1000);

    WiFiManager wifiManager;
    Serial.println("📡 Connexion au Wi-Fi...");
    delay(1000);

    if (!wifiManager.autoConnect("ESP32-Setup")) 
    {
        Serial.println("❌ Connexion Wi-Fi échouée. Redémarrage...");
        ESP.restart();
    }

    Serial.println("✅ Connexion Wi-Fi réussie !");
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Wi-Fi Connecté!");
    delay(2000);

    lastCheckTime = millis();
}

void loop() 
{
    checkWifiConnection();

    if (millis() - lastCheckTime >= 5000) 
    {
        Serial.println("⏳ En attente d'un nouvel événement...");
        fetchAppointments();
        lastCheckTime = millis();
    }

    delay(1000);
}



void waitForNewAppointments() 
{
    if (WiFi.status() != WL_CONNECTED) 
    {
        checkWifiConnection();
    }

    HTTPClient http;
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) 
    {
        String response = http.getString();
        DynamicJsonDocument newDoc(1024);
        DeserializationError error = deserializeJson(newDoc, response);
        if (!error) 
        {
            JsonArray newAppointments = newDoc.as<JsonArray>();
            checkAppointmentsForActions(newAppointments);
            doc = newDoc;
            appointments = newAppointments;
            currentAppointment = 0;
            displayNextAppointment();
        }
    }
    http.end();
}


void checkAppointmentsForActions(JsonArray newAppointments) {
    Serial.println("🔎 Vérification des rendez-vous...");

    std::vector<String> newKnownAppointments;

    if (newAppointments.size() == 0 && !knownAppointments.empty()) 
    {
        Serial.println("❌ TOUS les rendez-vous ont été supprimés !");
        playRandomTrackDeleted();
        delay(1000);
        knownAppointments.clear();
        return;
    }

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
                    Serial.println("📢 Mise à jour détectée !");
                    playRandomTrackUpdated();
                    delay(1000);
                }

                break;
            }
        }

        if (!found) 
        {
            Serial.println("🆕 Nouveau rendez-vous détecté !");
            playRandomTrackActive();
            delay(1000);
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
            Serial.println("❌ Rendez-vous supprimé !");
            playRandomTrackDeleted();
            delay(1000);
        }
    }

    knownAppointments = newKnownAppointments;
    doc.clear();
    doc.set(newAppointments);
}

void playRandomTrackActive() 
{
    Serial.println("🎵 Lecture d'un son pour 'active'...");
    delay(500);

    int tracks[] = {1, 2, 3, 6, 7, 8, 9, 10, 31, 33, 34, 35};

    int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
    Serial.print("🎵 Lecture de la piste : ");
    Serial.println(trackNumber);
    dfPlayer.play(trackNumber);
    delay(200);
}

void playRandomTrackUpdated() 
{
    Serial.println("🎵 Lecture d'un son pour 'updated'...");
    delay(500);

    int tracks[] = {4, 5, 12, 17, 20, 28, 29, 30};

    int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
    Serial.print("🎵 Lecture de la piste : ");
    Serial.println(trackNumber);
    dfPlayer.play(trackNumber);
    delay(200);
}

void playRandomTrackDeleted() 
{
    Serial.println("🎵 Lecture d'un son pour 'deleted'...");
    delay(500);
    int tracks[] = {24, 25, 26, 27, 23, 13, 14, 32};
    int trackNumber = tracks[random(0, sizeof(tracks) / sizeof(tracks[0]))];
    Serial.print("🎵 Lecture de la piste : ");
    Serial.println(trackNumber);
    dfPlayer.play(trackNumber);
    delay(200);
}
