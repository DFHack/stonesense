#include <assert.h>
#include <vector>

using namespace std;


#include "common.h"
#include "Block.h"
#include "GUI.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"

#include "loadpng/loadpng.h"


WorldSegment* viewedSegment;
int DisplayedSegmentX;
int DisplayedSegmentY;
int DisplayedSegmentZ;

BITMAP* IMGFloorSheet; 
BITMAP* IMGObjectSheet;
BITMAP* IMGCreatureSheet; 
BITMAP* IMGRampSheet; 
BITMAP* buffer = 0;

Crd2D debugCursor;


void pointToScreen(int *inx, int *iny, int inz){
  static int offx = config.screenWidth / 2;
  static int offy = 50;
  if( config.lift_segment_offscreen ) offy = -200;
	int x=*inx, y=*iny, z=inz;
	x = (int) (x-0.5*(x+y));
	y = (int) (0.5*(x+y));
	x+=offx;
	y+=offy;
	y-=z;

	*inx=x;*iny=y;
}
Crd2D WorldBlockToScreen(int32_t x, int32_t y, int32_t z){
	correctBlockForSegmetOffset( x, y, z);
	return LocalBlockToScreen(x, y, z);
}
Crd2D LocalBlockToScreen(int32_t x, int32_t y, int32_t z){
	x *= TILEWIDTH;
	y *= TILEWIDTH;
	z *= TILEHEIGHT;
	pointToScreen((int*)&x, (int*)&y, z);
	Crd2D result;
	result.x = x;
	result.y = y;
	return result;
}
void DrawCurrentLevelOutline(BITMAP* target, bool backPart){
  int x = viewedSegment->x;
  int y = viewedSegment->y;
  int z = DisplayedSegmentZ;
  int sizex = config.segmentSize.x;
  int sizey = config.segmentSize.y;
	
  if(config.hide_outer_blocks){
    x++;y++;
    sizex -= 2;
    sizey -= 2;
  }

	Crd2D p1 = WorldBlockToScreen(x, y, z);
	Crd2D p2 = WorldBlockToScreen(x, y + sizey , z);
	Crd2D p3 = WorldBlockToScreen(x + sizex , y, z);
	Crd2D p4 = WorldBlockToScreen(x + sizex , y + sizey , z);

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

void drawDebugCursorAndInfo(BITMAP* target){
  Crd2D point = LocalBlockToScreen(debugCursor.x, debugCursor.y, 0);

  int spriteNum =  SPRITEOBJECT_CURSOR;
	int sheetx = spriteNum % SHEET_OBJECTSWIDE;
	int sheety = spriteNum / SHEET_OBJECTSWIDE;
  masked_blit(IMGObjectSheet, target,
    sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
    point.x - SPRITEWIDTH/2, point.y - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);

  //get block info
  Block* b = viewedSegment->getBlockLocal( debugCursor.x, debugCursor.y, viewedSegment->sizez - 1);
  int i = 10;
  textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, "Block 0x%x", b);
  if(!b) return;

  textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
    "Coord:(%i,%i,%i)", b->x,b->y,b->z);

  textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
    "wall:%i floor:%i  Material:%s(%i)", b->wallType, b->floorType, 
    (b->materialIndex != INVALID_INDEX ? v_stonetypes[b->materialIndex].id: ""),
    b->materialIndex);

  if(b->water.index > 0 || b->tree.index != 0)
    textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
      "tree:%i water:%i", b->tree.index, b->water.index);
  //building
  if(b->building.info.type != BUILDINGTYPE_NA && b->building.info.type != BUILDINGTYPE_BLACKBOX){
    textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
      "Building: %s(%i) MatType:%i MatIndex:%i", 
      v_buildingtypes.at(b->building.info.type).c_str(),
      b->building.info.type, b->building.info.material.type, b->building.info.material.index);
  }
  //if creature
  if(b->creature.type){
    textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
      "Creature: %s(%i) ", 
      v_creatureNames.at(b->creature.type).id, b->creature.type);
    
    char strCreature[150] = {0};
    generateCreatureDebugString( &b->creature, strCreature );
    //memset(strCreature, -1, 50);
    textprintf(target, font, 2, config.screenHeight-20-(i--*10), 0xFFFFFF, 
      "flag1: %s ", strCreature );
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

