#include "common.h"
#include "EnumToString.h"

using namespace DFHack;
using namespace df::enums;

const char* TiletypeShapeToString(df::tiletype_shape input)
{
    return enum_item_key_str(input);
}

const char* TiletypeSpecialToString(df::tiletype_special input)
{
    return enum_item_key_str(input);
}

const char* TiletypeMaterialToString(df::tiletype_material input)
{
    return enum_item_key_str(input);
}

const char* TiletypeVariantToString(df::tiletype_variant input)
{
    return enum_item_key_str(input);
}

df::tiletype_shape StringToTiletypeShape(const char* input)
{
    df::tiletype_shape t{};
    if (input && find_enum_item(&t, input))
    {
        return t;
    }
    return tiletype_shape::NONE;
}
df::tiletype_special StringToTiletypeSpecial(const char* input)
{
    df::tiletype_special t{};
    if (input && find_enum_item(&t, input))
    {
        return t;
    }
    return tiletype_special::NONE;
}
df::tiletype_material StringToTiletypeMaterial(const char* input)
{
    df::tiletype_material t{};
    if (input && find_enum_item(&t, input))
    {
        return t;
    }
    return tiletype_material::NONE;
}
df::tiletype_variant StringToTiletypeVariant(const char* input)
{
    df::tiletype_variant t{};
    if (input && find_enum_item(&t, input))
    {
        return t;
    }
    return tiletype_variant::NONE;
}
