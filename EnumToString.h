#pragma once

#include "df/tiletype.h"

const char* TiletypeShapeToString(df::tiletype_shape input);
const char* TiletypeSpecialToString(df::tiletype_special input);
const char* TiletypeMaterialToString(df::tiletype_material input);
const char* TiletypeVariantToString(df::tiletype_variant input);

df::tiletype_shape StringToTiletypeShape(const char* input);
df::tiletype_special StringToTiletypeSpecial(const char* input);
df::tiletype_material StringToTiletypeMaterial(const char* input);
df::tiletype_variant StringToTiletypeVariant(const char* input);

