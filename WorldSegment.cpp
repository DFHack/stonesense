#include "common.h"
#include "WorldSegment.h"
#include "GUI.h"


Block* WorldSegment::getBlock(uint32_t x, uint32_t y, uint32_t z){
	if((int)x < this->x || x >= (uint32_t)this->x + this->sizex) return 0;
	if((int)y < this->y || y >= (uint32_t)this->y + this->sizey) return 0;
	if((int)z < this->z || z >= (uint32_t)this->z + this->sizez) return 0;
	/*for(uint32_t i=0; i<this->blocks.size(); i++){
		Block* b = this->blocks[i];
		if(x == b->x && y == b->y && z == b->z) 
			return b;
	}
  return 0;*/
  
  uint32_t lx = x;
  uint32_t ly = y;
  uint32_t lz = z;
  //make local
  lx -= this->x;
  ly -= this->y;
  lz -= this->z;

  uint32_t index = lx + (ly * this->sizex) + ((lz) * this->sizex * this->sizey);
	return blocksAsPointerVolume[index];
}
Block* WorldSegment::getBlockLocal(uint32_t x, uint32_t y, uint32_t z){
	if((int)x < 0 || x >= (uint32_t)this->sizex) return 0;
	if((int)y < 0 || y >= (uint32_t)this->sizey) return 0;
	if((int)z < 0 || z >= (uint32_t)this->sizez) return 0;

  uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
	return blocksAsPointerVolume[index];
}

Block* WorldSegment::getBlock(uint32_t index){
  if(index<0 || index >= blocks.size() ) 
    return 0;
  return blocks[index];
}

void WorldSegment::addBlock(Block* b){
  this->blocks.push_back(b);
  //b = &(this->blocks[ blocks.size() - 1]);

  uint32_t x = b->x;
  uint32_t y = b->y;
  uint32_t z = b->z;
  //make local
  x -= this->x;
  y -= this->y;
  z -= this->z;

  uint32_t index = x + (y * this->sizex) + ((z) * this->sizex * this->sizey);
  //assert( x < sizex && x >=0);
  //assert( y < sizey && y >=0);
  //assert( z < sizez && z >=0);
  //assure not overwriting
  //if(blocksAsPointerVolume[index] != 0)
  //  assert(blocksAsPointerVolume[index] == 0);

  blocksAsPointerVolume[index] = b;  
  /*Block* test = getBlock(b->x, b->y, b->z);
  if(test->x != b->x || test->y != b->y || test->z != b->z){
    test = getBlock(b->x, b->y, b->z);
    assert (test == b);
  }*/

}

void WorldSegment::drawAllBlocks(BITMAP* target){
  uint32_t numblocks = (uint32_t)this->blocks.size();
  for(uint32_t i=0; i < numblocks; i++){
	  viewedSegment->blocks[i]->Draw(target);
	}
}


bool WorldSegment::CoordinateInsideRegion(uint32_t x, uint32_t y, uint32_t z){
	if( x < 0 || (int32_t)x >= this->regionSize.x) return false;
	if( y < 0 || (int32_t)y >= this->regionSize.y) return false;
	if( z < 0 || (int32_t)z >= this->regionSize.z) return false;
	return true;
}