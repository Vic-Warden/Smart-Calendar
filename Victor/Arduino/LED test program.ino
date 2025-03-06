#define LED_PIN 5 // define the LED pin

void setup() {
  pinMode(LED_PIN, OUTPUT); // set the LED pin as an output
}

void loop() {
  digitalWrite(LED_PIN, HIGH); // turn the LED on
  delay(1000); // wait for 1 second
  digitalWrite(LED_PIN, LOW); // turn the LED off
  delay(1000); // wait for 1 second
}
