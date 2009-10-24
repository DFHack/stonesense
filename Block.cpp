#include "common.h"
#include "GUI.h"
#include "Block.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"

#include "dfhack/library/DFTypes.h"


BITMAP* temptile=0;



Block::Block(WorldSegment* ownerSegment)
{
  //clear out own memory
  memset(this, 0, sizeof(Block));

  this->ownerSegment = ownerSegment;

  building.type = BUILDINGTYPE_NA;
}

Block::~Block(void){}


void Block::Draw(BITMAP* target){
	int sheetOffsetX, sheetOffsetY;
	int32_t drawx = x;
	int32_t drawy = y;
  int32_t drawz = z; //- ownerSegment->sizez + 1;

	correctBlockForSegmetOffset( drawx, drawy, drawz);
	drawx *= TILEWIDTH;
	drawy *= TILEWIDTH;
	pointToScreen((int*)&drawx, (int*)&drawy, drawz * WALLHEIGHT);
	drawx -= TILEWIDTH>>1;

  int tileBorderColor = makecol(85,85,85);

	//Draw Floor
	if(floorType > 0){
		sheetOffsetX = TILEWIDTH * GetFloorSpriteMap(floorType);
		masked_blit(IMGFloorSheet, target, sheetOffsetX,0, drawx,drawy, TILEWIDTH,TILEHEIGHT);

    //Northern frame
    if(this->depthBorderNorth)
      line(target, drawx + (TILEWIDTH>>1), drawy, drawx+TILEWIDTH-1, drawy+(TILEHEIGHT>>1)-1, tileBorderColor);

    //Western frame
    if(this->depthBorderWest)
      line(target, drawx, drawy+(TILEHEIGHT>>1)-1, drawx+(TILEWIDTH>>1)-1, drawy, tileBorderColor);
	}


	//Draw Ramp
  if(ramp.type > 0){
    sheetOffsetX = SPRITEWIDTH * ramp.index;
    sheetOffsetY = SPRITEHEIGHT * GetRampMaterialTypeMap(ramp.type);

		masked_blit(IMGRampSheet, target, sheetOffsetX,sheetOffsetY, drawx,drawy - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);
	}

	//Draw Stairs
	if(stairType > 0){
		//down part
		int spriteNum = GetDownStairTypeMap(stairType);
    int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;
    if(spriteNum)
	    masked_blit(IMGObjectSheet, target,
        sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
        drawx,drawy - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);

		//up part
    bool mirrored = false;
    if(findWallCloseTo(ownerSegment, this) == eSimpleW)
      mirrored = true;
		spriteNum = GetUpStairTypeMap(stairType, mirrored);
    sheetx = spriteNum % SHEET_OBJECTSWIDE;
		sheety = spriteNum / SHEET_OBJECTSWIDE;
    if(spriteNum)
	    masked_blit(IMGObjectSheet, target,
        sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
        drawx,drawy - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);
	}

	//vegitation
	if(tree.index > 0 || tree.type > 0){
    int spriteNum =  GetWallSpriteVegitation( (VegetationType) getVegetationType( this->floorType ), tree.index );

		int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;
	  masked_blit(IMGObjectSheet, target,
      sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
      drawx,drawy - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);

	}
	//Building
  bool skipBuilding = 
    (building.type == BUILDINGTYPE_STOCKPILE && !config.show_stockpiles) ||
    (building.type == BUILDINGTYPE_ZONE && !config.show_zones);
  
  if(building.type != BUILDINGTYPE_NA && !skipBuilding){

    int spriteNum =  SPRITEOBJECT_NA; //getBuildingSprite(this->building, mirroredBuilding);
		if(overridingBuildingType)
			spriteNum = overridingBuildingType;
		int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;


    //if( mirroredBuilding == false){
      masked_blit(IMGObjectSheet, target,
			  sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
			  drawx, drawy - (WALLHEIGHT), TILEWIDTH,SPRITEHEIGHT);
    //}else{

    //  if(!temptile) temptile = create_bitmap(32,32);
		  //blit(IMGObjectSheet, temptile,
			 // sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
			 // 0, 0, TILEWIDTH,SPRITEHEIGHT);
    //  draw_sprite_h_flip(target, temptile, drawx,drawy - WALLHEIGHT);
    //}
    
    
	}

  	//Draw Walls
	if(wallType > 0){
    int spriteNum =  GetWallSpriteMap(wallType);
		int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;
    //draw wall
	  masked_blit(IMGObjectSheet, target,
      sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
      drawx,drawy - (WALLHEIGHT), SPRITEWIDTH, SPRITEHEIGHT);

    drawy -= (WALLHEIGHT);
    //Northern border
    if(this->depthBorderNorth)
      line(target, drawx + (TILEWIDTH>>1), drawy, drawx+TILEWIDTH-1, drawy+(TILEHEIGHT>>1)-1, tileBorderColor);

    //Western border
    if(this->depthBorderWest)
      line(target, drawx, drawy+(TILEHEIGHT>>1)-1, drawx+(TILEWIDTH>>1)-1, drawy, tileBorderColor);
	}

	//water
	if(water.index > 0){
		int spriteNum = 0;
    int waterlevel = water.index;

    if(waterlevel == 7) waterlevel--;

		if(water.type == 0)
			spriteNum = SPRITEOBJECT_WATERLEVEL1 + waterlevel - 1;
		if(water.type == 1)
			spriteNum = SPRITEOBJECT_WATERLEVEL1_LAVA + waterlevel - 1;
		int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;

		masked_blit(IMGObjectSheet, target,
			sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
			drawx, drawy - (WALLHEIGHT), TILEWIDTH,SPRITEHEIGHT);
	}

  //creature
  if(creature.type > 0){
    int spriteNum = GetCreatureSpriteMap( &creature );
		int sheetx = spriteNum % SHEET_OBJECTSWIDE;
		int sheety = spriteNum / SHEET_OBJECTSWIDE;

    masked_blit(IMGCreatureSheet, target,
		  sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT,
		  drawx, drawy - (WALLHEIGHT), TILEWIDTH,SPRITEHEIGHT);
  }
}



bool hasWall(Block* b){
  if(!b) return false;
  return b->wallType > 0;
}

bool wallShouldNotHaveBorders( int in ){
  switch( in ){
    case 65: //stone fortification
    case 436: //minstone fortification
    case 326: //lavastone fortification
    case 327: //featstone fortification
    case 494: //constructed fortification
    case ID_WOODFORTIFICATION:
      return true;
      break;
  };
  return false;
}