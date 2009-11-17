#include "common.h"
#include "GUI.h"
#include "MapLoading.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "Constructions.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "GroundMaterialConfiguration.h"

static API* pDFApiHandle = 0;

memory_info dfMemoryInfo;
bool memInfoHasBeenRead;

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
	bool n = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eUp) );
  bool s = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eDown) );
	bool e = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eRight) );
	bool w = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eLeft) );
	bool nw = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eUpLeft) );
	bool ne = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eUpRight) );
  bool SW = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eDownLeft) );
  bool se = isBlockHighRampEnd( segment->getBlockRelativeTo( x, y, z, eDownRight) );
	
	
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

bool isBlockOnVisisbleEdgeOfSegment(WorldSegment* segment, Block* b){
  if(b->z == segment->z + segment->sizez - 1) 
    return true;

  if(DisplayedRotation == 0 && (
    b->x == segment->x + segment->sizex - 1 || b->y == segment->y + segment->sizey - 1 ))
  {
      return true;
  }else if(DisplayedRotation == 1 && (
    b->x == segment->x + segment->sizex - 1 || b->y == segment->y ))
  {
      return true;
  }else if(DisplayedRotation == 2 && (
    b->x == segment->x || b->y == segment->y ))
  {
      return true;
  }
  else if(DisplayedRotation == 3 && (
    b->x == segment->x || b->y == segment->y + segment->sizey - 1  ))
  {
      return true;
  }

  return false;
}


void ReadCellToSegment(API& DF, WorldSegment& segment, int CellX, int CellY, int CellZ,
					   uint32_t BoundrySX, uint32_t BoundrySY,
					   uint32_t BoundryEX, uint32_t BoundryEY, 
						 uint16_t Flags/*not in use*/, 
             vector<t_building>* allBuildings, 
             vector<t_construction>* allConstructions,
             vector< vector <uint16_t> >* allLayers)
{
  //boundry check
  int celldimX, celldimY, celldimZ;
	DF.getSize((unsigned int &)celldimX, (unsigned int &)celldimY, (unsigned int &)celldimZ);
  if( CellX < 0 || CellX >= celldimX ||
      CellY < 0 || CellY >= celldimY ||
      CellZ < 0 || CellZ >= celldimZ
    ) return;

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
	uint8_t regionoffsets[16];
	DF.ReadTileTypes(CellX, CellY, CellZ, (uint16_t *) tiletypes);
	DF.ReadDesignations(CellX, CellY, CellZ, (uint32_t *) designations);
	DF.ReadOccupancy(CellX, CellY, CellZ, (uint32_t *) occupancies);
	DF.ReadRegionOffsets(CellX,CellY,CellZ, regionoffsets);
  
  //read local vein data
  vector <t_vein> veins;
  DF.ReadVeins(CellX,CellY,CellZ,veins);
  uint32_t numVeins = (uint32_t)veins.size();
	
	//parse cell
	for(uint32_t ly = BoundrySY; ly <= BoundryEY; ly++){
	for(uint32_t lx = BoundrySX; lx <= BoundryEX; lx++){
		uint32_t gx = lx + (CellX * CELLEDGESIZE);
		uint32_t gy = ly + (CellY * CELLEDGESIZE);
		if( !segment.CoordinateInsideSegment( gx, gy, CellZ) )
		{ 
			continue;
		}
		bool createdBlock = false;
		Block* b = segment.getBlock( gx, gy, CellZ);
		
		if (!b)
		{
		  createdBlock = true;
		  b = new Block ( &segment );
		  b->x = gx;
		  b->y = gy;
		  b->z = CellZ;
	  }
    
    b->occ = occupancies[lx][ly];

    //liquids
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
    //if(b->x == 142 && b->y == 136 && b->z == 15)
    //  int j = 10;

		//save in segment
		bool isHidden = designations[lx][ly].bits.hidden;
    //option for including hidden blocks
    isHidden &= !config.show_hidden_blocks;
    bool shouldBeIncluded = (!isOpenTerrain(t) && !isHidden) || b->water.index ;
    //include hidden blocks as shaded black 
    if(config.shade_hidden_blocks && isHidden && isBlockOnVisisbleEdgeOfSegment(&segment, b))
    {
      b->wallType = 0;
      b->building.info.type = BUILDINGTYPE_BLACKBOX;
      t_SpriteWithOffset sprite = {SPRITEOBJECT_BLACK, 0, 0,-1};
      b->building.sprites.push_back( sprite );
      shouldBeIncluded= true;
    }
    if(!shouldBeIncluded){
      //check if it is part of a bridge (or possibly other suspended buildings), in which case, include it
      //shouldBeIncluded = BlockHasSuspendedBuilding(allBuildings, b);
    }
    
		if( shouldBeIncluded ){
      //this only needs to be done for included blocks

      //Translate construction materials
		  if(IDisConstruction(t)) 
        changeConstructionMaterials(&segment, b, allConstructions);

      //determine rock/soil type
      int rockIndex = (*allLayers) [regionoffsets[designations[lx][ly].bits.biome]] [designations[lx][ly].bits.geolayer_index];
      //check veins
      for(uint32_t i=0; i<numVeins; i++){
				//TODO: This will be fixed in dfHack at some point, but right now objects that arnt veins pass through as. So we filter on vtable
				//if(veins[i].vtable != 9302320) only suitable for some versions
        if((uint32_t)veins[i].type >= groundTypes.size())
					continue;
        uint16_t row = veins[i].assignment[ly];
        bool set = (row & (1 << lx)) != 0;
				if(set){
					rockIndex = veins[i].type;
				}
      }
      b->materialIndex = rockIndex;
      //string name = v_stonetypes[j].id;
      if (createdBlock)
      {
      	segment.addBlock(b);
  	  }
    }else if (createdBlock){
      delete(b);
    }

	}
	}
}


