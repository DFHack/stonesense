#include <assert.h>
#include <vector>

#include <allegro.h>

using namespace std;


#include "common.h"
#include "Block.h"
#include "GUI.h"



DisplaySegment* viewedSegment;
int DisplayedSegmentX;
int DisplayedSegmentY;
int DisplayedSegmentZ;

BITMAP* IMGFloorSheet; 
BITMAP* IMGWallSheet; 
BITMAP* IMGStairSheet;
BITMAP* IMGRampSheet; 
BITMAP* buffer = 0;



Block* DisplaySegment::getBlock(uint32_t x, uint32_t y, uint32_t z){
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
Block* DisplaySegment::getBlock(uint32_t index){
  if(index<0 || index >= blocks.size() ) 
    return 0;
  return blocks[index];
}

void DisplaySegment::addBlock(Block* b){
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

void DisplaySegment::drawAllBlocks(BITMAP* target){
for(uint32_t i=0; i < this->blocks.size(); i++){
		viewedSegment->blocks[i]->Draw(target);
	}
}


void pointToScreen(int *inx, int *iny, int inz){
  static int offx = config.screenWidth / 2;
  static int offy = 50;
	int x=*inx, y=*iny, z=inz;
	x = (int) (x-0.5*(x+y));
	y = (int) (0.5*(x+y));
	x+=offx;
	y+=offy;
	y-=z;

	*inx=x;*iny=y;
}
Crd2D WorldBlockToScreen(uint32_t x, uint32_t y, uint32_t z){
	correctBlockForSegmetOffset( x, y, z);
	x *= TILEWIDTH;
	y *= TILEWIDTH;
	//z *= TILEHEIGHT;
	pointToScreen((int*)&x, (int*)&y, z);
	Crd2D result;
	result.x = x;
	result.y = y;
	return result;
}
void DrawCurrentLevelOutline(BITMAP* target, bool backPart){
	
	Crd2D p1 = WorldBlockToScreen(DisplayedSegmentX, DisplayedSegmentY, DisplayedSegmentZ);
	Crd2D p2 = WorldBlockToScreen(DisplayedSegmentX, DisplayedSegmentY + config.segmentSize.y , DisplayedSegmentZ);
	Crd2D p3 = WorldBlockToScreen(DisplayedSegmentX + config.segmentSize.x , DisplayedSegmentY, DisplayedSegmentZ);
	Crd2D p4 = WorldBlockToScreen(DisplayedSegmentX + config.segmentSize.x , DisplayedSegmentY + config.segmentSize.y , DisplayedSegmentZ);
	if(backPart){
		line(target, p1.x, p1.y, p1.x, p1.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
		line(target, p1.x, p1.y, p2.x, p2.y, COLOR_SEGMENTOUTLINE);
		line(target, p1.x, p1.y-WALLHEIGHT, p2.x, p2.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
		line(target, p2.x, p2.y, p2.x, p2.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);

		line(target, p1.x, p1.y, p3.x, p3.y, COLOR_SEGMENTOUTLINE);
		line(target, p1.x, p1.y-WALLHEIGHT, p3.x, p3.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
		line(target, p3.x, p3.y, p3.x, p3.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
	}else{
		line(target, p4.x, p4.y, p4.x, p4.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
		line(target, p4.x, p4.y, p2.x, p2.y, COLOR_SEGMENTOUTLINE);
		line(target, p4.x, p4.y-WALLHEIGHT, p2.x, p2.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);

		line(target, p4.x, p4.y, p3.x, p3.y, COLOR_SEGMENTOUTLINE);
		line(target, p4.x, p4.y-WALLHEIGHT, p3.x, p3.y-WALLHEIGHT, COLOR_SEGMENTOUTLINE);
	}
}

void DrawMinimap(BITMAP* target){
	int size = 100;
  double oneBlockInPixels;
  int posx = config.screenWidth-size-10;
	int posy = 10;

  if(!viewedSegment || viewedSegment->regionSize.x == 0 || viewedSegment->regionSize.y == 0){
		textprintf(target, font, posx,posy, 0xFFFFFF, "No map loaded");
		return;
	}

  oneBlockInPixels = (double) size / viewedSegment->regionSize.x;
  //map outine
  int mapheight = (int)(viewedSegment->regionSize.y * oneBlockInPixels);
  rect(target, posx, posy, posx+size, posy+mapheight, 0);
  //current segment outline
  int x = (size * viewedSegment->x) / viewedSegment->regionSize.x;
  int y = (size * viewedSegment->y) / viewedSegment->regionSize.y;
  int segmentWidth = viewedSegment->sizex * oneBlockInPixels;
  int segmentHeight = viewedSegment->sizey * oneBlockInPixels;
  rect(target, posx+x, posy+y, posx+x+segmentWidth, posy+y+segmentHeight,0);
}

void paintboard(){
	uint32_t starttime = clock();
	if(!buffer)
		buffer = create_bitmap(config.screenWidth, config.screenHeight);

	clear_to_color(buffer,makecol(95,95,160));
	
  /*PALETTE pal;
  get_palette(pal);
  for(int i =0; i<255; i++)
    pal[i].g+=1;
  //set_palette(pal);
  select_palette(pal);
*/
	DrawMinimap(buffer);

	DrawCurrentLevelOutline(buffer, true);
  viewedSegment->drawAllBlocks(buffer);
	DrawCurrentLevelOutline(buffer, false);

	DebugInt1 = clock() - starttime;
	
  
	textprintf_ex(buffer, font, 10,10, 0xFFFFFF,0, "%i,%i,%i", DisplayedSegmentX,DisplayedSegmentY,DisplayedSegmentZ);
	
  /*ClockedTime = clock();
  for(int i = 0; i<300000; i++)
    masked_blit(IMGStairSheet, buffer, 0,0, 0,0, TILEWIDTH,24);
  ClockedTime -= clock();*/
  
#ifdef DEBUG
  //masked_blit(IMGFloorSheet, buffer, 0,0, 10,60, TILEWIDTH,24);

	textprintf_ex(buffer, font, 10,20, 0xFFFFFF,0, "Timer1: %ims", ClockedTime);
  textprintf_ex(buffer, font, 10,30, 0xFFFFFF,0, "Timer2: %ims", ClockedTime2);
	textprintf_ex(buffer, font, 10,40, 0xFFFFFF,0, "D1: %i", DebugInt1);
#endif

  if(config.single_layer_view)
    textprintf_centre_ex(buffer, font, config.screenWidth/2,20, 0xFFFFFF,0, "Single layer view");
  if(config.automatic_reload_time)
    textprintf_centre_ex(buffer, font, config.screenWidth/2,30, 0xFFFFFF,0, "Reloading every %0.1fs", (float)config.automatic_reload_time/1000);

	acquire_screen();
	draw_sprite(screen,buffer,0,0);
	release_screen();
}




BITMAP* load_bitmap_withWarning(char* path){
  BITMAP* img = 0;
  img = load_bitmap(path,0);
  if(!img){
    allegro_message("Unable to load image %s", path);
    exit(0);
  }
  return img;
}

void loadGraphicsFromDisk(){

  //PALETTE pal;
  //get_palette(pal);
  //for(int i =0; i<255; i++)
  //  pal[i].g+=1;
  //set_palette(pal);
  //select_palette(pal);
  IMGStairSheet = load_bitmap_withWarning("objects.pcx");

	IMGFloorSheet = load_bitmap_withWarning("floors.bmp");
	IMGWallSheet = load_bitmap_withWarning("walls.bmp");
	
	IMGRampSheet = load_bitmap_withWarning("ramps.bmp");
}
void destroyGraphics(){

  destroy_bitmap(IMGFloorSheet);
  destroy_bitmap(IMGWallSheet);
  destroy_bitmap(IMGStairSheet);
  destroy_bitmap(IMGRampSheet);
}

void saveScreenshot(){
  paintboard();
  //get filename
  string baseFilename = "JEJEEJ";
  char filename[20] ={0};
  FILE* fp;
  int index = 1;
  while(true){
    sprintf_s(filename, "screenshot%i.bmp", index);
    
    fopen_s(&fp, filename, "r");
    if( fp != 0)
      fclose(fp);
    else
      break;
    index++;
  };
  save_bitmap(filename, buffer, 0);
}