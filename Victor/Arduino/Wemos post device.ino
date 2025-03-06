#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "DeKey-Fraijlemaborg";
const char* password = "i3xHp*ag";

const char* apiUrl = "http://217.117.224.118/Database/Device/insert_device.php";

WiFiClient wifiClient;

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connexion au Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté au Wi-Fi.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;

    String name = "Wemos";
    String ip_address = WiFi.localIP().toString();

    String postData = "name=" + name + "&ip_address=" + ip_address;

    http.begin(wifiClient, apiUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) 
    {
      String response = http.getString();
      Serial.println("Réponse de l'API : ");
      Serial.println(response);
    } 
    else 
    {
      Serial.print("Erreur requête HTTP : ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  }
  else
  {
    Serial.println("Déconnecté du WiFi, tentative de reconnexion...");
  }

  delay(60000);
}
