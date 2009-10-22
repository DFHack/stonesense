#pragma once
#pragma warning( disable : 4312 )

#include <assert.h>
#include <vector>
#include <allegro.h>

using namespace std;

#include "dfhack/library/DFTypes.h"
#include "dfhack/library/DFTileTypes.h"
#include "dfhack/library/DFHackAPI.h"


#include "Block.h"

//#define RELEASE
#define DEBUG

#define null 0

#define INVALID_INDEX -1


#define GFXMODE GFX_AUTODETECT_WINDOWED
#define FULLSCREEN false
#define RESOLUTION_WIDTH 800
#define RESOLUTION_HEIGHT 600
#define TILEWIDTH 32
#define TILEHEIGHT 16
#define WALLHEIGHT 16
#define SPRITEWIDTH TILEWIDTH
#define SPRITEHEIGHT (TILEHEIGHT + WALLHEIGHT)
#define MAPSIZE 25

#define DEFAULT_SEGMENTSIZE_X 40
#define DEFAULT_SEGMENTSIZE_Y 40
#define DEFAULT_SEGMENTSIZE_Z  10
#define MAPNAVIGATIONSTEP 1
#define MAPNAVIGATIONSTEPBIG 10

#define SHEET_OBJECTSWIDE 20

//do not alter, defined by DF
#define CELLEDGESIZE 16

#define COLOR_SEGMENTOUTLINE 0x112211



typedef struct Crd2D {
	uint32_t x,y;
}Crd2D;
typedef struct Crd3D {
	uint32_t x,y,z;
}Crd3D;

typedef struct {
  bool show_zones;
  bool show_stockpiles;
  bool single_layer_view;
  bool shade_hidden_blocks;
  int automatic_reload_time;

  int screenWidth;
  int screenHeight;
  bool Fullscreen;

  Crd3D segmentSize;

} GameConfiguration;

// normal tree = 0
// tree by water or something = 1
// Shrub = 2
enum material{
  MAT_WILLOW = 9, 
  MAT_DOLOMITE = 135,
	MAT_FLINT = 136,
  MAT_TALC = 171,

};

enum dirTypes{
  eSimpleInvalid = -1,
  eSimpleSingle,
  eSimpleN,
  eSimpleW,
  eSimpleS,
  eSimpleE,
  eSimpleNnS,
  eSimpleWnE,

  eSimpleNnW,
  eSimpleSnW,
  eSimpleSnE,
  eSimpleNnE,

  eSimpleNnEnS,
  eSimpleNnEnW,
  eSimpleNnWnS,
  eSimpleWnSnE,

  eSimpleNnWnSnE
};

//class WorldSegment;


//main.cpp
void correctBlockForSegmetOffset(uint32_t& x, uint32_t& y, uint32_t& z);


extern GameConfiguration config;

extern uint32_t DebugInt1;
extern uint32_t ClockedTime;
extern uint32_t ClockedTime2;

extern bool timeToReloadSegment;

#define TMR2_START (ClockedTime2 = clock())
#define TMR2_STOP (ClockedTime2 = clock() - ClockedTime2)


//from UserInput.cpp
void doKeys();

//Config.cpp
bool loadConfigFile();


//xmlBuildingReader.cpp
class BuildingConfiguration;
bool LoadBuildingConfiguration( vector<BuildingConfiguration>* knownBuildings );





