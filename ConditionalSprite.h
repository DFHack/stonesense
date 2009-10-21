#pragma once

#include "BlockCondition.h"

class ConditionalSprite
{
private:

public:
  int  spriteIndex;
  vector<BlockCondition> conditions;
  

  ConditionalSprite(void);
  ~ConditionalSprite(void){}

  bool BlockMatches(Block* b);
};