bool checkFloorBorderRequirement(WorldSegment* segment, int x, int y, int z, dirRelative offset)
{
	Block* bHigh = segment->getBlockRelativeTo(x, y, z, offset);
	if (bHigh && (bHigh->floorType > 0 || bHigh->ramp.type > 0 || bHigh->wallType > 0))
	{
		return false;
	}
	Block* bLow = segment->getBlockRelativeTo(x, y, z-1, offset);
	if (bLow == NULL || bLow->ramp.type == 0)
	{
		return true;
	}
	return false;
}


WorldSegment* ReadMapSegment(API &DF, int x, int y, int z, int sizex, int sizey, int sizez){
  uint32_t index;
  TMR2_START;
  if( IsConnectedToDF() == false || DF.InitMap() == false ){
    //DF.Detach();
    //return new blank segment
		return new WorldSegment(x,y,z,sizex,sizey,sizez);
  }
  
  //read memory info
  if( memInfoHasBeenRead == false){
    dfMemoryInfo = DF.getMemoryInfo();
    memInfoHasBeenRead = true;
  }
  

	//Read Number of cells
	int celldimX, celldimY, celldimZ;
	DF.getSize((unsigned int &)celldimX, (unsigned int &)celldimY, (unsigned int &)celldimZ);
  //bound view to world
  if(x > celldimX * CELLEDGESIZE -10) DisplayedSegmentX = x = celldimX * CELLEDGESIZE -10;
  if(y > celldimY * CELLEDGESIZE -10) DisplayedSegmentY = y = celldimY * CELLEDGESIZE -10;
  //setup new world segment
  WorldSegment* segment = new WorldSegment(x,y,z,sizex,sizey,sizez);
  segment->regionSize.x = celldimX * CELLEDGESIZE;
  segment->regionSize.y = celldimY * CELLEDGESIZE;
	segment->regionSize.z = celldimZ;
  
	//read world wide buildings
  vector<t_building> allBuildings;
  ReadBuildings(DF, &allBuildings);

  //read stone material types
  DF.ReadStoneMatgloss(v_stonetypes);
  

  if(GroundMaterialNamesTranslatedFromGame == false)
    TranslateGroundMaterialNames();
  
  //read layers
  vector< vector <uint16_t> > layers;
  DF.ReadGeology( layers );
  

  
  // read constructions
  vector<t_construction> allConstructions;
  uint32_t numconstructions = DF.InitReadConstructions();
  t_construction tempcon;
  index = 0;
  while(index < numconstructions)
  {
      DF.ReadConstruction(index, tempcon);
      if(segment->CoordinateInsideSegment(tempcon.x, tempcon.y, tempcon.z))
        allConstructions.push_back(tempcon);
      index++;
  }
  DF.FinishReadConstructions();
  
	//merge buildings with segment
  MergeBuildingsToSegment(&allBuildings, segment);
  
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
                          0, &allBuildings, &allConstructions, &layers );
                          
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

  

  //Read Creatures
  ReadCreaturesToSegment( DF, segment );


	//do misc beautification
  uint32_t numblocks = segment->getNumBlocks();
  for(uint32_t i=0; i < numblocks; i++){
			Block* b = segment->getBlock(i);
	  //setup building sprites
      if( b->building.info.type != BUILDINGTYPE_NA && b->building.info.type != BUILDINGTYPE_BLACKBOX )
      		loadBuildingSprites( b );
			
      //setup ramps
      if(b->ramp.type > 0) 
        b->ramp.index = CalculateRampType(b->x, b->y, b->z, segment);

      //add edges to blocks and floors  
      if( b->floorType > 0 ){
	     b->depthBorderWest = checkFloorBorderRequirement(segment, b->x, b->y, b->z, eLeft);
	     b->depthBorderNorth = checkFloorBorderRequirement(segment, b->x, b->y, b->z, eUp);
      }else if( b->wallType > 0 && wallShouldNotHaveBorders( b->wallType ) == false ){
        Block* leftBlock = segment->getBlockRelativeTo(b->x, b->y, b->z, eLeft);
        Block* upBlock = segment->getBlockRelativeTo(b->x, b->y, b->z, eUp);
        if(!leftBlock || (!leftBlock->wallType && !leftBlock->ramp.type)) 
          b->depthBorderWest = true;
        if(!upBlock || (!upBlock->wallType && !upBlock->ramp.type))
          b->depthBorderNorth = true;
      }
	}
  
  DF.DestroyMap();
