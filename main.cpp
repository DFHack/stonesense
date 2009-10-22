#include <assert.h>
#include <vector>

#include <allegro.h>

using namespace std;

#include "common.h"
#include "Block.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"

uint32_t ClockedTime = 0;
uint32_t ClockedTime2 = 0;
uint32_t DebugInt1;

int keyoffset=0;

GameConfiguration config;
bool timeToReloadSegment;

inline bool IDisWall(int in){
  switch( in ){
    case ID_METALWALL:
    case ID_WOODWALL:
      return true;
      break;
  }
  //if not a custom type, do a lookup in dfHack's interface
  return isWallTerrain( in );
}

inline bool IDisFloor(int in){
  switch( in ){
    case ID_METALFLOOR:
    case ID_WOODFLOOR:
    case ID_WOODFLOOR_DETAIL:
    case ID_WOODFLOOR_STAIR_UPDOWN:
    case ID_WOODFLOOR_STAIR_DOWN:
    case ID_WOODFLOOR_STAIR_UP:
      return true;
      break;
  }
  //if not a custom type, do a lookup in dfHack's interface
  return isFloorTerrain( in );;
}


bool isBlockHighRampEnd(Block* block){
	if(!block) return false;
	if(block->wallType == 0) return false;
	return IDisWall( block->wallType );
}
int CalculateRampType(uint32_t x, uint32_t y, uint32_t z, DisplaySegment* segment){
	bool n = isBlockHighRampEnd( segment->getBlock( x, y-1, z) );
	bool s = isBlockHighRampEnd( segment->getBlock( x, y+1, z) );
	bool e = isBlockHighRampEnd( segment->getBlock( x+1, y, z) );
	bool w = isBlockHighRampEnd( segment->getBlock( x-1, y, z) );
	bool nw = isBlockHighRampEnd( segment->getBlock( x-1, y-1, z) );
	bool ne = isBlockHighRampEnd( segment->getBlock( x+1, y-1, z) );
	bool SW = isBlockHighRampEnd( segment->getBlock( x-1, y+1, z) );
	bool se = isBlockHighRampEnd( segment->getBlock( x+1, y+1, z) );
	
	
	if(n && w)  return 10;
	if(s && w)  return 11;
	if(n && e)  return 12;
	if(s && e)  return 13;

	if(n)  return 2;
	if(w)  return 3;
	if(e)  return 4;
	if(s)  return 5;
	if(nw) return 6;
	if(SW) return 7;
	if(ne) return 8;
	if(se) return 9;
	return 1;
}

bool CoordinateInsideRegion(DisplaySegment* segment, uint32_t x, uint32_t y, uint32_t z){
  if( !segment ) return false;
	if( x < 0 || x >= segment->regionSize.x) return false;
	if( y < 0 || y >= segment->regionSize.y) return false;
	if( z < 0 || z >= segment->regionSize.z) return false;
	
	return true;
}

void ReadCellToSegment(DFHackAPI& DF, DisplaySegment& segment, int CellX, int CellY, int CellZ,
					   uint32_t BoundrySX, uint32_t BoundrySY,
					   uint32_t BoundryEX, uint32_t BoundryEY, 
						 uint16_t Flags/*not in use*/, vector<t_building>* allBuildings, vector<t_construction>* allConstructions)
{

	if(!DF.isValidBlock(CellX, CellY, CellZ))
		return;

	//make boundries local
	BoundrySX -= CellX * CELLEDGESIZE;
	BoundryEX -= CellX * CELLEDGESIZE;
	BoundrySY -= CellY * CELLEDGESIZE;
	BoundryEY -= CellY * CELLEDGESIZE;


	//read cell data
	uint16_t tiletypes[16][16];
	t_designation designations[16][16];
	t_occupancy occupancies[16][16];
	//uint8_t regionoffsets[16];
	DF.ReadTileTypes(CellX, CellY, CellZ, (uint16_t *) tiletypes);
	DF.ReadDesignations(CellX, CellY, CellZ, (uint32_t *) designations);
	DF.ReadOccupancy(CellX, CellY, CellZ, (uint32_t *) occupancies);
	//DF.ReadRegionOffsets(CellX,CellY,CellZ, regionoffsets);

	
	//parse cell
	for(uint32_t ly = BoundrySY; ly <= BoundryEY; ly++){
	for(uint32_t lx = BoundrySX; lx <= BoundryEX; lx++){
		Block* b = new Block ( &segment );
		b->x = lx + (CellX * CELLEDGESIZE);
		b->y = ly + (CellY * CELLEDGESIZE);
		b->z = CellZ;
		if( !CoordinateInsideRegion(&segment, b->x, b->y, b->z) ) 	continue;

		if(designations[lx][ly].bits.flow_size > 0){
			b->water.type  = designations[lx][ly].bits.liquid_type;
			b->water.index = designations[lx][ly].bits.flow_size;
		}

		//read tiletype
		int t = tiletypes[lx][ly];
    if(IDisWall(t)) 
			b->wallType = t;
		if(IDisFloor(t))
			b->floorType = t;
		if(isStairTerrain(t))
			b->stairType = t;
		if(isRampTerrain(t))
			b->ramp.type = t;

		//142,136,15
    if(b->x == 142 && b->y == 136 && b->z == 15)
      int j = 10;

		if(IsConstruction(t)) 
      changeConstructionMaterials(&segment, b, allConstructions);

		 

		//save in segment
		bool isHidden = designations[lx][ly].bits.hidden;
    bool shouldBeIncluded = (!isOpenTerrain(t) && !isHidden) || b->water.index ;
    //include black 
    if(config.shade_hidden_blocks && isHidden && (
       b->z == segment.z + segment.sizez - 1 ||
       b->x == segment.x + segment.sizex - 1 || 
       b->y == segment.y + segment.sizey - 1))
    {
      b->wallType = 0;
      b->building.type = BUILDINGTYPE_TABLE;
      b->overridingBuildingType = SPRITEOBJECT_BLACK;
      shouldBeIncluded= true;
    }
    if(!shouldBeIncluded){
      //check if it is part of a bridge (or possibly other suspended buildings), in which case, include it
      shouldBeIncluded = BlockHasSuspendedBuilding(allBuildings, b);
    }
    
		if( shouldBeIncluded ){
      segment.addBlock(b);
		}
	}
	}
}

