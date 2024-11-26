#pragma once

#include <memory>

#include "common.h"
#include "Tile.h"

enum TileConditionTypes {
    Cond_MaterialType,
    Cond_MaterialIndex,
    Cond_NeighbourWall,
    Cond_NeighbourSameBuilding,
    Cond_PositionIndex,
    Cond_NeighbourIdentical,
    Cond_BuildingOcc,
    Cond_NeighbourSameIndex,

};

// abstract base class
class TileCondition
{
public:
    TileCondition() = default;
    virtual ~TileCondition() = default;

    virtual bool Matches(Tile* b) = 0;
};

class ConditionalNode
{
public:
    ConditionalNode() = default;
    virtual ~ConditionalNode() = default;

    //false on failure
    virtual bool addCondition(std::unique_ptr<TileCondition> cond) = 0;
};

//TODO: sort these (alpha order?)

class NeighbourWallCondition : public TileCondition
{
public:
    NeighbourWallCondition(const char* strValue);
    ~NeighbourWallCondition() = default;

    int value;
    bool Matches(Tile* b);
};


class PositionIndexCondition : public TileCondition
{
public:
    PositionIndexCondition(const char* strValue);
    ~PositionIndexCondition() = default;

    int value;
    bool Matches(Tile* b);
};


class MaterialTypeCondition : public TileCondition
{
public:
    MaterialTypeCondition(const char* strValue, const char* strSubtype, const char* item_index);
    ~MaterialTypeCondition() = default;

    int value;
    int subtype;
    int item_index;
    bool Matches(Tile* b);
};

class AnimationFrameCondition : public TileCondition
{
public:
    AnimationFrameCondition(const char* strValue);
    ~AnimationFrameCondition() = default;

    int value;
    bool Matches(Tile* b);
};


class BuildingOccupancyCondition : public TileCondition
{
public:
    BuildingOccupancyCondition(const char* strValue);
    ~BuildingOccupancyCondition() = default;

    df::tile_building_occ value;
    bool Matches(Tile* b);
};


class NeighbourSameBuildingCondition : public TileCondition
{
public:
    NeighbourSameBuildingCondition(const char* strValue);
    ~NeighbourSameBuildingCondition() = default;

    int value;
    bool Matches(Tile* b);
};

class BuildingSpecialCondition : public TileCondition
{
public:
    BuildingSpecialCondition(const char* strValue);
    ~BuildingSpecialCondition() = default;

    int value;
    bool Matches(Tile* b);
};

class NeighbourIdenticalCondition : public TileCondition
{
public:
    NeighbourIdenticalCondition(const char* strValue);
    ~NeighbourIdenticalCondition() = default;

    int value;
    bool Matches(Tile* b);
};


class NeighbourOfTypeCondition : public TileCondition
{
public:
    NeighbourOfTypeCondition(const char* strDir, const char* strValue);
    ~NeighbourOfTypeCondition() = default;

    int value;
    int direction;
    bool Matches(Tile* b);
};

class NeighbourSameTypeCondition : public TileCondition
{
public:
    NeighbourSameTypeCondition(const char* strDir);
    ~NeighbourSameTypeCondition() = default;

    int direction;
    bool Matches(Tile* b);
};

class AndConditionalNode : public TileCondition, public ConditionalNode
{
public:
    AndConditionalNode() = default;
    ~AndConditionalNode() = default;

    std::vector<std::unique_ptr<TileCondition>> children;

    bool Matches(Tile* b);
    bool addCondition(std::unique_ptr<TileCondition> cond);
};

class OrConditionalNode : public TileCondition, public ConditionalNode
{
public:
    OrConditionalNode() = default;
    ~OrConditionalNode() = default;

    std::vector<std::unique_ptr<TileCondition>> children;

    bool Matches(Tile* b);
    bool addCondition(std::unique_ptr<TileCondition> cond);
};

class AlwaysCondition : public TileCondition
{
public:
    AlwaysCondition() = default;
    ~AlwaysCondition() = default;

    bool Matches(Tile* b);
};

class NeverCondition : public TileCondition
{
public:
    NeverCondition() = default;
    ~NeverCondition(void) = default;

    bool Matches(Tile* b);
};

class NotConditionalNode : public TileCondition, public ConditionalNode
{
public:
    NotConditionalNode() = default;
    ~NotConditionalNode() = default;
    std::unique_ptr<TileCondition> childcond;

    bool Matches(Tile* b);
    bool addCondition(std::unique_ptr<TileCondition> cond);
};

class HaveFloorCondition : public TileCondition
{
public:
    HaveFloorCondition() = default;
    ~HaveFloorCondition() = default;

    bool Matches(Tile* b);
};

class FluidBelowCondition : public TileCondition
{
public:
    FluidBelowCondition(const char* strValue);
    ~FluidBelowCondition() = default;

    unsigned int value;
    bool Matches(Tile* b);
};
