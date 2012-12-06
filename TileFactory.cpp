#include "TileFactory.h"
#include "Tile.h"


TileFactory tileFactory;

TileFactory::TileFactory(void)
{
    poolSize = 0;
    tilesCreated = 0;
}

TileFactory::~TileFactory(void)
{
    for(uint32_t i=0; i<poolSize; i++) {
        free( pool[i] );
        tilesCreated--;
    }
    poolSize = 0;
    pool.clear();
}


Tile* TileFactory::allocateTile()
{
    if( poolSize > 0 ) {
        poolSize--;
        Tile* b = pool[poolSize];
        pool.pop_back();
        return b;
    }

    //pool is empty(full), just New up a new tile
    tilesCreated++;
    Tile* b = (Tile*) malloc( sizeof(Tile) );
    return b;
}


void TileFactory::deleteTile(Tile *b)
{
    if(2*poolSize < tilesCreated) {
        poolSize++;
        pool.push_back( b );
    } else {
        tilesCreated--;
        free( b );
    }
}
