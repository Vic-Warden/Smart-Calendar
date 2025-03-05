#include <ESP8266WiFi.h>

const char* ssid = "iotroam";
const char* password = "uBZTpJnK5M";

void setup() 
{  
  Serial.begin(115200);
  delay(2000);

  Serial.println("Tentative de connexion au Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connecté au Wi-Fi!");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  // Code principal ici
}