TMR2_STOP;

	return segment;
}



//TODO: dont need double ref anymore
bool ConnectDFAPI(API* pDF){
	if(!pDF->Attach() || !pDF->InitMap())
    return false;

  return pDF->isAttached();
}

void DisconnectFromDF(){
  if(pDFApiHandle){
    pDFApiHandle->DestroyMap();
    pDFApiHandle->Detach();
    delete pDFApiHandle;
  }
}

bool IsConnectedToDF(){
  if(!pDFApiHandle) return false;
  return pDFApiHandle->isAttached();
}

void reloadDisplayedSegment(){
  if(DisplayedSegmentX<0)DisplayedSegmentX=0;
  if(DisplayedSegmentY<0)DisplayedSegmentY=0;
  //create handle to dfHack API
  if(pDFApiHandle == 0){
    memInfoHasBeenRead = false;
    pDFApiHandle = new API("Memory.xml");
    if( ConnectDFAPI( pDFApiHandle ) == false ){
      delete( pDFApiHandle );
      pDFApiHandle = 0;
      WriteErr("No Dwarf Fortress executable found\n");
      return;
    }
  }

  TMR1_START;
  //dispose old segment
  if(viewedSegment){
    viewedSegment->Dispose();
	  delete(viewedSegment);
  }
  
  if (config.follow_DFscreen)
  {
	  //WriteErr("checkfollow\n");
	  if (pDFApiHandle->InitViewAndCursor())
	  {
	  	//WriteErr("tryfollow\n");
		int32_t newviewx;
		int32_t newviewy;
		int32_t newviewz;	  
		pDFApiHandle->getViewCoords(newviewx,newviewy,newviewz);
    DisplayedSegmentX = newviewx;
		DisplayedSegmentY = newviewy;
		DisplayedSegmentZ = newviewz;
		/*
    DisplayedSegmentX = DisplayedSegmentX + newviewx - viewx;
		DisplayedSegmentY = DisplayedSegmentY + newviewy - viewy;
		DisplayedSegmentZ = DisplayedSegmentZ + newviewz - viewz;
	  WriteErr("%d %d %d -> %d %d %d\n",viewx,viewy,viewz,newviewx,newviewy,newviewz);
		viewx = newviewx;
		viewy = newviewy;
		viewz = newviewz;*/
	  }else
      //fail
      config.follow_DFscreen = false;
  }
  
  int segmentHeight = config.single_layer_view ? 1 : config.segmentSize.z;
  //load segment
  API& DF = *pDFApiHandle;
	viewedSegment = ReadMapSegment(DF, DisplayedSegmentX, DisplayedSegmentY, DisplayedSegmentZ,
		                config.segmentSize.x, config.segmentSize.y, segmentHeight);
  /*bool res = DF.InitViewAndCursor();
  if(res){
    int x,y,z;
    res = DF.getViewCoords( x, y, z );
  }*/
  TMR1_STOP;
}
