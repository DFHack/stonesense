#include "SpriteMaps.h"
#include "BuildingConfiguration.h"
#include "GameBuildings.h"

bool BuildingNamesTranslatedFromGame = false;

BuildingConfiguration::BuildingConfiguration(string name, char* IDstring)
{
  memset(this, 0, sizeof(BuildingConfiguration));
  
  this->name = name;
  this->gameID = -1;
  this->width = this->height = 1;
  this->canBeFloating = false;
  this->canBeAnySize = false;

  int len = (int) strlen(IDstring);
  if(len > 100) len = 100;
  memcpy(this->gameIDstr, IDstring, len);
}

BuildingConfiguration::~BuildingConfiguration(void)
{
}



void TranslateBuildingNames(){
  //for each config, find it's integer ID
  for(uint32_t i=0; i < buildingTypes.size(); i++){
    char* ptr = buildingTypes[i].gameIDstr;
    uint32_t j;
    for(j=0; j < v_buildingtypes.size(); j++){
      if( strcmpi( ptr, v_buildingtypes[j].c_str()) == 0){
        //assign ID
        buildingTypes[i].gameID = j; 
        //jump to next buildingType
        break;
      }
    }
    if(j >= v_buildingtypes.size())
      WriteErr("Unable to match building '%s' to anything in-game\n", ptr);
  }

  BuildingNamesTranslatedFromGame = true;
}