const int photoResistorPin = 34;

void setup() 
{
    Serial.begin(115200);
}

void loop() 
{
    int lightLevel = analogRead(photoResistorPin);
    float lightPercentage = (lightLevel / 4095.0) * 100;

    Serial.print("Niveau de lumière: ");
    Serial.print(lightPercentage);
    Serial.println("%");

    delay(500);
}
