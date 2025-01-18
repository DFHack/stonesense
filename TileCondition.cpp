#include "TileCondition.h"
#include "GameBuildings.h"
#include "WorldSegment.h"
#include "ContentLoader.h"
#include "StonesenseState.h"

#include <iostream>

int getDirectionFromString(const char* strDir)
{
    if (strDir == NULL) {
        return INVALID_INDEX;
    }
    if( strcmp(strDir, "None") == 0) {
        return eSimpleSingle;
    }
    if( strcmp(strDir, "North") == 0) {
        return eSimpleN;
    }
    if( strcmp(strDir, "South") == 0) {
        return eSimpleS;
    }
    if( strcmp(strDir, "West") == 0) {
        return eSimpleW;
    }
    if( strcmp(strDir, "East") == 0) {
        return eSimpleE;
    }
    //these will change when rotation is available
    if( strcmp(strDir, "TopRight") == 0) {
        return eSimpleN;
    }
    if( strcmp(strDir, "BottomLeft") == 0) {
        return eSimpleS;
    }
    if( strcmp(strDir, "TopLeft") == 0) {
        return eSimpleW;
    }
    if( strcmp(strDir, "BottomRight") == 0) {
        return eSimpleE;
    }
    return INVALID_INDEX;
}

int getBuildingFromString(const char* strType)
{
    df::building_type item = df::building_type::NONE;
    if (DFHack::find_enum_item(&item, strType)) {
        return (int)item;
    }
    return INVALID_INDEX;
}

NeighbourWallCondition::NeighbourWallCondition(const char* strDir)
    : TileCondition()
{
    this->value = getDirectionFromString(strDir);
}

namespace {
    bool neighbor_match(Tile* b, auto value, auto pred)
    {
        bool n = pred(b->ownerSegment->getTileRelativeTo(b->x, b->y, b->z, eUp));
        bool s = pred(b->ownerSegment->getTileRelativeTo(b->x, b->y, b->z, eDown));
        bool w = pred(b->ownerSegment->getTileRelativeTo(b->x, b->y, b->z, eLeft));
        bool e = pred(b->ownerSegment->getTileRelativeTo(b->x, b->y, b->z, eRight));

        if (value == eSimpleN && n) {
            return true;
        }
        if (value == eSimpleS && s) {
            return true;
        }
        if (value == eSimpleW && w) {
            return true;
        }
        if (value == eSimpleE && e) {
            return true;
        }

        if (value == eSimpleSingle && !n && !s && !w && !e) {
            return true;
        }

        return false;
    }
}

bool NeighbourWallCondition::Matches(Tile* b)
{
    return neighbor_match(b, value, hasWall);
}

PositionIndexCondition::PositionIndexCondition(const char* strValue)
    : TileCondition()
{
    this->value = atoi( strValue );
}

bool PositionIndexCondition::Matches(Tile* b)
{
    if(!b->building.info) {
        return false;
    }

    int x = b->x - b->building.info->x1;
    int y = b->y - b->building.info->y1;
    int w = b->building.info->x2 - b->building.info->x1 + 1 ;
    int pos = y * w + x;

    return pos == this->value;
}

MaterialTypeCondition::MaterialTypeCondition(const char* strValue, const char* strSubtype, const char* strPattern_index)
    : TileCondition()
{
    // is there a better way to handle this?
    // seems non-extensible
    subtype = INVALID_INDEX;
    item_index = INVALID_INDEX;
    value = lookupMaterialType(strValue);
    if (value == INVALID_INDEX) {
        return;
    }
    if (strSubtype == NULL || strSubtype[0] == 0) {
        return;
    }
    if (strPattern_index == NULL || strPattern_index[0] == 0) {
        item_index = INVALID_INDEX;
    }
    else{
        item_index = atoi(strPattern_index);
    }
    subtype = lookupMaterialIndex( value, strSubtype);
    if (subtype == INVALID_INDEX) {
        LogVerbose("Material subtype not found in MaterialTypeCondition: %s\n", strSubtype);
        //make material never match;
        value = INVALID_INDEX;
    }
}

bool MaterialTypeCondition::Matches(Tile* b)
{
    if(!b->building.info) {
        return false;
    }
    if(item_index == -1)
    {
        if (b->building.info->material.type != this->value) {
            return false;
        }
        if (this->subtype == INVALID_INDEX) {
            return true;
        }
        return b->building.info->material.index == this->subtype;
    }
    else {
        if (b->building.constructed_mats[item_index%b->building.constructed_mats.size()].matt.type != this->value) {
            return false;
        }
        if (this->subtype == INVALID_INDEX) {
            return true;
        }
        return b->building.constructed_mats[item_index%b->building.constructed_mats.size()].matt.index == this->subtype;
    }
}

