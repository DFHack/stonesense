#pragma once

#include "df/construction.h"

void changeConstructionMaterials(WorldSegment* segment, std::vector<df::construction>* allConstructions);
bool readConstructionsToTile( Tile* b, const DFHack::Buildings::t_building* building );