DisplaySegment* ReadMapSegment(int x, int y, int z, int sizex, int sizey, int sizez){
  uint32_t index;

  DisplaySegment* segment = new DisplaySegment(x,y,z,sizex,sizey,sizez);

	uint32_t starttime = clock();

	DFHackAPI DF("Memory.xml");
	if(!DF.Attach() || !DF.InitMap())
	{
		return segment;
	}

	//Read Number of cells
	int celldimX, celldimY, celldimZ;
	DF.getSize((unsigned int &)celldimX, (unsigned int &)celldimY, (unsigned int &)celldimZ);
	
  segment->regionSize.x = celldimX * CELLEDGESIZE;
  segment->regionSize.y = celldimY * CELLEDGESIZE;
	segment->regionSize.z = celldimZ;
	
	//read world wide buildings
  vector<t_building> allBuildings;
  ReadBuildings(DF, &allBuildings);
  
  
  // read constructions
  vector<t_construction> allConstructions;
  uint32_t numconstructions = DF.InitReadConstructions();
  t_construction tempcon;
  index = 0;
  while(index < numconstructions)
  {
      DF.ReadConstruction(index, tempcon);
      allConstructions.push_back(tempcon);
      index++;
  }
  DF.FinishReadConstructions();


	//figure out what cells to read
	uint32_t firstTileToReadX = x;
	
	while(firstTileToReadX < (uint32_t) x + sizex){
		int cellx = firstTileToReadX / CELLEDGESIZE;
		uint32_t lastTileInCellX = (cellx+1) * CELLEDGESIZE - 1;
		uint32_t lastTileToReadX = min<uint32_t>(lastTileInCellX, x+sizex-1);
		
		uint32_t firstTileToReadY = y;
		while(firstTileToReadY < (uint32_t) y + sizey){
			int celly = firstTileToReadY / CELLEDGESIZE;
			uint32_t lastTileInCellY = (celly+1) * CELLEDGESIZE - 1;
			uint32_t lastTileToReadY = min<uint32_t>(lastTileInCellY, y+sizey-1);
			
			for(int lz=z-sizez+1; lz <= z; lz++){
				//load the blcoks from this cell to the map segment
				ReadCellToSegment(DF, *segment, cellx, celly, lz, 
													firstTileToReadX, firstTileToReadY, lastTileToReadX, lastTileToReadY,
                          0, &allBuildings, &allConstructions );
			}
			firstTileToReadY = lastTileToReadY + 1;
		}
		firstTileToReadX = lastTileToReadX + 1;
	}
  
  
	//Read Vegetation
	uint32_t numtrees = DF.InitReadVegetation();
	t_tree_desc temptree;
	index = 0;
	while(index < numtrees )
	{
		DF.ReadVegetation(index, temptree);

		//want hashtable :(
		Block* b;
		if( b = segment->getBlock( temptree.x, temptree.y, temptree.z) )
			b->tree = temptree.material;
		index ++;
	}
	DF.FinishReadVegetation();

  

	//merge buildings with segment
  MergeBuildingsToSegment(&allBuildings, segment);
  

	//do misc beautification
  for(uint32_t i=0; i<segment->getNumBlocks(); i++){
			Block* b = segment->getBlock(i);
      //setup ramps
      if(b->ramp.type > 0) 
        b->ramp.index = CalculateRampType(b->x, b->y, b->z, segment);

      //add edges to blocks and floors
      if( b->floorType > 0 ){
        Block* westBlock = segment->getBlock(b->x - 1, b->y, b->z);
        Block* northBlock = segment->getBlock(b->x, b->y - 1, b->z);
        if(!segment->getBlock(b->x - 1, b->y, b->z)) 
          b->depthBorderWest = true;
        if(!segment->getBlock(b->x, b->y - 1, b->z)) 
          b->depthBorderNorth = true;
      }else if( b->wallType > 0 ){
        Block* westBlock = segment->getBlock(b->x - 1, b->y, b->z);
        Block* northBlock = segment->getBlock(b->x, b->y - 1, b->z);
        if(westBlock && !westBlock->wallType && !westBlock->ramp.type) 
          b->depthBorderWest = true;
        if(northBlock && !northBlock->wallType && !northBlock->ramp.type) 
          b->depthBorderNorth = true;
      }
	}

  //cleanup
  DF.Detach();

	ClockedTime = clock() - starttime;

	return segment;
}

