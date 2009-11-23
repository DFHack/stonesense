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

void initAutoReload()
{
    if( config.automatic_reload_time > 0 )
      install_int( automaticReloadProc, config.automatic_reload_time );
}

void abortAutoReload()
{
	config.automatic_reload_time = 0;
	remove_int( automaticReloadProc );	
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
  char stepsize = (key[KEY_LSHIFT] || key[KEY_RSHIFT] ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
  //mouse_callback = mouseProc;
    static int last_mouse_z;
    if(mouse_z < last_mouse_z)
    {
        if(key[KEY_LCONTROL] || key[KEY_RCONTROL])
        {
            config.segmentSize.z++;
        }
        else
        {
            if (config.follow_DFscreen)
			    config.viewZoffset -= stepsize;
            else
			    DisplayedSegmentZ -= stepsize;
   		    if(DisplayedSegmentZ<0) DisplayedSegmentZ = 0;
        }
        timeToReloadSegment = true;
        last_mouse_z = mouse_z;
    }
    if(mouse_z > last_mouse_z){
        if(key[KEY_LCONTROL] || key[KEY_RCONTROL])
        {
            config.segmentSize.z--;
            if(config.segmentSize.z <= 0) config.segmentSize.z = 1;
        }
        else
        {
            if (config.follow_DFscreen)
			    config.viewZoffset += stepsize;
            else
			    DisplayedSegmentZ += stepsize;
   		    if(DisplayedSegmentZ<0) DisplayedSegmentZ = 0;
        }
        timeToReloadSegment = true;
        last_mouse_z = mouse_z;
    }
    if((mouse_b & 2) || ((mouse_b & 1) && !config.debug_mode )){
      int pos, x, y;
      pos = mouse_pos;
      x = pos >> 16;
      y = pos & 0x0000ffff;
      int blockx,blocky,blockz;
      ScreenToPoint(x,y,blockx,blocky,blockz);
      int diffx = blockx - config.segmentSize.x/2;
      int diffy = blocky - config.segmentSize.y/2;
      /*we use changeRelativeToRotation directly, and not through moveViewRelativeToRotation 
      because we don't want to move the offset with the mouse. It just feels weird. */
      changeRelativeToRotation(DisplayedSegmentX, DisplayedSegmentY, diffx+5, diffy+5 );
      //moveViewRelativeToRotation(diffx+5, diffy+5); // for whatever reason, the +5 makes it be centered, someone should check this works correctly in other resolutions than 800x600
      timeToReloadSegment = true;
      //rest(50);
    }
    else if (mouse_b & 1){
        int pos, x, y;
        pos = mouse_pos;
        x = mouse_x;//pos >> 16;
        y = mouse_y; //pos & 0x0000ffff;
        if(x >= MiniMapTopLeftX && x <= MiniMapBottomRightX && y >= MiniMapTopLeftY && y <= MiniMapBottomRightY){ // in minimap
            DisplayedSegmentX = (x-MiniMapTopLeftX-MiniMapSegmentWidth/2)/oneBlockInPixels;
            DisplayedSegmentY = (y-MiniMapTopLeftY-MiniMapSegmentHeight/2)/oneBlockInPixels;
        }
        else{
            int blockx,blocky,blockz;
            ScreenToPoint(x,y,blockx,blocky,blockz);
            int diffx = blockx - config.segmentSize.x/2;
            int diffy = blocky - config.segmentSize.y/2;
            debugCursor.x = blockx;
            debugCursor.y = blocky;
        }
        timeToReloadSegment = true;
    }
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
