void setup() {
    Serial.begin(115200);
    pinMode(A0, INPUT);
  }
  
  void loop() {
    int valeur = analogRead(A0);
    Serial.print("Valeur A0 : ");
    Serial.println(valeur);
    delay(500);
  }
  