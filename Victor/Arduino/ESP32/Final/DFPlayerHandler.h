#ifndef DFPLAYER_HANDLER_H
#define DFPLAYER_HANDLER_H

#include "Config.h"

extern HardwareSerial mySoftwareSerial;
extern DFRobotDFPlayerMini dfPlayer;
extern bool dfPlayerInitialized;
extern CharacterMode currentCharacter;
extern String currentCharacterName;

void setupDFPlayer();
void playRandomSound(const int tracks[], int count);
void playAppropriateSound(bool isActiveOrUpdated);

#endif
