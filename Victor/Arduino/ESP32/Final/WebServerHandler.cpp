#include "WebServerHandler.h"
#include "DisplayManager.h"
#include "DFPlayerHandler.h"

WebServer server(80);

extern bool showingCharacterNotification;
extern unsigned long characterNotificationStart;

void setupWebServer() 
{
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "Smart Calendar is online!");
  });

  server.on("/switch_character", HTTP_GET, []() {
    switch (currentCharacter) 
    {
      case COMMISSAR:
        currentCharacter = LEMAN_RUSS;
        currentCharacterName = "Leman Russ";
        break;
      case LEMAN_RUSS:
        currentCharacter = BANEBLADE;
        currentCharacterName = "Baneblade";
        break;
      case BANEBLADE:
        currentCharacter = COMMISSAR;
        currentCharacterName = "Commissar";
        break;
    }

    updateDisplay("Switched to", currentCharacterName, SHOWING_CHARACTER_NOTIFICATION);

    if (dfPlayerInitialized && !nightMode) 
    {
      switch (currentCharacter) 
      {
        case COMMISSAR: dfPlayer.play(COMMISSAR_TRACK); break;
        case LEMAN_RUSS: dfPlayer.play(LEMAN_RUSS_TRACK); break;
        case BANEBLADE: dfPlayer.play(BANEBLADE_TRACK); break;
      }
    }

    showingCharacterNotification = true;
    characterNotificationStart = millis();

    server.send(200, "text/plain", "Character changed to " + currentCharacterName);
  });

  server.on("/play_random", HTTP_GET, []() 
  {
    if (dfPlayerInitialized && !nightMode) 
    {
      switch (currentCharacter) 
      {
        case COMMISSAR:
          playRandomSound(COMMISSAR_ACTIVE_TRACKS, sizeof(COMMISSAR_ACTIVE_TRACKS) / sizeof(int));
          break;
        case LEMAN_RUSS:
          playRandomSound(LEMAN_RUSS_ACTIVE_TRACKS, sizeof(LEMAN_RUSS_ACTIVE_TRACKS) / sizeof(int));
          break;
        case BANEBLADE:
          playRandomSound(BANEBLADE_ACTIVE_TRACKS, sizeof(BANEBLADE_ACTIVE_TRACKS) / sizeof(int));
          break;
      }
      server.send(200, "text/plain", "Random voice line played.");
    } 
    
    else 
    {
      server.send(503, "text/plain", "Night mode active or DFPlayer unavailable.");
    }
  });

  server.begin();
  Serial.println("HTTP server started!");
}
