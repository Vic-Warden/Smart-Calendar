#define PIR_SENSOR_PIN D2  

void setup() 
{
  Serial.begin(115200); 
  pinMode(PIR_SENSOR_PIN, INPUT);
}

void loop() 
{
  int etatPIR = digitalRead(PIR_SENSOR_PIN); 

  if (etatPIR == HIGH) 
  {
    Serial.println("🔴 Mouvement détecté !");
  } 
  
  else 
  {
    Serial.println("⚪ Pas de mouvement...");
  }

  delay(1000);
}
