#pragma once

class ConditionalSprite
{
private:
  bool matchPosition(Block* b);
  bool matchMaterialType(Block* b);
  bool matchNeighbourHasWall(Block* b);

public:
  int  spriteIndex;

  int  cMaterialType;
  int  cMaterialIndex;
  bool cOnWesternEdge;
  int  cPositionIndex;
  dirTypes cNeighbourHasWall;
  

  ConditionalSprite(void);
  ~ConditionalSprite(void){}

  bool BlockMatches(Block* b);
};
