#include <Servo.h>

Servo monServomoteur;

int brocheServo = 4;

void setup() 
{
  monServomoteur.attach(brocheServo);
  
  monServomoteur.write(0); 
  delay(1000);

  monServomoteur.write(90); 
  delay(1000); 

  monServomoteur.write(180); 
  delay(1000); 
}

void loop() 
{

}
