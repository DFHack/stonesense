#pragma once

#include <assert.h>
#include <vector>
#include <map>
#include <memory>
#include <stdint.h>

// allegro also leaks stdint.h and some weird equivalent of it on windows. let's disable the copy leaked by dfhack.
#define SKIP_DFHACK_STDINT
#define DFHACK_WANT_PLATETYPES

#include <PluginManager.h>

#include <modules/Buildings.h>
#include <modules/Gui.h>
#include <modules/Units.h>
#include <modules/World.h>

#define ALLEGRO_NO_MAGIC_MAIN //This is a DLL file. we got no main function.
#define ALLEGRO_HAVE_STDINT_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/utf8.h>

// allegro leaks X headers, undef some of it here:
#undef TileShape
#undef None
#undef Status

#include "commonTypes.h"
#include "Tile.h"
#include <stdio.h>
#include <stdarg.h>

#define RELEASE

constexpr auto INVALID_INDEX = -1;
constexpr auto UNCONFIGURED_INDEX = -2;

#define ISOMETRIC
//#define CAVALIER

// TILEWIDTH: total size of sprite left to right
// TILETOPHEIGHT: floor part of sprite top to bottom
// WALLHEIGHT: height of a one pixel wide stripe of wall top to bottom
//        == amount top of wall is raised from where the floor would be
// FLOORHEIGHT: height of a one pixel stripe of the 'wall' of a floor plate

#ifdef CAVALIER
constexpr auto TILEWIDTH = 46;
constexpr auto TILETOPHEIGHT = 46;
constexpr auto WALLHEIGHT = 24;
constexpr auto FLOORHEIGHT = 6;
#endif

#ifdef ISOMETRIC
constexpr auto TILEWIDTH = 32;
constexpr auto TILETOPHEIGHT = 16;
constexpr auto WALLHEIGHT = 16;
constexpr auto FLOORHEIGHT = 4;
#endif

#ifdef DOUBLESIZE
constexpr auto TILEWIDTH = 64;
constexpr auto TILETOPHEIGHT = 32;
constexpr auto WALLHEIGHT = 32;
constexpr auto FLOORHEIGHT = 8;
#endif

// constexpr auto GFXMODE = GFX_AUTODETECT_WINDOWED;
constexpr auto DEFAULT_FULLSCREEN_MODE = false;
constexpr auto DEFAULT_RESOLUTION_WIDTH = 800;
constexpr auto DEFAULT_RESOLUTION_HEIGHT = 600;
// Height of a one pixel stripe of the wall of an entire tile,
//        including wall and floor plate
constexpr auto TILEHEIGHT = (WALLHEIGHT + FLOORHEIGHT);
// Width of area copied from an image file
// may be different to plate dimensions to allow overlap later
constexpr auto SPRITEWIDTH = TILEWIDTH;
// Height of area copied from an image file
// may be different to plate dimensions to allow overlap later
constexpr auto SPRITEHEIGHT = (TILETOPHEIGHT + WALLHEIGHT);
constexpr auto WALL_CUTOFF_HEIGHT = 15;

constexpr auto DEFAULT_SIZE = 20;
constexpr auto DEFAULT_SIZE_Z = 6;
constexpr auto MAPNAVIGATIONSTEP = 1;
constexpr auto MAPNAVIGATIONSTEPBIG = 10;

constexpr auto SHEET_OBJECTSWIDE = 20;
constexpr auto LETTERS_OBJECTSWIDE = 16;

//do not alter, defined by DF
constexpr auto BLOCKEDGESIZE = 16;

constexpr auto COLOR_SEGMENTOUTLINE = 0x112211;

constexpr auto BASE_SHADOW_PLATE = 160;
constexpr auto DEFAULT_SHADOW = 4;
constexpr auto MAX_SHADOW = 7;

constexpr auto RANDOM_CUBE = 16;

// this shouldn't change with mods, so should
// be know ahead of time (especially since we
// use it for fake terrains already)
constexpr auto MAX_BASE_TERRAIN = 525;
constexpr auto FAKE_TERRAIN_COUNT = 0;
// we only need as many as our translation system
// in ContentLoader/lookupMaterialType deals with
constexpr auto MAX_MATGLOSS = 24;

enum dirTypes {
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

enum dirRelative {
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
class SegmentWrap;



//main.cpp
void correctTileForDisplayedOffset(int32_t& x, int32_t& y, int32_t& z);

void LogError(const char* msg, ...) Wformat(printf,1,2);
void PrintMessage(const char* msg, ...) Wformat(printf,1,2);
void LogVerbose(const char* msg, ...) Wformat(printf,1,2);
void SetTitle(const char *format, ...) Wformat(printf,1,2);

extern FrameTimers ssTimers;

extern uint32_t DebugInt1;

extern bool timeToReloadSegment;
extern bool timeToReloadConfig;
extern char currentAnimationFrame;
extern uint32_t currentFrameLong;
extern bool animationFrameShown;

constexpr auto MAX_ANIMFRAME = 6;

// binary 00111111
constexpr auto ALL_FRAMES = 0b111111;

extern bool key[ALLEGRO_KEY_MAX];

//from UserInput.cpp
void doMouse();
void doKeys(int32_t key, uint32_t keymod);
void doRepeatActions();
void initAutoReload();
void abortAutoReload();

//Keybinds.cpp
bool loadKeymapFile();
bool getKeyStrings(int32_t keycode, std::string*& keyname, std::string*& actionname);

//xmlBuildingReader.cpp
class BuildingConfiguration;

// BUG: this is dangerous!
constexpr auto FILENAME_BUFFERSIZE = 1024;
constexpr auto FILENAME_BUFFERSIZE_LOCAL = 2048;

extern ALLEGRO_FONT *font;
extern ALLEGRO_KEYBOARD_STATE keyboard;
extern ALLEGRO_TIMER *reloadtimer;
extern ALLEGRO_TIMER * animationtimer;
extern ALLEGRO_MOUSE_STATE mouse;

enum MAT_BASICS {
    INVALID = -1,
    INORGANIC = 0,
    AMBER = 1,
    CORAL = 2,
    GREEN_GLASS = 3,
    CLEAR_GLASS = 4,
    CRYSTAL_GLASS = 5,
    ICE = 6,
    COAL = 7,
    POTASH = 8,
    ASH = 9,
    PEARLASH = 10,
    LYE = 11,
    MUD = 12,
    VOMIT = 13,
    SALT = 14,
    FILTH = 15,
    FILTH_FROZEN = 16,
    UNKOWN_FROZEN = 17,
    GRIME = 18,
    ICHOR = 20,
    LEATHER = 37,
    BLOOD_1 = 39,
    BLOOD_2 = 40,
    BLOOD_3 = 41,
    BLOOD_4 = 42,
    BLOOD_5 = 43,
    BLOOD_6 = 44,
    BLOOD_NAMED = 242,
    PLANT = 419,
    WOOD = 420,
    PLANTCLOTH = 421,

    // filthy hacks to get interface stuff
    DESIGNATION = 422,
    CONSTRUCTION = 423,

};

constexpr auto NUM_FORMS = 5;
constexpr auto FORM_BAR = 1;
constexpr auto FORM_BLOCK = 2;
constexpr auto FORM_BOULDER = 3;
constexpr auto FORM_LOG = 4;

extern int randomCube[RANDOM_CUBE][RANDOM_CUBE][RANDOM_CUBE];
