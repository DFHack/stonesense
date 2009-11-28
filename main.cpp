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
#include "Creatures.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"

uint32_t ClockedTime = 0;
uint32_t ClockedTime2 = 0;
uint32_t DebugInt1;

int keyoffset=0;

GameConfiguration config;
bool timeToReloadSegment;
char currentAnimationFrame;
bool animationFrameShown;

vector<t_matgloss> v_stonetypes;

/*int32_t viewx = 0;
int32_t viewy = 0;
int32_t viewz = 0;
bool followmode = true;*/

void WriteErr(char* msg, ...){
  int j = 10;  
  va_list arglist;
  va_start(arglist, msg);
//  char buf[200] = {0};
//  vsprintf(buf, msg, arglist);
  FILE* fp = fopen( "Stonesense.log", "a");
  if(fp)
    vfprintf( fp, msg, arglist );
  va_end(arglist);
  fclose(fp);
}

void LogVerbose(char* msg, ...){
	if (!config.verbose_logging)
		return;
  int j = 10;  
  va_list arglist;
  va_start(arglist, msg);
//  char buf[200] = {0};
//  vsprintf(buf, msg, arglist);
  FILE* fp = fopen( "Stonesense.log", "a");
  if(fp)
    vfprintf( fp, msg, arglist );
  va_end(arglist);
  fclose(fp);
}

void correctBlockForSegmetOffset(int32_t& x, int32_t& y, int32_t& z){
	x -= viewedSegment->x;
	y -= viewedSegment->y; //DisplayedSegmentY;
  z -= viewedSegment->z + viewedSegment->sizez - 1;
}


void benchmark(){
  DisplayedSegmentX = DisplayedSegmentY = 0;
  DisplayedSegmentX = 110; DisplayedSegmentY = 110;DisplayedSegmentZ = 18;
  uint32_t startTime = clock();
  int i = 20;
  while(i--)
	  reloadDisplayedSegment();

  FILE* fp = fopen("benchmark.txt", "w" );
  if(!fp) return;
  fprintf( fp, "%lims", clock() - startTime);
  fclose(fp);
}

void animUpdateProc()
{
	if (animationFrameShown)
	{
		// check before setting, or threadsafety will be borked
		if (currentAnimationFrame > 4) // ie ends up 0-5
			currentAnimationFrame = 0;
		else
			currentAnimationFrame = currentAnimationFrame + 1;
		animationFrameShown = false;
	}	
}

int main(void)
{	

	allegro_init();
  install_keyboard();
  install_mouse();
  enable_hardware_cursor();
  select_mouse_cursor(MOUSE_CURSOR_ARROW);
  WriteErr("\nStonesense launched\n");

  config.debug_mode = false;
  config.hide_outer_blocks = false;
  config.shade_hidden_blocks = true;
  config.load_ground_materials = true;
  config.automatic_reload_time = 0;
  config.automatic_reload_step = 500;
  config.Fullscreen = FULLSCREEN;
  config.screenHeight = RESOLUTION_HEIGHT;
  config.screenWidth = RESOLUTION_WIDTH;
  config.segmentSize.x = DEFAULT_SEGMENTSIZE;
  config.segmentSize.y = DEFAULT_SEGMENTSIZE;
  config.segmentSize.z = DEFAULT_SEGMENTSIZE_Z;
  config.show_creature_names = true;
  config.show_osd = true;
  config.animation_step = 300;
  config.follow_DFscreen = false;
  loadConfigFile();
  
  //set debug cursor
  debugCursor.x = config.segmentSize.x / 2;
  debugCursor.y = config.segmentSize.y / 2;

	set_color_depth(32);

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
  set_alpha_blender();

  
#ifdef RELEASE
  textprintf_centre(screen, font, config.screenWidth/2, 50, makecol(255,255,0), "Welcome to Stonesense!");
	textprintf_centre(screen, font, config.screenWidth/2, 60, 0xffffff, "Stonesense is an isometric viewer for Dwarf Fortress.");
  
	
	textprintf_centre(screen, font, config.screenWidth/2, 80, 0xffffff, "Programming, Jonas Ask and Kris Parker");
	textprintf_centre(screen, font, config.screenWidth/2, 90, 0xffffff, "Lead graphics designer, Dale Holdampf");
	
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-130, 0xffffff, "Contributors:");
	textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-120, 0xffffff, "7¢ Nickel, Belal, DeKaFu, Dante, Deon, dyze, Errol, fifth angel,");
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-110, 0xffffff, "frumpton, IDreamOfGiniCoeff, Impaler, Japa, jarathor, ");
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-100, 0xffffff, "Jiri Petru, Lord Nightmare, McMe, Mike Mayday, Nexii ");
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-90, 0xffffff, "Malthus, peterix, Seuss, Talvara, winner, and Xandrin.");

	textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-70, 0xffffff, "With special thanks to peterix for making dfHack");

	//"The program is in a very early alpha, we're only showcasing it to get ideas and feedback, so use it at your own risk."
  textprintf_centre(screen, font, config.screenWidth/2, config.screenHeight-40, 0xffffff, "Press F9 to continue");
	
#endif
  loadGraphicsFromDisk();
#ifdef RELEASE
	while(!key[KEY_F9]) readkey();
#endif
  
	//upper left corners
	DisplayedSegmentX = DisplayedSegmentY = DisplayedSegmentZ = 0;
	//Middle of fort
	//DisplayedSegmentX = 131; DisplayedSegmentY = 100;DisplayedSegmentZ = 17;
	//Two trees and a shrub
	//DisplayedSegmentX = 114; DisplayedSegmentY = 124;DisplayedSegmentZ = 15;

	//ramps
	//DisplayedSegmentX = 238; DisplayedSegmentY = 220;DisplayedSegmentZ = 23;

	//ford. Main hall
	DisplayedSegmentX = 172; DisplayedSegmentY = 195;DisplayedSegmentZ = 15;

  //ford. desert map
  //sDisplayedSegmentX = 78; DisplayedSegmentY = 123;DisplayedSegmentZ = 15;

  DisplayedSegmentX = 155; DisplayedSegmentY = 177;DisplayedSegmentZ = 18;

  //DisplayedSegmentX = 242; DisplayedSegmentY = 345;DisplayedSegmentZ = 15;
  

  #ifdef RELEASE
  DisplayedSegmentX = 0; DisplayedSegmentY = 0;DisplayedSegmentZ = 18;
  #endif

  contentLoader.Load();

  //while(1)
	reloadDisplayedSegment();
	//if(!viewedSegment) return 1;
	
	// we should have a dfhack attached now, load the config
	/*LoadBuildingConfiguration( &buildingTypes );
	LoadCreatureConfiguration( &creatureTypes );
	LoadGroundMaterialConfiguration( );
  */
  
	
	// reload now we have config
	reloadDisplayedSegment();

#ifdef BENCHMARK
  benchmark();
#endif
	install_int( animUpdateProc, config.animation_step );
	initAutoReload();
	
	paintboard();
	while(!key[KEY_ESC]){
		rest(30);
    if( timeToReloadSegment ){
      reloadDisplayedSegment();
      paintboard();
      timeToReloadSegment = false;
      animationFrameShown = true;
    }
    else if (animationFrameShown == false)
    {
	 	paintboard();
	 	animationFrameShown = true;
    }
		doKeys();
	}
  destroyGraphics();
  DisconnectFromDF();
	return 0;
}
END_OF_MAIN()


