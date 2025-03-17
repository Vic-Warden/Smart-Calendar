const int boutonPin = 14;
bool boutonAppuye = false;

void setup() 
{
    pinMode(boutonPin, INPUT_PULLUP);
    Serial.begin(115200);
}

void loop() 
{
    if (digitalRead(boutonPin) == LOW) 
    {
        boutonAppuye = true;
    }

    if (boutonAppuye && digitalRead(boutonPin) == HIGH) 
    {
        Serial.println("Clic détecté !");
        boutonAppuye = false;
        delay(200);
    }

    delay(10);
}
