#include "common.h"
#include "GUI.h"
#include "MapLoading.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "GameBuildings.h"


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
  //first consider special cases, added by me
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
int CalculateRampType(uint32_t x, uint32_t y, uint32_t z, WorldSegment* segment){
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



void ReadCellToSegment(DFHackAPI& DF, WorldSegment& segment, int CellX, int CellY, int CellZ,
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
		if( !segment.CoordinateInsideRegion( b->x, b->y, b->z) ) 	continue;

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

		if(IDisConstruction(t)) 
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
      b->building.type = BUILDINGTYPE_TABLE; //dosnt matter what kind of building we set it to
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

WorldSegment* ReadMapSegment(int x, int y, int z, int sizex, int sizey, int sizez){
  uint32_t index;

  WorldSegment* segment = new WorldSegment(x,y,z,sizex,sizey,sizez);

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



void reloadDisplayedSegment(){
  if(DisplayedSegmentX<0)DisplayedSegmentX=0;
  if(DisplayedSegmentY<0)DisplayedSegmentY=0;

  //dispose old segment
  if(viewedSegment)
    viewedSegment->Dispose();
	free(viewedSegment);
  
  int segmentHeight = config.single_layer_view ? 1 : config.segmentSize.z;
  //load segment
	viewedSegment = ReadMapSegment(DisplayedSegmentX, DisplayedSegmentY, DisplayedSegmentZ,
		config.segmentSize.x,config.segmentSize.y,segmentHeight);
}