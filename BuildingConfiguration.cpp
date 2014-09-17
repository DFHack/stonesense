#include "SpriteMaps.h"
#include "BuildingConfiguration.h"
#include "GameBuildings.h"
#include "ContentLoader.h"

bool BuildingNamesTranslatedFromGame = false;

BuildingConfiguration::BuildingConfiguration(string name, int game_type, int game_subtype, int32_t custom)
{
    this->name = name;
    this->game_type = game_type;
    this->game_subtype = game_subtype;
    this->game_custom = custom;
    this->width = this->height = 1;
    this->canBeFloating = false;
    this->canBeAnySize = false;
    this->sprites = NULL;
}
BuildingConfiguration::BuildingConfiguration()
{
    this->name = "";
    this->game_type = -1;
    this->game_subtype = -1;
    this->game_custom = -1;
    this->width = this->height = 1;
    this->canBeFloating = false;
    this->canBeAnySize = false;
    this->sprites = NULL;
}
BuildingConfiguration::~BuildingConfiguration(void)
{
    //cant delete bc.sprites here- screws up BCs copy semantics
}


void DumpBuildingNamesToDisk()
{
    /*
    FILE* fp = fopen("buildingdump.txt", "w");
    if(!fp) return;
    for(uint32_t j=0; j < contentLoader->classIdStrings.size(); j++){
    fprintf(fp, "%i:%s\n",j, contentLoader->classIdStrings[j].c_str());
    }
    fclose(fp);
    */
}
