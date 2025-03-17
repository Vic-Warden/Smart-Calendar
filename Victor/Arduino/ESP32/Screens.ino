#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>

#define SCREEN_WIDTH 128   
#define SCREEN_HEIGHT 64   
#define OLED_RESET    -1   
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ECG_CYCLE_WIDTH 50  
#define ECG_TOTAL_WIDTH (ECG_CYCLE_WIDTH * 2 + 30)  

int ecgWave[ECG_TOTAL_WIDTH];  
int offset = 0;  

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(115200);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
    {
        Serial.println(F("Échec de l'initialisation de l'écran OLED"));
        while (1);
    }
    display.clearDisplay();
    generateECGWave();
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Hello world");
}

void loop() 
{
    display.clearDisplay();
    drawECG();
    scrollECG();
    display.display();
    delay(30);
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