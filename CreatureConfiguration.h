#pragma once


#define CREATURESTRLENGTH 50
class CreatureConfiguration
{
public:
  char gameIDstr[CREATURESTRLENGTH];
  int gameID;
  char professionstr[CREATURESTRLENGTH];
  int professionID;
  int sheetIndex;
  enumCreatureSex sex;

  CreatureConfiguration(char* gameIDstr, char* professionStr, enumCreatureSex sex, int sheetIndex);
  ~CreatureConfiguration(void);
};





void TranslateCreatureNames();

void LoadCreatureConfiguration( vector<CreatureConfiguration>* knownCreatures );

extern bool CreatureNamesTranslatedFromGame;
