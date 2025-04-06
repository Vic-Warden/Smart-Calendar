#include "DisplayManager.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
TM1637Display display(CLK_PIN, DIO_PIN);
DisplayState currentDisplayState = SHOWING_INITIALIZATION;

static String lastLine1 = "";
static String lastLine2 = "";
static DisplayState lastState = SHOWING_INITIALIZATION;

void setupDisplay() 
{
  lcd.init();
  lcd.backlight();
  updateDisplay("Initializing...", "", SHOWING_INITIALIZATION);
}

void setup7Segment() 
{
  display.setBrightness(0x0a);
  display.clear();
  display.showNumberDec(8888);
}

void updateDisplay(String line1, String line2, DisplayState newState) 
{
  if (newState != lastState || line1 != lastLine1 || line2 != lastLine2) 
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));

    lastLine1 = line1;
    lastLine2 = line2;
    lastState = newState;
    currentDisplayState = newState;
  }
}
