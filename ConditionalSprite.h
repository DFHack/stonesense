#pragma once

#include "BlockCondition.h"

class ConditionalSprite
{
private:

public:
  vector<t_SpriteWithOffset> sprites;
  vector<BlockCondition> conditions;
  

  ConditionalSprite(void);
  ~ConditionalSprite(void){}

  bool BlockMatches(Block* b);
};
