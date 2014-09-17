#pragma once
#include "common.h"
#include "ConditionalSprite.h"


class BuildingConfiguration
{
public:
    int32_t game_type;
    int32_t game_subtype;
    int32_t game_custom;
    uint32_t width, height;
    std::string name;
    bool canBeFloating;
    bool canBeAnySize;
    SpriteNode* sprites;

    BuildingConfiguration(std::string name, int game_type, int game_subtype, int32_t game_custom);
    BuildingConfiguration();
    ~BuildingConfiguration(void);
};
