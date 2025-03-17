#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

const int boutonPin = 14;
bool boutonAppuye = false;

HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    pinMode(boutonPin, INPUT_PULLUP);
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    
    if (!myDFPlayer.begin(mySerial)) {
        Serial.println("Échec de la connexion au DFPlayer Mini !");
        while (true);
    }
    
    Serial.println("DFPlayer Mini prêt");
    myDFPlayer.volume(20);
    randomSeed(analogRead(0)); 
}

void loop() {
    if (digitalRead(boutonPin) == LOW) {
        boutonAppuye = true;
    }

    if (boutonAppuye && digitalRead(boutonPin) == HIGH) {
        Serial.println("Clic détecté ! Lecture d'une musique aléatoire...");
        int trackNumber = random(1, 37); 
        myDFPlayer.play(trackNumber);
        boutonAppuye = false;
        delay(200);
    }

    delay(10);
}
