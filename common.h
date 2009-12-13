#pragma once
#pragma warning( disable : 4312 ) //64-bit portability issue
#pragma warning( disable : 4996 ) //'deprecated' function warning
#pragma warning( disable : 4251 ) //'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#define SKIP_DFHACK_STDINT

#include <assert.h>
#include <vector>
#include <map>
#include <allegro.h>

using namespace std;

#include "dfhack/library/DFTypes.h"
#include "dfhack/library/DFTileTypes.h"
#include "dfhack/library/DFHackAPI.h"
#include "dfhack/library/DFMemInfo.h"
using namespace DFHack;

#include "commonTypes.h"
#include "Block.h"
#include <stdio.h>

//#define RELEASE

#define null 0

#define INVALID_INDEX -1

#define ISOMETRIC
//#define CAVALIER

// TILEWIDTH: total size of sprite left to right
// TILEHEIGHT: floor part of sprite top to bottom
// WALLHEIGHT: height of a one pixel wide stripe of wall top to bottom
//		== amount top of wall is raised from where the floor would be
// FLOORHEIGHT: height of a one pixel stripe of the 'wall' of a floor tile

#ifdef CAVALIER
#define TILEWIDTH 46
#define TILEHEIGHT 46
#define WALLHEIGHT 24
#define FLOORHEIGHT 6
#endif

#ifdef ISOMETRIC
#define TILEWIDTH 32
#define TILEHEIGHT 16
#define WALLHEIGHT 16
#define FLOORHEIGHT 4
#endif

#ifdef DOUBLESIZE
#define TILEWIDTH 64
#define TILEHEIGHT 32
#define WALLHEIGHT 32
#define FLOORHEIGHT 8
#endif

#define GFXMODE GFX_AUTODETECT_WINDOWED
#define FULLSCREEN false 
#define RESOLUTION_WIDTH 800
#define RESOLUTION_HEIGHT 600
// Height of a one pixel stripe of the wall of an entire block,
//		including wall and floor tile
#define BLOCKHEIGHT (WALLHEIGHT + FLOORHEIGHT)
// Width of area copied from an image file
// may be different to tile dimensions to allow overlap later
#define SPRITEWIDTH TILEWIDTH
// Height of area copied from an image file
// may be different to tile dimensions to allow overlap later
#define SPRITEHEIGHT (TILEHEIGHT + WALLHEIGHT)
#define WALL_CUTOFF_HEIGHT 15

#define DEFAULT_SEGMENTSIZE 20
#define DEFAULT_SEGMENTSIZE_Z  6
#define MAPNAVIGATIONSTEP 1
#define MAPNAVIGATIONSTEPBIG 10

#define SHEET_OBJECTSWIDE 20

//do not alter, defined by DF
#define CELLEDGESIZE 16

#define COLOR_SEGMENTOUTLINE 0x112211

#define BASE_SHADOW_TILE 160

// this shouldn't change with mods, so should
// be know ahead of time (especially since we
// use it for fake terrains already)
#define MAX_BASE_TERRAIN 525
#define FAKE_TERRAIN_COUNT 0
// we only need as many as our translation system
// in ContentLoader/lookupMaterialType deals with
#define MAX_MATGLOSS 24

enum dirTypes{
  eSimpleInvalid = -1,
  eSimpleSingle,
  //-----START: Do NOT rearrange these, they're used to autoinsert building borders
  eSimpleN,
  eSimpleW,
  eSimpleS,
  eSimpleE,

  eSimpleNnW,
  eSimpleSnW,
  eSimpleSnE,
  eSimpleNnE,
  //-----END

  eSimpleNnS,
  eSimpleWnE,

  eSimpleNnEnS,
  eSimpleNnEnW,
  eSimpleNnWnS,
  eSimpleWnSnE,

  eSimpleNnWnSnE
};

enum dirRelative{
  eLeft,
  eDown,
  eRight,
  eUp,
  eAbove,
  eBelow,

  eUpLeft,
  eUpRight,
  eDownLeft,
  eDownRight,
};
//class WorldSegment;


//main.cpp
void correctBlockForSegmetOffset(int32_t& x, int32_t& y, int32_t& z);

void WriteErr(char* msg, ...);
void LogVerbose(char* msg, ...);

extern GameConfiguration config;

extern uint32_t DebugInt1;
extern uint32_t ClockedTime;
extern uint32_t ClockedTime2;

extern bool timeToReloadSegment;
extern bool timeToReloadConfig;
extern char currentAnimationFrame;
extern bool animationFrameShown;

#define MAX_ANIMFRAME 6

// binary 00111111
#define ALL_FRAMES 63

#define TMR1_START (ClockedTime = clock())
#define TMR1_STOP  (ClockedTime = clock() - ClockedTime)
#define TMR2_START (ClockedTime2 = clock())
#define TMR2_STOP  (ClockedTime2 = clock() - ClockedTime2)


void correctBlockForRotation(int32_t& x, int32_t& y, int32_t& z);

//from UserInput.cpp
void doKeys();
void initAutoReload();
void abortAutoReload();

//Config.cpp
bool loadConfigFile();


//xmlBuildingReader.cpp
class BuildingConfiguration;

#define FILENAME_BUFFERSIZE 50
// temp buffers are bigger because they deal with absolute paths
#define FILENAME_BUFFERSIZE_LOCAL 100
