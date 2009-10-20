#pragma once

class ConditionalSprite
{
private:
  bool matchPosition(Block* b);
  bool matchMaterial(Block* b);
  bool matchNeighbourHasWall(Block* b);

public:
  int spriteIndex;
  int  cMaterial;
  bool cOnWesternEdge;
  int  cPositionIndex;
  dirTypes cNeighbourHasWall;

  ConditionalSprite(void);
  ~ConditionalSprite(void){}

  bool BlockMatches(Block* b);
};
