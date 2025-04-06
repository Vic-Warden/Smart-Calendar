#include "DFPlayerHandler.h"

HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini dfPlayer;
bool dfPlayerInitialized = false;
CharacterMode currentCharacter = COMMISSAR;
String currentCharacterName = "Commissar";

void setupDFPlayer() 
{
  if (dfPlayer.begin(mySoftwareSerial)) 
  {
    dfPlayer.volume(17);
    dfPlayerInitialized = true;
    Serial.println("DFPlayer initialized");
  } 
  
  else 
  {
    Serial.println("DFPlayer initialization failed!");
  }
}

void playRandomSound(const int tracks[], int count) 
{
  if (!dfPlayerInitialized || count <= 0 || nightMode) return;

  static unsigned long lastPlayTime = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastPlayTime > 100) 
  {
    int randomIndex = random(0, count);
    dfPlayer.play(tracks[randomIndex]);
    lastPlayTime = currentMillis;
  }
}

void playAppropriateSound(bool isActiveOrUpdated) 
{
  if (!dfPlayerInitialized || nightMode) return;

  switch (currentCharacter) 
  {
    case COMMISSAR:
      playRandomSound(isActiveOrUpdated ? COMMISSAR_ACTIVE_TRACKS : COMMISSAR_DELETED_TRACKS,
                      isActiveOrUpdated ? sizeof(COMMISSAR_ACTIVE_TRACKS) / sizeof(int) :
                                          sizeof(COMMISSAR_DELETED_TRACKS) / sizeof(int));
      break;

    case LEMAN_RUSS:
      playRandomSound(isActiveOrUpdated ? LEMAN_RUSS_ACTIVE_TRACKS : LEMAN_RUSS_DELETED_TRACKS,
                      isActiveOrUpdated ? sizeof(LEMAN_RUSS_ACTIVE_TRACKS) / sizeof(int) :
                                          sizeof(LEMAN_RUSS_DELETED_TRACKS) / sizeof(int));
      break;

    case BANEBLADE:
      playRandomSound(isActiveOrUpdated ? BANEBLADE_ACTIVE_TRACKS : BANEBLADE_DELETED_TRACKS,
                      isActiveOrUpdated ? sizeof(BANEBLADE_ACTIVE_TRACKS) / sizeof(int) :
                                          sizeof(BANEBLADE_DELETED_TRACKS) / sizeof(int));
      break;
  }
}
