#include "common.h"
#include "GUI.h"
#include "BuildingConfiguration.h"
#include "GroundMaterialConfiguration.h"
#include "GameBuildings.h"
#include "Creatures.h"

void mouseProc(int flags){
  int j = 10;
  if(flags & MOUSE_FLAG_LEFT_DOWN){
    
    
  }
  //paintboard();
}

void automaticReloadProc(){
		timeToReloadSegment = true;
}

void changeRelativeToRotation( int &inputx, int &inputy, int stepx, int stepy ){
  switch(DisplayedRotation){
  case 0:
    inputx += stepx;
    inputy += stepy;
    break;
  case 1:
    inputx += stepy;
    inputy -= stepx;
    break;
  case 2:
    inputx -= stepx;
    inputy -= stepy;
    break;
  case 3:
    inputx -= stepy;
    inputy += stepx;
    break;
  };
}

void moveViewRelativeToRotation( int stepx, int stepy )
{
  if (config.follow_DFscreen)
  	changeRelativeToRotation(config.viewXoffset, config.viewYoffset, stepx, stepy );
  else
  	changeRelativeToRotation(DisplayedSegmentX, DisplayedSegmentY, stepx, stepy );
}

void doKeys(){
  //mouse_callback = mouseProc;

  char stepsize = (key[KEY_LSHIFT] || key[KEY_RSHIFT] ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
  if(key[KEY_UP]){
    	moveViewRelativeToRotation( 0, -stepsize );
		timeToReloadSegment = true;
	}
	if(key[KEY_DOWN]){
    	moveViewRelativeToRotation( 0, stepsize );
		timeToReloadSegment = true;
	}
	if(key[KEY_LEFT]){
    	moveViewRelativeToRotation( -stepsize, 0 );
		timeToReloadSegment = true;
	}
	if(key[KEY_RIGHT]){
    	moveViewRelativeToRotation( stepsize, 0 );
		timeToReloadSegment = true;
	}
  if(key[KEY_ENTER]){
		DisplayedRotation++;
    DisplayedRotation %= 4;
		timeToReloadSegment = true;
	}
	if(key[KEY_PGDN] || key[KEY_9]){
		if (config.follow_DFscreen)
			config.viewZoffset -= stepsize;
		else
			DisplayedSegmentZ -= stepsize;
   		 if(DisplayedSegmentZ<0) DisplayedSegmentZ = 0;
		timeToReloadSegment = true;
	}
	if(key[KEY_PGUP] || key[KEY_0]){
		if (config.follow_DFscreen)
			config.viewZoffset += stepsize;
		else
			DisplayedSegmentZ += stepsize;
		timeToReloadSegment = true;
	}
	if(key[KEY_R]){
		timeToReloadSegment = true;
	}
	if(key[KEY_D]){
		paintboard();
	}
  if(key[KEY_G]){
    LoadCreatureConfiguration( &creatureTypes );
    LoadBuildingConfiguration( &buildingTypes );
    LoadGroundMaterialConfiguration();
		destroyGraphics();
    loadGraphicsFromDisk();

		timeToReloadSegment = true;
	}
  if(key[KEY_U]){
		config.show_stockpiles = !config.show_stockpiles;
    timeToReloadSegment = true;
		while(key[KEY_U]);
	}
  if(key[KEY_I]){
		config.show_zones = !config.show_zones;
    timeToReloadSegment = true;
		while(key[KEY_I]);
	}
  if(key[KEY_C]){
		config.truncate_walls = !config.truncate_walls;
    timeToReloadSegment = true;
	}
	if(key[KEY_F]){
		config.follow_DFscreen = !config.follow_DFscreen;
		timeToReloadSegment = true;
	}
  if(key[KEY_1]){
    config.segmentSize.z--;
    if(config.segmentSize.z <= 0) config.segmentSize.z = 1;
    timeToReloadSegment = true;
	}
  if(key[KEY_2]){
    config.segmentSize.z++;
    //add a limit?
    timeToReloadSegment = true;
	}
  if(key[KEY_S]){
		config.single_layer_view = !config.single_layer_view;
    timeToReloadSegment = true;
		while(key[KEY_S]);
	}
  if(key[KEY_B]){
    config.shade_hidden_blocks = !config.shade_hidden_blocks;
    timeToReloadSegment = true;
	}
  if(key[KEY_H]){
    config.show_hidden_blocks = !config.show_hidden_blocks;
    timeToReloadSegment = true;
	}
  if(key[KEY_N]){
    config.show_creature_names = !config.show_creature_names;
    timeToReloadSegment = true;
	}
  if(key[KEY_F2]){
	config.show_osd = !config.show_osd;
	timeToReloadSegment = true;
	}
  if(key[KEY_F5]){
    while(key[KEY_F5]);
    saveScreenshot();
  } 
  if(key[KEY_PLUS_PAD]){
    config.automatic_reload_time += config.automatic_reload_step;
    paintboard();
    install_int( automaticReloadProc, config.automatic_reload_time );
  }
  if(key[KEY_MINUS_PAD]){
    config.automatic_reload_time -= config.automatic_reload_step;
    if( config.automatic_reload_time <= 0 ){
      config.automatic_reload_time = 0;
      remove_int( automaticReloadProc );
    }
    else
      install_int( automaticReloadProc, config.automatic_reload_time );
    paintboard();
  }

  if(config.debug_mode){
    if(key[KEY_8_PAD]){
      debugCursor.y--;
      rest(60);
		  paintboard();
	  }
	  if(key[KEY_2_PAD]){
		  debugCursor.y++;
      rest(60);
		  paintboard();
	  }
	  if(key[KEY_4_PAD]){
		  debugCursor.x--;
      rest(60);
		  paintboard();
	  }
	  if(key[KEY_6_PAD]){
		  debugCursor.x++;
      rest(60);
		  paintboard();
	  }

    if(key[KEY_F10]){
      while(key[KEY_F10]);
      DoSpriteIndexOverlay();
    }
  }
}
