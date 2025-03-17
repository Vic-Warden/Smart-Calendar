#include <ESP32Servo.h>

const int servoPin = 27;
Servo monServo;

void setup() {
    Serial.begin(115200);
    monServo.attach(servoPin, 500, 2500);
}

void loop() {
    for (int angle = 0; angle <= 180; angle += 5) 
    {
        monServo.write(angle);
        Serial.print("Angle: ");
        Serial.println(angle);
        delay(20);
    }

    for (int angle = 180; angle >= 0; angle -= 5) 
    {
        monServo.write(angle);
        Serial.print("Angle: ");
        Serial.println(angle);
        delay(20);
    }
}
