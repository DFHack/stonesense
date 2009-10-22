#include <assert.h>
#include <vector>


using namespace std;

#include "common.h"
#include "Block.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"
#include "MapLoading.h"
#include "WorldSegment.h"

uint32_t ClockedTime = 0;
uint32_t ClockedTime2 = 0;
uint32_t DebugInt1;

int keyoffset=0;

GameConfiguration config;
bool timeToReloadSegment;





void correctBlockForSegmetOffset(int32_t& x, int32_t& y, int32_t& z){
	x -= viewedSegment->x;
	y -= viewedSegment->y; //DisplayedSegmentY;
  z -= viewedSegment->z + viewedSegment->sizez - 1;
}


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
  
  //set debug cursor
  debugCursor.x = config.segmentSize.x / 2;
  debugCursor.y = config.segmentSize.y / 2;

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

  //DisplayedSegmentX = 189; DisplayedSegmentY = 326;DisplayedSegmentZ = 14;
  

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


