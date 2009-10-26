#include "common.h"
#include "CreatureConfiguration.h"
#include "Creatures.h"

bool CreatureNamesTranslatedFromGame = false;


CreatureConfiguration::CreatureConfiguration(char* gameIDstr, int sheetIndex)
{
  memset(this, 0, sizeof(CreatureConfiguration) );
  this->sheetIndex = sheetIndex;
  this->gameID = INVALID_INDEX;

  int len = (int) strlen(gameIDstr);
  if(len > 100) len = 100;
  memcpy(this->gameIDstr, gameIDstr, len);
}

CreatureConfiguration::~CreatureConfiguration(void)
{
}

void DumpCreatureNamesToDisk(){
  FILE* fp = fopen("dump.txt", "w");
  if(!fp) return;
  for(uint32_t j=0; j < v_creatureNames.size(); j++){
    fprintf(fp, "%s\n", v_creatureNames[j].id);
  }
  fclose(fp);
}

void TranslateCreatureNames(){
  //for each config, find it's integer ID
  for(uint32_t i=0; i < creatureTypes.size(); i++){
    char* ptr = creatureTypes[i].gameIDstr;
    uint32_t j;
    for(j=0; j < v_creatureNames.size(); j++){
      if( strcmp( ptr, v_creatureNames[j].id) == 0){
        //assign ID
        creatureTypes[i].gameID = j; 
        //jump to next creatureType
        break;
      }
    }
    if(j >= v_creatureNames.size())
      WriteErr("Unable to match creature '%s' to anything in-game\n", ptr);
  }

  CreatureNamesTranslatedFromGame = true;
}