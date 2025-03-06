#define BOUTON_PIN 16  // GPIO 16

void setup() 
{
  Serial.begin(115200);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  Serial.println("Test du bouton - Appuie pour voir le changement !");
}

void loop() 
{
  int etatBouton = digitalRead(BOUTON_PIN);

  if (etatBouton == LOW) 
  {
    Serial.println("Not pressed");
  } 
  
  else 
  {
    Serial.println("Pressed");
  }

  delay(100);
}
