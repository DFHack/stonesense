#ifndef WORLDSEGMENT_H
#define WORLDSEGMENT_H

#include "Block.h"

class WorldSegment{
private:
	vector<Block*> blocks;
public:
	ALLEGRO_MUTEX * mutie;
	bool loaded;
	bool processed;
	int x, y, z;
	int sizex, sizey, sizez;
	unsigned char rotation;
	long read_time;
	long beautify_time;
	Crd3D regionSize;
	Block** blocksAsPointerVolume;
	WorldSegment(int x, int y, int z, int sizex, int sizey, int sizez)
	{
		this->x = x; 
		this->y = y; 
		this->z = z - sizez + 1;
		this->sizex = sizex;
		this->sizey = sizey;
		this->sizez = sizez;

		regionSize.x = regionSize.y = regionSize.z = 0;

		uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Block*);
		blocksAsPointerVolume = (Block**) malloc( memoryNeeded );
		memset(blocksAsPointerVolume, 0, memoryNeeded);
		mutie = al_create_mutex();
	}

	~WorldSegment(){
		uint32_t num = (uint32_t)blocks.size();
		for(uint32_t i = 0; i < num; i++){
			delete(blocks[i]);
		}
		blocks.clear();
		al_destroy_mutex(mutie);
	}

	void Dispose(void){
		free(blocksAsPointerVolume);
	}

	uint32_t getNumBlocks(){
		return (uint32_t)blocks.size();
	}

	Block* getBlock(int32_t x, int32_t y, int32_t z);
	Block* getBlockLocal(uint32_t x, uint32_t y, uint32_t z);
	Block* getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction);
	Block* getBlockRelativeTo(uint32_t x, uint32_t y, uint32_t z,  dirRelative direction, int distance);
	Block* getBlock(uint32_t index);
	void addBlock(Block* b);
	void drawAllBlocks();
	bool CoordinateInsideSegment(uint32_t x, uint32_t y, uint32_t z);
};

#endif