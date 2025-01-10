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
    std::unique_ptr<SpriteNode> sprites;

    BuildingConfiguration(std::string name, int game_type, int game_subtype, int32_t custom) :
        game_type{ game_type },
        game_subtype{ game_subtype },
        game_custom{ custom },
        width{ 1 },
        height{ 1 },
        name{ name },
        canBeFloating{ false },
        canBeAnySize{ false },
        sprites{ nullptr }
    { }
    BuildingConfiguration() :
        BuildingConfiguration{ "", -1, -1, -1 }
    { }
    ~BuildingConfiguration()
    {
        //cant delete bc.sprites here- screws up BCs copy semantics
    }
};
