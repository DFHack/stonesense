#include "EnumToString.h"

const char* TiletypeShapeToString(RemoteFortressReader::TiletypeShape input)
{
    switch (input)
    {
    case RemoteFortressReader::NO_SHAPE:
        return "NO_SHAPE";
        break;
    case RemoteFortressReader::EMPTY:
        return "EMPTY";
        break;
    case RemoteFortressReader::FLOOR:
        return "FLOOR";
        break;
    case RemoteFortressReader::BOULDER:
        return "BOULDER";
        break;
    case RemoteFortressReader::PEBBLES:
        return "PEBBLES";
        break;
    case RemoteFortressReader::WALL:
        return "WALL";
        break;
    case RemoteFortressReader::FORTIFICATION:
        return "FORTIFICATION";
        break;
    case RemoteFortressReader::STAIR_UP:
        return "STAIR_UP";
        break;
    case RemoteFortressReader::STAIR_DOWN:
        return "STAIR_DOWN";
        break;
    case RemoteFortressReader::STAIR_UPDOWN:
        return "STAIR_UPDOWN";
        break;
    case RemoteFortressReader::RAMP:
        return "RAMP";
        break;
    case RemoteFortressReader::RAMP_TOP:
        return "RAMP_TOP";
        break;
    case RemoteFortressReader::BROOK_BED:
        return "BROOK_BED";
        break;
    case RemoteFortressReader::BROOK_TOP:
        return "BROOK_TOP";
        break;
    case RemoteFortressReader::TREE_SHAPE:
        return "TREE";
        break;
    case RemoteFortressReader::SAPLING:
        return "SAPLING";
        break;
    case RemoteFortressReader::SHRUB:
        return "SHRUB";
        break;
    case RemoteFortressReader::ENDLESS_PIT:
        return "ENDLESS_PIT";
        break;
    case RemoteFortressReader::BRANCH:
        return "BRANCH";
        break;
    case RemoteFortressReader::TRUNK_BRANCH:
        return "TRUNK_BRANCH";
        break;
    case RemoteFortressReader::TWIG:
        return "TWIG";
        break;
    default:
        return "?";
        break;
    }
}

const char* TiletypeSpecialToString(RemoteFortressReader::TiletypeSpecial input)
{
    switch (input)
    {
    case RemoteFortressReader::NO_SPECIAL:
        return "NO_SPECIAL";
        break;
    case RemoteFortressReader::NORMAL:
        return "NORMAL";
        break;
    case RemoteFortressReader::RIVER_SOURCE:
        return "RIVER_SOURCE";
        break;
    case RemoteFortressReader::WATERFALL:
        return "WATERFALL";
        break;
    case RemoteFortressReader::SMOOTH:
        return "SMOOTH";
        break;
    case RemoteFortressReader::FURROWED:
        return "FURROWED";
        break;
    case RemoteFortressReader::WET:
        return "WET";
        break;
    case RemoteFortressReader::DEAD:
        return "DEAD";
        break;
    case RemoteFortressReader::WORN_1:
        return "WORN_1";
        break;
    case RemoteFortressReader::WORN_2:
        return "WORN_2";
        break;
    case RemoteFortressReader::WORN_3:
        return "WORN_3";
        break;
    case RemoteFortressReader::TRACK:
        return "TRACK";
        break;
    case RemoteFortressReader::SMOOTH_DEAD:
        return "SMOOTH_DEAD";
        break;
    default:
        return "?";
        break;
    }
}



