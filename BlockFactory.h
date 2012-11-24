#pragma once

#include "Block.h"

class BlockFactory
{
    uint32_t poolSize;
    uint32_t blocksCreated;
    vector<Block*> pool;

public:
    BlockFactory(void);
    ~BlockFactory(void);
    Block* allocateBlock( );
    void deleteBlock( Block* );
    uint32_t getPoolSize() {
        return poolSize;
    }
    uint32_t getBlocksCreated() {
        return blocksCreated;
    }
};



extern BlockFactory blockFactory;