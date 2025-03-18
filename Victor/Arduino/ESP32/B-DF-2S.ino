#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>

// Définition du bouton
const int boutonPin = 14;
bool boutonAppuye = false;

// Initialisation du DFPlayer Mini
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

// Définition de l'écran OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ECG parameters
#define ECG_CYCLE_WIDTH 50
#define ECG_TOTAL_WIDTH (ECG_CYCLE_WIDTH * 2 + 30)
int ecgWave[ECG_TOTAL_WIDTH];
int offset = 0;

// Écran LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() 
{
    Serial.begin(115200);
    pinMode(boutonPin, INPUT_PULLUP);

    // Initialisation du DFPlayer Mini
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    if (!myDFPlayer.begin(mySerial)) 
    {
        Serial.println("Échec de la connexion au DFPlayer Mini !");
        while (true);
    }
    myDFPlayer.volume(10);
    randomSeed(analogRead(0));
    Serial.println("DFPlayer Mini prêt");

    // Initialisation de l'écran OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    {
        Serial.println(F("Échec de l'initialisation de l'écran OLED"));
        while (1);
    }
    display.clearDisplay();
    generateECGWave();

    // Initialisation de l'écran LCD I2C
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Hello world");
}

void loop() {
    gestionBouton();
    miseAJourEcranOLED();
    delay(10); 
}

void gestionBouton() 
{
    if (digitalRead(boutonPin) == LOW) 
    {
        boutonAppuye = true;
    }

    if (boutonAppuye && digitalRead(boutonPin) == HIGH) 
    {
        Serial.println("Clic détecté ! Lecture d'une musique aléatoire...");
        int trackNumber = random(1, 37);
        myDFPlayer.play(trackNumber);
        boutonAppuye = false;
        delay(200);
    }
}

void miseAJourEcranOLED() 
{
    display.clearDisplay();
    drawECG();
    scrollECG();
    display.display();
}

void generateECGWave() 
{
    int baseY = SCREEN_HEIGHT / 2;
    int x = 0;

    for (int rep = 0; rep < 2; rep++) 
    {
        for (int i = 0; i < 7; i++) ecgWave[x++] = baseY;
        for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - i;
        for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - 10 + i * 6;
        for (int i = 0; i < 6; i++) ecgWave[x++] = baseY + 30 - i * 10;
        for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - 30 + i * 7;
        for (int i = 0; i < 15; i++) ecgWave[x++] = baseY;
    }
    for (int i = x; i < ECG_TOTAL_WIDTH; i++) ecgWave[i] = baseY;
}

void drawECG() 
{
    for (int x = 1; x < SCREEN_WIDTH; x++) 
    {
        int index1 = (x + offset) % ECG_TOTAL_WIDTH;
        int index2 = (x - 1 + offset) % ECG_TOTAL_WIDTH;
        display.drawLine(x - 1, ecgWave[index2], x, ecgWave[index1], SSD1306_WHITE);
    }
}

void scrollECG() 
{
    offset = (offset + 1) % ECG_TOTAL_WIDTH;
}
