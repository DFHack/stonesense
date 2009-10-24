#pragma once

class CreatureConfiguration
{
public:
  char gameIDstr[100];
  int gameID;
  int sheetIndex;

  CreatureConfiguration(char* gameIDstr, int sheetIndex);
  ~CreatureConfiguration(void);
};




extern bool CreatureNamesTranslatedFromGame;

void TranslateCreatureNames();