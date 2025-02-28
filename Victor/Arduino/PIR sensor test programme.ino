const int pirPin = 2;

void setup() 
{
    pinMode(pirPin, INPUT);
    Serial.begin(9600);
}

void loop() 
{
    int motionState = digitalRead(pirPin);
    
    if (motionState == HIGH) 
    {
        Serial.println("Mouvement détecté !");
    } else {
        Serial.println("Aucun mouvement");
    }
    
    delay(500);
}