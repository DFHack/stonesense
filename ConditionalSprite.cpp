#include "common.h"
#include "ConditionalSprite.h"
#include "Block.h"
#include "GameBuildings.h"


ConditionalSprite::ConditionalSprite(void)
{
  memset(this, 0, sizeof(ConditionalSprite));
}


bool ConditionalSprite::BlockMatches(Block* b){
  bool okSoFar = true;

  for(uint32_t i=0; i<conditions.size(); i++){
    okSoFar &= conditions[i].Matches( b );
  }

  return okSoFar;
}