#define PIR_PIN 14 
#define LED_PIN 4  

void setup() 
{
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("PIR prêt !");
}

void loop() 
{
  int detection = digitalRead(PIR_PIN);
    detection = !detection;

  if (detection == HIGH) 
  {  
    Serial.println("🚨 Mouvement détecté ! Allumage LED.");
    digitalWrite(LED_PIN, HIGH);  
  } 
  
  else 
  {  
    Serial.println("Aucun mouvement...");
    digitalWrite(LED_PIN, LOW);  
  }

  delay(500);
}
