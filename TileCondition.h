#pragma once
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
    TileCondition() {};
    virtual ~TileCondition(void) {};

    virtual bool Matches(Tile* b) = 0;
};

class ConditionalNode
{
public:
    ConditionalNode() {};
    virtual ~ConditionalNode(void) {};

    //false on failure
    virtual bool addCondition(TileCondition* cond) = 0;
};

//TODO: sort these (alpha order?)

class NeighbourWallCondition : public TileCondition
{
public:
    NeighbourWallCondition(const char* strValue);
    ~NeighbourWallCondition(void) {};

    int value;
    bool Matches(Tile* b);
};


class PositionIndexCondition : public TileCondition
{
public:
    PositionIndexCondition(const char* strValue);
    ~PositionIndexCondition(void) {};

    int value;
    bool Matches(Tile* b);
};


class MaterialTypeCondition : public TileCondition
{
public:
    MaterialTypeCondition(const char* strValue, const char* strSubtype, const char* item_index);
    ~MaterialTypeCondition(void) {};

    int value;
    int subtype;
    int item_index;
    bool Matches(Tile* b);
};

class AnimationFrameCondition : public TileCondition
{
public:
    AnimationFrameCondition(const char* strValue);
    ~AnimationFrameCondition(void) {};

    int value;
    bool Matches(Tile* b);
};


class BuildingOccupancyCondition : public TileCondition
{
public:
    BuildingOccupancyCondition(const char* strValue);
    ~BuildingOccupancyCondition(void) {};

    int value;
    bool Matches(Tile* b);
};


class NeighbourSameBuildingCondition : public TileCondition
{
public:
    NeighbourSameBuildingCondition(const char* strValue);
    ~NeighbourSameBuildingCondition(void) {};

    int value;
    bool Matches(Tile* b);
};

class BuildingSpecialCondition : public TileCondition
{
public:
    BuildingSpecialCondition(const char* strValue);
    ~BuildingSpecialCondition(void) {};

    int value;
    bool Matches(Tile* b);
};

class NeighbourIdenticalCondition : public TileCondition
{
public:
    NeighbourIdenticalCondition(const char* strValue);
    ~NeighbourIdenticalCondition(void) {};

    int value;
    bool Matches(Tile* b);
};


class NeighbourOfTypeCondition : public TileCondition
{
public:
    NeighbourOfTypeCondition(const char* strDir, const char* strValue);
    ~NeighbourOfTypeCondition(void) {};

    int value;
    int direction;
    bool Matches(Tile* b);
};

class NeighbourSameTypeCondition : public TileCondition
{
public:
    NeighbourSameTypeCondition(const char* strDir);
    ~NeighbourSameTypeCondition(void) {};

    int direction;
    bool Matches(Tile* b);
};

class AndConditionalNode : public TileCondition, public ConditionalNode
{
public:
    AndConditionalNode() {};
    ~AndConditionalNode(void);

    std::vector<TileCondition*> children;

    bool Matches(Tile* b);
    bool addCondition(TileCondition* cond);
};

class OrConditionalNode : public TileCondition, public ConditionalNode
{
public:
    OrConditionalNode() {};
    ~OrConditionalNode(void);

    std::vector<TileCondition*> children;

    bool Matches(Tile* b);
    bool addCondition(TileCondition* cond);
};

class AlwaysCondition : public TileCondition
{
public:
    AlwaysCondition() {};
    ~AlwaysCondition(void) {};

    bool Matches(Tile* b);
};

class NeverCondition : public TileCondition
{
public:
    NeverCondition() {};
    ~NeverCondition(void) {};

    bool Matches(Tile* b);
};

class NotConditionalNode : public TileCondition, public ConditionalNode
{
public:
    NotConditionalNode();
    ~NotConditionalNode(void);
    TileCondition* childcond;

    bool Matches(Tile* b);
    bool addCondition(TileCondition* cond);
};

class HaveFloorCondition : public TileCondition
{
public:
    HaveFloorCondition() {};
    ~HaveFloorCondition(void) {};

    bool Matches(Tile* b);
};

class FluidBelowCondition : public TileCondition
{
public:
    FluidBelowCondition(const char* strValue);
    ~FluidBelowCondition(void) {};

    int value;
    bool Matches(Tile* b);
};

