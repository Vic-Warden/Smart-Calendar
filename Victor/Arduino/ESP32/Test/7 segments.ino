#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>

#define CLK 18
#define DIO 19

const char* ssid = "DeKey-Fraijlemaborg";
const char* password = "i3xHp*ag";   

TM1637Display display(CLK, DIO);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);

void setup() 
{
    Serial.begin(115200);
    
    Serial.print("Connexion au Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnecté au Wi-Fi !");
    
    timeClient.begin();
    display.setBrightness(7);
}

void loop() 
{
    timeClient.update();
    
    int heure = timeClient.getHours();
    int minute = timeClient.getMinutes();
    int affichage = heure * 100 + minute;

    display.showNumberDecEx(affichage, 0b01000000, true);
    
    Serial.print("Heure actuelle : ");
    Serial.print(heure);
    Serial.print(":");
    Serial.println(minute);
    
    delay(1000);
}
