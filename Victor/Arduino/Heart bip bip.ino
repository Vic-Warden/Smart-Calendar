#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128   
#define SCREEN_HEIGHT 64   
#define OLED_RESET    -1   
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ECG_CYCLE_WIDTH 50  // Taille d'un battement
#define ECG_TOTAL_WIDTH (ECG_CYCLE_WIDTH * 2 + 30)  // Taille totale

int ecgWave[ECG_TOTAL_WIDTH];  // Tableau pour stocker les 2 battements et la droite
int offset = 0;  // Position initiale de la droite 

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  {
    Serial.println(F("Échec de l'initialisation de l'écran OLED"));
    while (1);
  }

  display.clearDisplay();
  generateECGWave();
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

  for (int rep = 0; rep < 2; rep++) { 
    // Ligne de base
    for (int i = 0; i < 7; i++) ecgWave[x++] = baseY;

    // Premier pic
    for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - i;

    // Deuxième pic
    for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - 10 + i * 6;

    // Troisième pic
    for (int i = 0; i < 6; i++) ecgWave[x++] = baseY + 30 - i * 10;

    // Quatrième pic
    for (int i = 0; i < 6; i++) ecgWave[x++] = baseY - 30 + i * 7;

    // Cinquième pic
    for (int i = 0; i < 15; i++) ecgWave[x++] = baseY;
  }

  // Retour sur la ligne 
  for (int i = x; i < ECG_TOTAL_WIDTH; i++) ecgWave[i] = baseY;
}

// Dessiner les différents pic
void drawECG() 
{
  for (int x = 1; x < SCREEN_WIDTH; x++) {
    int index1 = (x + offset) % ECG_TOTAL_WIDTH;
    int index2 = (x - 1 + offset) % ECG_TOTAL_WIDTH;

    display.drawLine(x - 1, ecgWave[index2], x, ecgWave[index1], SSD1306_WHITE);
  }
}

// Défilement
void scrollECG() 
{
  offset = (offset + 1) % ECG_TOTAL_WIDTH;
}