void DrawSpriteFromSheet( int spriteNum, BITMAP* target, BITMAP* spriteSheet, int x, int y){
    int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;
    //
    /*
    static BITMAP* tiny = null;
    if(!tiny)
      tiny = create_bitmap_ex(32, 32, 32);
   
    blit(spriteSheet, tiny, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, 0, 0, SPRITEWIDTH, SPRITEHEIGHT);

    blit(tiny, target,
      0,0,
      10, 60 , SPRITEWIDTH, SPRITEHEIGHT);
*/
    //draw_trans_sprite(target, tiny, x, y);
    
    masked_blit(spriteSheet, target,
      sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
      x, y - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);
}

void DrawSpriteIndexOverlay(){
  clear_to_color(screen, makecol(255,0,255));
  draw_sprite(screen,IMGObjectSheet,0,0);
  for(int i =0; i<= 20*SPRITEWIDTH; i+=SPRITEWIDTH)
    line(screen, i,0,i, config.screenHeight, 0);
  for(int i =0; i< config.screenHeight; i+=SPRITEHEIGHT)
    line(screen, 0,i, 20*SPRITEWIDTH,i, 0);

  for(int y = 0; y<20; y++){
    for(int x = 0; x<20; x+=5){
      int index = y * 20 + x;
      textprintf(screen, font,  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+5, 0xFFffFF, "%i", index);
    }
  }
  textprintf_right(screen, font, config.screenWidth-10, config.screenHeight -10, 0xFFffFF, "Press SPACE to return");
  while(!key[KEY_SPACE]) rest(50);
  //redraw screen again
  paintboard();
}

void paintboard(){
	uint32_t starttime = clock();
	if(!buffer)
		buffer = create_bitmap(config.screenWidth, config.screenHeight);

	clear_to_color(buffer,makecol(95,95,160));
  //clear_to_color(buffer,makecol(12,7,49)); //this one is calm and nice
	

	DrawCurrentLevelOutline(buffer, true);
  viewedSegment->drawAllBlocks(buffer);
	DrawCurrentLevelOutline(buffer, false);

  DebugInt1 = viewedSegment->getNumBlocks();
	
  ClockedTime2 = clock() - starttime;
  
	textprintf_ex(buffer, font, 10,10, 0xFFFFFF,0, "%i,%i,%i", DisplayedSegmentX,DisplayedSegmentY,DisplayedSegmentZ);
  
  if(config.debug_mode){
	  textprintf_ex(buffer, font, 10,20, 0xFFFFFF,0, "Timer1: %ims", ClockedTime);
    textprintf_ex(buffer, font, 10,30, 0xFFFFFF,0, "Timer2: %ims", ClockedTime2);
	  textprintf_ex(buffer, font, 10,40, 0xFFFFFF,0, "D1: %i", DebugInt1);

    drawDebugCursorAndInfo(buffer);
  }

  if(config.single_layer_view)
    textprintf_centre_ex(buffer, font, config.screenWidth/2,20, 0xFFFFFF,0, "Single layer view");
  if(config.automatic_reload_time)
    textprintf_centre_ex(buffer, font, config.screenWidth/2,30, 0xFFFFFF,0, "Reloading every %0.1fs", (float)config.automatic_reload_time/1000);

  DrawMinimap(buffer);

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

  register_png_file_type();
 

  IMGObjectSheet = load_bitmap_withWarning("objects.png");

	IMGFloorSheet = load_bitmap_withWarning("floors.png");
	
  IMGCreatureSheet = load_bitmap_withWarning("creatures.png");

	IMGRampSheet = load_bitmap_withWarning("Ramps.png");
}
void destroyGraphics(){
  destroy_bitmap(IMGFloorSheet);
  destroy_bitmap(IMGObjectSheet);
  destroy_bitmap(IMGCreatureSheet);
  destroy_bitmap(IMGRampSheet);
}

void saveScreenshot(){
  paintboard();
  //get filename
  char filename[20] ={0};
  FILE* fp;
  int index = 1;
  //search for the first screenshot# that does not exist already
  while(true){
    sprintf(filename, "screenshot%i.png", index);
    
    fp = fopen(filename, "r");
    if( fp != 0)
      fclose(fp);
    else
      //file does not exist, so exit loop
      break;
    index++;
  };
  //save_bitmap(filename, buffer, 0);
  save_png(filename, buffer, 0);
}