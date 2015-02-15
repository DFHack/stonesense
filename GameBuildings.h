#pragma once

#include "common.h"
#include "BuildingConfiguration.h"

/*
 * extension to building_type enum from DFHack
 */
#define BUILDINGTYPE_NA df::building_type::NONE
#define BUILDINGTYPE_TREE (df::building_type) (ENUM_LAST_ITEM(building_type)+1)
#define BUILDINGTYPE_BLACKBOX (df::building_type) (ENUM_LAST_ITEM(building_type)+2)

int getBuildingSprite(Buildings::t_building &building, bool mirrored);
void ReadBuildingsToSegment(DFHack::Core& DF, WorldSegment* segment);
void MergeBuildingsToSegment(vector<Buildings::t_building>* buildings, WorldSegment* segment);
void loadBuildingSprites( Tile* b);
void ReadBuildings(DFHack::Core& DF, vector<Buildings::t_building>* buildingHolder);

dirTypes findWallCloseTo(WorldSegment* segment, Tile* b);