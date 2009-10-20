#pragma once

#include "common.h"

class DisplaySegment{
private:
  vector<Block*> blocks;
public:
	int x, y, z;
	int sizex, sizey, sizez;
  Crd3D regionSize;
  Block** blocksAsPointerVolume;
  

	DisplaySegment(int x, int y, int z, int sizex, int sizey, int sizez){
		this->x = x; 
    this->y = y; 
    this->z = z - sizez + 1;
		this->sizex = sizex; this->sizey = sizey; this->sizez = sizez;
		
    regionSize.x = regionSize.y = regionSize.z = 0;
    
    uint32_t memoryNeeded = sizex * sizey * sizez * sizeof(Block*);
    blocksAsPointerVolume = (Block**) malloc( memoryNeeded );
    memset(blocksAsPointerVolume, 0, memoryNeeded);
	}

  ~DisplaySegment(){
    for(uint32_t i = 0; i < blocks.size(); i++){
      free(blocks[i]);
    }
  }

  void Dispose(void){
    free(blocksAsPointerVolume);
  }

  uint32_t getNumBlocks(){
    return (uint32_t)blocks.size();
  }

  Block* getBlock(uint32_t x, uint32_t y, uint32_t z);
  Block* getBlock(uint32_t index);
  void addBlock(Block* b);
  void drawAllBlocks(BITMAP* target);
};




void pointToScreen(int *inx, int *iny, int inz);
Crd2D WorldBlockToScreen(uint32_t x, uint32_t y, uint32_t z);
void DrawMinimap(BITMAP* target);
void paintboard();


BITMAP* load_bitmap_withWarning(char* path);
void loadGraphicsFromDisk();
void destroyGraphics();
void saveScreenshot();

extern DisplaySegment* viewedSegment;//current, loaded
extern int DisplayedSegmentX;
extern int DisplayedSegmentY;
extern int DisplayedSegmentZ;


extern BITMAP* IMGFloorSheet; 
extern BITMAP* IMGWallSheet; 
extern BITMAP* IMGStairSheet; 
extern BITMAP* IMGRampSheet; 