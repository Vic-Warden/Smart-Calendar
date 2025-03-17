#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() 
{
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17); 
    
    if (!myDFPlayer.begin(mySerial)) 
    {
        Serial.println("Échec de la connexion au DFPlayer Mini !");
        while (true);
    }
    
    Serial.println("DFPlayer Mini prêt");
    myDFPlayer.volume(20); 
    myDFPlayer.play(2); 
}

void loop() 
{

}
