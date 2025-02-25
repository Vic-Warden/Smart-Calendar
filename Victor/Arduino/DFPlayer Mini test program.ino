#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mySerial(10, 11); 
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    
    Serial.println("Initialisation du DFPlayer Mini...");
    
    if (!myDFPlayer.begin(mySerial)) {  
        Serial.println("Erreur : DFPlayer Mini non détecté !");
        while (true);
    }

    Serial.println("DFPlayer Mini prêt !");
    myDFPlayer.volume(20);

    Serial.println("Lecture du fichier 0001.mp3...");
    myDFPlayer.play(4);
}

void loop() {

}
