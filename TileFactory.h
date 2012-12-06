#pragma once

#include "Tile.h"

class TileFactory
{
    uint32_t poolSize;
    uint32_t tilesCreated;
    vector<Tile*> pool;

public:
    TileFactory(void);
    ~TileFactory(void);
    Tile* allocateTile( );
    void deleteTile( Tile* );
    uint32_t getPoolSize() {
        return poolSize;
    }
    uint32_t getTilesCreated() {
        return tilesCreated;
    }
};



extern TileFactory tileFactory;