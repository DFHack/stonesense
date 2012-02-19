#pragma once
#include "common.h"
#include "ConditionalSprite.h"


class BuildingConfiguration
{
public:
  int32_t game_type;
  int32_t game_subtype;
  int32_t game_custom;
  std::string str_custom;
  uint32_t width, height;
  string name;
  bool canBeFloating;
  bool canBeAnySize;
  SpriteNode* sprites;

  BuildingConfiguration(string name, int game_type, int game_subtype, std::string game_custom);
  BuildingConfiguration();
  ~BuildingConfiguration(void);
};
