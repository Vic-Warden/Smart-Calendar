#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "DeKey-Fraijlemaborg";
const char* password = "i3xHp*ag";
const char* serverName = "http://100.74.252.69/Database/Device/insert_sensor.php";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connexion au Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté au Wi-Fi !");
  
  sendSensorData("Temperature Sensor", "temperature", 25.5, 1);
}

void loop() 
{

}

void sendSensorData(String name, String type, float threshold, int device_id) 
{
  if(WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "name=" + name +
                      "&type=" + type +
                      "&activation_threshold=" + String(threshold) +
                      "&device_id=" + String(device_id);

    int httpResponseCode = http.POST(postData);

    Serial.print("Code réponse HTTP : ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) 
    {
      String response = http.getString();
      Serial.println("Réponse serveur :");
      Serial.println(response);
    } 
    
    else 
    {
      Serial.print("Erreur d'envoi POST : ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } 
  
  else 
  {
    Serial.println("WiFi non connecté !");
  }
}