AnimationFrameCondition::AnimationFrameCondition(const char* strValue)
    : TileCondition()
{
    this->value = atoi( strValue );
}

bool AnimationFrameCondition::Matches(Tile* b)
{
    return this->value == stonesenseState.currentAnimationFrame;
}


BuildingOccupancyCondition::BuildingOccupancyCondition(const char* strValue)
    : TileCondition()
{
    this->value = df::tile_building_occ( atoi( strValue ) );
}

bool BuildingOccupancyCondition::Matches(Tile* b)
{
    return b->occ.bits.building == this->value;
}

BuildingSpecialCondition::BuildingSpecialCondition(const char* strValue)
    : TileCondition()
{
    this->value = atoi( strValue );
}

bool BuildingSpecialCondition::Matches(Tile* b)
{
    return b->building.special == this->value;
}

NeighbourSameBuildingCondition::NeighbourSameBuildingCondition(const char* strDir)
    : TileCondition()
{
    this->value = getDirectionFromString(strDir);
}

bool NeighbourSameBuildingCondition::Matches(Tile* b)
{
    Stonesense_Building* tilesBuildingIndex = b->building.info;
    auto fn = [&](Tile* b) { return hasBuildingOfIndex(b, tilesBuildingIndex); };
    return neighbor_match(b, value, fn);
}


NeighbourIdenticalCondition::NeighbourIdenticalCondition(const char* strDir)
    : TileCondition()
{
    this->value = getDirectionFromString(strDir);
}

bool NeighbourIdenticalCondition::Matches(Tile* b)
{
    Stonesense_Building* tilesBuildingIndex = b->building.info;
    df::tile_building_occ tilesBuildingOcc = b->occ.bits.building;
    auto fn = [&](Tile* b) { return hasBuildingIdentity(b, tilesBuildingIndex, tilesBuildingOcc); };
    return neighbor_match(b, value, fn);
}


NeighbourOfTypeCondition::NeighbourOfTypeCondition(const char* strDir, const char* strType)
    : TileCondition()
{
    this->direction = getDirectionFromString(strDir);
    this->value = getBuildingFromString(strType);
}

bool NeighbourOfTypeCondition::Matches(Tile* b)
{
    auto fn = [&](Tile* b) { return hasBuildingOfID(b, value); };
    return neighbor_match(b, direction, fn);
}

NeighbourSameTypeCondition::NeighbourSameTypeCondition(const char* strDir)
    : TileCondition()
{
    this->direction = getDirectionFromString(strDir);
}

bool NeighbourSameTypeCondition::Matches(Tile* b)
{
    int value = b->building.type;
    auto fn = [&](Tile* b) { return hasBuildingOfID(b, value); };
    return neighbor_match(b, direction, fn);
}


bool AndConditionalNode::Matches(Tile* b)
{
    uint32_t max = (int)children.size();
    for(uint32_t i=0; i<max; i++) {
        if (!children[i]->Matches( b )) {
            return false;
        }
    }
    return true;
}
bool AndConditionalNode::addCondition(std::unique_ptr<TileCondition> cond)
{
    children.push_back(std::move(cond));
    return true;
}


bool OrConditionalNode::Matches(Tile* b)
{
    uint32_t max = (int)children.size();
    for(uint32_t i=0; i<max; i++) {
        if (children[i]->Matches( b )) {
            return true;
        }
    }
    return false;
}
bool OrConditionalNode::addCondition(std::unique_ptr<TileCondition> cond)
{
    children.push_back(std::move(cond));
    return true;
}

bool AlwaysCondition::Matches(Tile* b)
{
    return true;
}
bool NeverCondition::Matches(Tile* b)
{
    return false;
}


bool NotConditionalNode::Matches(Tile* b)
{
    if (childcond == NULL) {
        return true;
    }
    return !childcond->Matches( b );
}

bool NotConditionalNode::addCondition(std::unique_ptr<TileCondition> cond)
{
    if (childcond != NULL) {
        LogError("Too many condition elements for NotConditionalNode\n");
        return false;
    }
    childcond = std::move(cond);
    return true;
}

bool HaveFloorCondition::Matches(Tile* b)
{
    return (b->tileShapeBasic()==df::tiletype_shape_basic::Floor);
}

FluidBelowCondition::FluidBelowCondition(const char* strValue)
    : TileCondition()
{
    this->value = atoi( strValue );
}

bool FluidBelowCondition::Matches(Tile* b)
{
    Tile* bLow = b->ownerSegment->getTileRelativeTo( b->x, b->y, b->z, eBelow );
    if (!bLow) {
        return false;
    }
    return (bLow->designation.bits.flow_size >= this->value);
}
