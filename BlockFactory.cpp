#include "BlockFactory.h"
#include "Block.h"


BlockFactory blockFactory;

BlockFactory::BlockFactory(void)
{
	mute = al_create_mutex();
	poolSize = 0;
}

BlockFactory::~BlockFactory(void)
{
	al_lock_mutex(mute);
	al_unlock_mutex(mute);
	al_destroy_mutex(mute);
	for(uint32_t i=0; i<poolSize; i++){
		free( pool[i] );
	}
}


Block* BlockFactory::allocateBlock(){
	al_lock_mutex(mute);
	if( poolSize > 0 ){
		poolSize--;
		Block* b = pool[poolSize];
		pool.pop_back();
		al_unlock_mutex(mute);
		return b;
	}

	//pool is empty(full), just New up a new block
	Block* b = (Block*) malloc( sizeof(Block) );
	al_unlock_mutex(mute);
	return b;
}


void BlockFactory::deleteBlock(Block *b){
	al_lock_mutex(mute);
	poolSize++;
	pool.push_back( b );
	al_unlock_mutex(mute);
}
