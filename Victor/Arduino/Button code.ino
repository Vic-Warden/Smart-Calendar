#define BUTTON_PIN D3
#define LED_PIN D4

void setup() 
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);  
}

void loop() 
{
    int buttonState = digitalRead(BUTTON_PIN);
    
    if (buttonState == LOW) 
    { 
        digitalWrite(LED_PIN, HIGH);
    } 
    
    else 
    {
        digitalWrite(LED_PIN, LOW); 
    }
}
