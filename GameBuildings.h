#pragma once

#include "common.h"
#include "BuildingConfiguration.h"

/*
 * extension to building_type enum from DFHack
 */
constexpr auto BUILDINGTYPE_NA = df::building_type::NONE;
constexpr auto BUILDINGTYPE_TREE = (df::building_type)(ENUM_LAST_ITEM(building_type) + 1);
constexpr auto BUILDINGTYPE_BLACKBOX = (df::building_type)(ENUM_LAST_ITEM(building_type) + 2);

int getBuildingSprite(Stonesense_Building &building, bool mirrored);
void ReadBuildingsToSegment(DFHack::Core& DF, WorldSegment* segment);
void MergeBuildingsToSegment(std::vector<Stonesense_Building>* buildings, WorldSegment* segment);
void loadBuildingSprites( Tile* b);
void ReadBuildings(DFHack::Core& DF, std::vector<Stonesense_Building>* buildingHolder);

dirTypes findWallCloseTo(WorldSegment* segment, Tile* b);