const char* TiletypeMaterialToString(RemoteFortressReader::TiletypeMaterial input)
{
    switch (input)
    {
    case RemoteFortressReader::NO_MATERIAL:
        return "NO_MATERIAL";
        break;
    case RemoteFortressReader::AIR:
        return "AIR";
        break;
    case RemoteFortressReader::SOIL:
        return "SOIL";
        break;
    case RemoteFortressReader::STONE:
        return "STONE";
        break;
    case RemoteFortressReader::FEATURE:
        return "FEATURE";
        break;
    case RemoteFortressReader::LAVA_STONE:
        return "LAVA_STONE";
        break;
    case RemoteFortressReader::MINERAL:
        return "MINERAL";
        break;
    case RemoteFortressReader::FROZEN_LIQUID:
        return "FROZEN_LIQUID";
        break;
    case RemoteFortressReader::CONSTRUCTION:
        return "CONSTRUCTION";
        break;
    case RemoteFortressReader::GRASS_LIGHT:
        return "GRASS_LIGHT";
        break;
    case RemoteFortressReader::GRASS_DARK:
        return "GRASS_DARK";
        break;
    case RemoteFortressReader::GRASS_DRY:
        return "GRASS_DRY";
        break;
    case RemoteFortressReader::GRASS_DEAD:
        return "GRASS_DEAD";
        break;
    case RemoteFortressReader::PLANT:
        return "PLANT";
        break;
    case RemoteFortressReader::HFS:
        return "HFS";
        break;
    case RemoteFortressReader::CAMPFIRE:
        return "CAMPFIRE";
        break;
    case RemoteFortressReader::FIRE:
        return "FIRE";
        break;
    case RemoteFortressReader::ASHES:
        return "ASHES";
        break;
    case RemoteFortressReader::MAGMA:
        return "MAGMA";
        break;
    case RemoteFortressReader::DRIFTWOOD:
        return "DRIFTWOOD";
        break;
    case RemoteFortressReader::POOL:
        return "POOL";
        break;
    case RemoteFortressReader::BROOK:
        return "BROOK";
        break;
    case RemoteFortressReader::RIVER:
        return "RIVER";
        break;
    case RemoteFortressReader::ROOT:
        return "ROOT";
        break;
    case RemoteFortressReader::TREE_MATERIAL:
        return "TREE";
        break;
    case RemoteFortressReader::MUSHROOM:
        return "MUSHROOM";
        break;
    case RemoteFortressReader::UNDERWORLD_GATE:
        return "UNDERWORLD_GATE";
        break;
    default:
        return "?";
        break;
    }
}


const char* TiletypeVariantToString(RemoteFortressReader::TiletypeVariant input)
{
    switch (input)
    {
    case RemoteFortressReader::NO_VARIANT:
        return "NO_VARIANT";
        break;
    case RemoteFortressReader::VAR_1:
        return "VAR_1";
        break;
    case RemoteFortressReader::VAR_2:
        return "VAR_2";
        break;
    case RemoteFortressReader::VAR_3:
        return "VAR_3";
        break;
    case RemoteFortressReader::VAR_4:
        return "VAR_4";
        break;
    default:
        return "?";
        break;
    }
}

RemoteFortressReader::TiletypeShape StringToTiletypeShape(const char* input)
{
    if (input == NULL || input[0] == 0)
    {
        return RemoteFortressReader::NO_SHAPE;
    }
    for (int i = 0; i < RemoteFortressReader::TiletypeShape_ARRAYSIZE; i++)
    {
        if (strcmp(input, TiletypeShapeToString((RemoteFortressReader::TiletypeShape)i)) == 0)
            return (RemoteFortressReader::TiletypeShape)i;
    }
    return RemoteFortressReader::NO_SHAPE;
}
RemoteFortressReader::TiletypeSpecial StringToTiletypeSpecial(const char* input)
{
    if (input == NULL || input[0] == 0)
    {
        return RemoteFortressReader::NO_SPECIAL;
    }
    for (int i = 0; i < RemoteFortressReader::TiletypeSpecial_ARRAYSIZE; i++)
    {
        if (strcmp(input, TiletypeSpecialToString((RemoteFortressReader::TiletypeSpecial)i)) == 0)
            return (RemoteFortressReader::TiletypeSpecial)i;
    }
    return RemoteFortressReader::NO_SPECIAL;
}
RemoteFortressReader::TiletypeMaterial StringToTiletypeMaterial(const char* input)
{
    if (input == NULL || input[0] == 0)
    {
        return RemoteFortressReader::NO_MATERIAL;
    }
    for (int i = 0; i < RemoteFortressReader::TiletypeMaterial_ARRAYSIZE; i++)
    {
        if (strcmp(input, TiletypeMaterialToString((RemoteFortressReader::TiletypeMaterial)i)) == 0)
            return (RemoteFortressReader::TiletypeMaterial)i;
    }
    return RemoteFortressReader::NO_MATERIAL;
}
RemoteFortressReader::TiletypeVariant StringToTiletypeVariant(const char* input)
{
    if (input == NULL || input[0] == 0)
    {
        return RemoteFortressReader::NO_VARIANT;
    }
    for (int i = 0; i < RemoteFortressReader::TiletypeVariant_ARRAYSIZE; i++)
    {
        if (strcmp(input, TiletypeVariantToString((RemoteFortressReader::TiletypeVariant)i)) == 0)
            return (RemoteFortressReader::TiletypeVariant)i;
    }
    return RemoteFortressReader::NO_VARIANT;
}
