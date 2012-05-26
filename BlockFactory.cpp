#include "BlockFactory.h"
#include "Block.h"


BlockFactory blockFactory;

BlockFactory::BlockFactory(void)
{
	poolSize = 0;
	blocksCreated = 0;
}

BlockFactory::~BlockFactory(void)
{
	for(uint32_t i=0; i<poolSize; i++){
		free( pool[i] );
		blocksCreated--;
	}
	poolSize = 0;
	pool.clear();
}


Block* BlockFactory::allocateBlock(){
	if( poolSize > 0 ){
		poolSize--;
		Block* b = pool[poolSize];
		pool.pop_back();
		return b;
	}

	//pool is empty(full), just New up a new block
	blocksCreated++;
	Block* b = (Block*) malloc( sizeof(Block) );
	return b;
}


void BlockFactory::deleteBlock(Block *b){
	if(2*poolSize < blocksCreated){
		poolSize++;
		pool.push_back( b );
	}
	else{
		blocksCreated--;
		free( b );
	}
}
