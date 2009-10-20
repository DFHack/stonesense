#pragma once
#include "common.h"
#include "ConditionalSprite.h"


class BuildingConfiguration
{
public:
  uint32_t gameID;
  uint32_t width, height;
  //uint32_t spriteIndex;
  string name;
  bool canBeFloating;
  bool canBeAnySize;
  vector<ConditionalSprite> sprites;

  BuildingConfiguration(string name, int ID);
  ~BuildingConfiguration(void);



};
