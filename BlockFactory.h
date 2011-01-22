#pragma once

#include "Block.h"

class BlockFactory
{
  uint32_t poolSize;
  vector<Block*> pool;
  
public:
  BlockFactory(void);
  ~BlockFactory(void);
  Block* allocateBlock( );
  void deleteBlock( Block* );
  uint32_t getPoolSize(){ return poolSize; }
};



extern BlockFactory blockFactory;