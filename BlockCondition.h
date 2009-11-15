#pragma once
#include "common.h"
#include "Block.h"

enum BlockConditionTypes{
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
class BlockCondition
{
public:
  BlockCondition(){};
  virtual ~BlockCondition(void){};

  virtual bool Matches(Block* b) = 0;
};

class ConditionalNode
{
	public:
  ConditionalNode(){};
  virtual ~ConditionalNode(void){};

  //false on failure
  virtual bool addCondition(BlockCondition* cond) = 0;
};

//TODO: sort these (alpha order?)

class NeighbourWallCondition : public BlockCondition
{
  public:
  NeighbourWallCondition(const char* strValue);
  ~NeighbourWallCondition(void){};

  int value;
  bool Matches(Block* b);
};


class PositionIndexCondition : public BlockCondition
{
  public:
  PositionIndexCondition(const char* strValue);
  ~PositionIndexCondition(void){};

  int value;
  bool Matches(Block* b);
};


class MaterialTypeCondition : public BlockCondition
{
  public:
  MaterialTypeCondition(const char* strValue);
  ~MaterialTypeCondition(void){};

  int value;
  bool Matches(Block* b);
};

class MaterialIndexCondition : public BlockCondition
{
  public:
  MaterialIndexCondition(const char* strValue);
  ~MaterialIndexCondition(void){};

  int value;
  bool Matches(Block* b);
};

class AnimationFrameCondition : public BlockCondition
{
  public:
  AnimationFrameCondition(const char* strValue);
  ~AnimationFrameCondition(void){};

  int value;
  bool Matches(Block* b);
};


class BuildingOccupancyCondition : public BlockCondition
{
  public:
  BuildingOccupancyCondition(const char* strValue);
  ~BuildingOccupancyCondition(void){};

  int value;
  bool Matches(Block* b);
};


class NeighbourSameBuildingCondition : public BlockCondition
{
  public:
  NeighbourSameBuildingCondition(const char* strValue);
  ~NeighbourSameBuildingCondition(void){};

  int value;
  bool Matches(Block* b);
};


class NeighbourIdenticalCondition : public BlockCondition
{
  public:
  NeighbourIdenticalCondition(const char* strValue);
  ~NeighbourIdenticalCondition(void){};

  int value;
  bool Matches(Block* b);
};


class NeighbourOfTypeCondition : public BlockCondition
{
  public:
  NeighbourOfTypeCondition(const char* strDir, const char* strValue);
  ~NeighbourOfTypeCondition(void){};

  int value;
  int direction;
  bool Matches(Block* b);
};

class NeighbourSameTypeCondition : public BlockCondition
{
  public:
  NeighbourSameTypeCondition(const char* strDir);
  ~NeighbourSameTypeCondition(void){};

  int direction;
  bool Matches(Block* b);
};

class AndConditionalNode : public BlockCondition, public ConditionalNode
{
  public:
  AndConditionalNode(){};
  ~AndConditionalNode(void);
  
  vector<BlockCondition*> children;

  bool Matches(Block* b);
  bool addCondition(BlockCondition* cond);
};

class OrConditionalNode : public BlockCondition, public ConditionalNode
{
  public:
  OrConditionalNode(){};
  ~OrConditionalNode(void);
  
  vector<BlockCondition*> children;

  bool Matches(Block* b);
  bool addCondition(BlockCondition* cond);
};

class AlwaysCondition : public BlockCondition
{
  public:
  AlwaysCondition(){};
  ~AlwaysCondition(void){};

  bool Matches(Block* b);
};

class NeverCondition : public BlockCondition
{
  public:
  NeverCondition(){};
  ~NeverCondition(void){};

  bool Matches(Block* b);
};

class NotConditionalNode : public BlockCondition, public ConditionalNode
{
  public:
  NotConditionalNode();
  ~NotConditionalNode(void);
  BlockCondition* childcond;
  
  bool Matches(Block* b);
  bool addCondition(BlockCondition* cond);
};

class HaveFloorCondition : public BlockCondition
{
  public:
  HaveFloorCondition(){};
  ~HaveFloorCondition(void){};

  bool Matches(Block* b);
};

