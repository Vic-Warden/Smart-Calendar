#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define BOUTON_PIN 16 
#define PIR_SENSOR_PIN D2 
#define LDR_PIN A0

const char* ssid = "DeKey-Fraijlemaborg";
const char* password = "i3xHp*ag";

const char* apiUrl = "http://100.74.252.69/Database/SensorData/insert_sensor_data.php";

WiFiClient wifiClient;

int lastButtonState = HIGH;
int lastPIRState = LOW;
int lastLDRState = -1;
unsigned long lastPIRTime = 0;
bool pirLocked = false;
const long pirInterval = 300000;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connexion au Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnecté au Wi-Fi.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    pinMode(BOUTON_PIN, INPUT_PULLUP);
    pinMode(PIR_SENSOR_PIN, INPUT);
}

void sendSensorData(int sensor1, float value1, int sensor2, float value2, int sensor3, float value3) 
{
    if (WiFi.status() == WL_CONNECTED) 
    {
        HTTPClient http;

        String postData = "sensor_id_1=" + String(sensor1) + "&value_1=" + String(value1) +
                          "&sensor_id_2=" + String(sensor2) + "&value_2=" + String(value2) +
                          "&sensor_id_3=" + String(sensor3) + "&value_3=" + String(value3);

        Serial.println("📡 Envoi des données : " + postData);

        http.begin(wifiClient, apiUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) 
        {
            String response = http.getString();
            Serial.println("📡 Réponse du serveur : ");
            Serial.println(response);
        } 
        
        else 
        {
            Serial.print("❌ Erreur HTTP : ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } 
    
    else 
    {
        Serial.println("❌ WiFi déconnecté, tentative de reconnexion...");
    }
}

void loop() 
{
    int buttonState = digitalRead(BOUTON_PIN);
    int pirState = digitalRead(PIR_SENSOR_PIN);
    int ldrValue = analogRead(LDR_PIN);

    if (buttonState == LOW && lastButtonState == HIGH) 
    {
        Serial.println("🔘 Bouton pressé ! Envoi des données...");
        sendSensorData(1, 1, 2, pirState, 3, ldrValue);
    }
    lastButtonState = buttonState;

    if (pirState == HIGH && lastPIRState == LOW && !pirLocked) 
    {
        Serial.println("🚨 Mouvement détecté ! Envoi immédiat...");
        sendSensorData(1, buttonState == LOW ? 1 : 0, 2, pirState, 3, ldrValue);
        lastPIRTime = millis(); 
        pirLocked = true;  
    }

    if (pirLocked && millis() - lastPIRTime >= pirInterval) 
    {
        Serial.println("🔓 PIR débloqué, prêt pour une nouvelle détection !");
        pirLocked = false;
    }

    lastPIRState = pirState;

    int currentLDRState = (ldrValue < 200) ? 0 : 1;

    if (currentLDRState != lastLDRState) 
    {
        Serial.print("🔆 Changement de luminosité détecté ! Nouvelle valeur : ");
        Serial.println(ldrValue);
        sendSensorData(1, buttonState == LOW ? 1 : 0, 2, pirState, 3, ldrValue);
        lastLDRState = currentLDRState;
    }

    delay(50);
}
