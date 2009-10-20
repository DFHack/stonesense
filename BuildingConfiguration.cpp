#include "SpriteMaps.h"
#include "BuildingConfiguration.h"

BuildingConfiguration::BuildingConfiguration(string name, int ID)
{
  this->name = name;
  this->gameID = ID;
  this->width = this->height = 1;
  this->canBeFloating = false;
  this->canBeAnySize = false;
}

BuildingConfiguration::~BuildingConfiguration(void)
{
}