void correctBlockForSegmetOffset(uint32_t& x, uint32_t& y, uint32_t& z){
	x -= DisplayedSegmentX;
	y -= DisplayedSegmentY;
  z -= DisplayedSegmentZ;
}

void reloadDisplayedSegment(){
  if(DisplayedSegmentX<0)DisplayedSegmentX=0;
  if(DisplayedSegmentY<0)DisplayedSegmentY=0;
  if(viewedSegment)
    viewedSegment->Dispose();
	free(viewedSegment);

  int segmentHeight = config.single_layer_view ? 1 : config.segmentSize.z;
	viewedSegment = ReadMapSegment(DisplayedSegmentX, DisplayedSegmentY, DisplayedSegmentZ,
		config.segmentSize.x,config.segmentSize.y,segmentHeight);
}


void xmltest();
int main(void)
{	
	allegro_init();
  install_keyboard();
  //install_mouse();

	

  config.shade_hidden_blocks = true;
  config.automatic_reload_time = 0;
  config.Fullscreen = FULLSCREEN;
  config.screenHeight = RESOLUTION_HEIGHT;
  config.screenWidth = RESOLUTION_WIDTH;
  config.segmentSize.x = DEFAULT_SEGMENTSIZE_X;
  config.segmentSize.y = DEFAULT_SEGMENTSIZE_Y;
  config.segmentSize.z = DEFAULT_SEGMENTSIZE_Z;
  loadConfigFile();
  
  //load building configuration information from xml files
  LoadBuildingConfiguration( &buildingTypes );

	set_color_depth(16);
  int gfxMode = config.Fullscreen ? GFX_AUTODETECT : GFX_AUTODETECT_WINDOWED;
  if( set_gfx_mode(gfxMode, config.screenWidth, config.screenHeight, 0,0) != 0 ){
    allegro_message("unable to set graphics mode.");
    return 1;
  }

	if (is_windowed_mode()) {
		int res = set_display_switch_mode(SWITCH_BACKGROUND);
		if( res != 0 )
			allegro_message("could not set run in background mode");
	}
  
#ifdef RELEASE
  textprintf_centre(screen, font, config.screenWidth/2, 50, 0xffffff, "Welcome to alpha of Stonesense!");
	textprintf_centre(screen, font, config.screenWidth/2, 60, 0xffffff, "Stonesense is an isometric viewer for Dwarf Fortress.");
  
	
	textprintf_centre(screen, font, config.screenWidth/2, 80, 0xffffff, "Programing by Jonas Ask");
	textprintf_centre(screen, font, config.screenWidth/2, 90, 0xffffff, "Artwork by Dale Holdampf");

	textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-80, 0xffffff, "Testers: TOMzors, sava2004 and peterix");
	textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-70, 0xffffff, "Special thanks to peterix for making dfHack");

	//"The program is in a very early alpha, we're only showcasing it to get ideas and feedback, so use it at your own risk."
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-50, 0xffffff, "Press F9 to continue");
	
#endif
  loadGraphicsFromDisk();
#ifdef RELEASE
	while(!key[KEY_F9]) readkey();
#endif

	//upper left corners
	DisplayedSegmentX = DisplayedSegmentY = 0;DisplayedSegmentZ = 15;
	//Middle of fort
	//DisplayedSegmentX = 131; DisplayedSegmentY = 100;DisplayedSegmentZ = 17;
	//Two trees and a shrub
	//DisplayedSegmentX = 114; DisplayedSegmentY = 124;DisplayedSegmentZ = 15;

	//ramps
	//DisplayedSegmentX = 238; DisplayedSegmentY = 220;DisplayedSegmentZ = 23;

	//ford. Main hall
	DisplayedSegmentX = 182; DisplayedSegmentY = 195;DisplayedSegmentZ = 15;

  //ford. desert map
  //sDisplayedSegmentX = 78; DisplayedSegmentY = 123;DisplayedSegmentZ = 15;

  //DisplayedSegmentX = 227; DisplayedSegmentY = 158;DisplayedSegmentZ = 19;

  //DisplayedSegmentX = 80; DisplayedSegmentY = 56;DisplayedSegmentZ = 18;
  

  #ifdef RELEASE
  DisplayedSegmentX = 0; DisplayedSegmentY = 0;DisplayedSegmentZ = 15;
  #endif

	reloadDisplayedSegment();
	if(!viewedSegment) return 1;

	paintboard();
	while(!key[KEY_ESC]){
		rest(30);
    if( timeToReloadSegment ){
      reloadDisplayedSegment();
      paintboard();
      timeToReloadSegment = false;
    }
		doKeys();
	}
  destroyGraphics();
	return 0;
}
END_OF_MAIN()


