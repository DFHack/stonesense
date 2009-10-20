#include "common.h"
#include "GUI.h"


void mouseProc(int flags){
  int j = 10;
  if(flags & MOUSE_FLAG_LEFT_DOWN){
    
    
  }
  //paintboard();
}

void automaticReloadProc(){
		timeToReloadSegment = true;
}


void doKeys(){
  //mouse_callback = mouseProc;

  char stepsize = (key[KEY_LSHIFT] || key[KEY_RSHIFT] ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
  if(key[KEY_UP]){
		DisplayedSegmentY-=stepsize;
		timeToReloadSegment = true;
	}
	if(key[KEY_DOWN]){
		DisplayedSegmentY+=stepsize;
		timeToReloadSegment = true;
	}
	if(key[KEY_LEFT]){
		DisplayedSegmentX-=stepsize;
		timeToReloadSegment = true;
	}
	if(key[KEY_RIGHT]){
		DisplayedSegmentX+=stepsize;
		timeToReloadSegment = true;
	}
	if(key[KEY_PGDN] || key[KEY_9]){
		DisplayedSegmentZ--;
		timeToReloadSegment = true;
	}
	if(key[KEY_PGUP] || key[KEY_0]){
		DisplayedSegmentZ++;
		timeToReloadSegment = true;
	}
	if(key[KEY_R]){
		timeToReloadSegment = true;
	}
	if(key[KEY_D]){
		paintboard();
	}
  if(key[KEY_G]){
		destroyGraphics();
    loadGraphicsFromDisk();
		paintboard();
		while(key[KEY_G]);
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
  if(key[KEY_S]){
		config.single_layer_view = !config.single_layer_view;
    timeToReloadSegment = true;
		while(key[KEY_S]);
	}
  if(key[KEY_B]){
    config.shade_hidden_blocks = !config.shade_hidden_blocks;
    timeToReloadSegment = true;
	}
  if(key[KEY_F5]){
    while(key[KEY_F5]);
    saveScreenshot();
  }
  if(key[KEY_PLUS_PAD]){
    config.automatic_reload_time += 500;
    paintboard();
    install_int( automaticReloadProc, config.automatic_reload_time );
    while(key[KEY_PLUS_PAD]);
  }
  if(key[KEY_MINUS_PAD]){
    config.automatic_reload_time -= 500;
    paintboard();
    if( config.automatic_reload_time <= 0 ){
      config.automatic_reload_time = 0;
      remove_int( automaticReloadProc );
    }
    else
      install_int( automaticReloadProc, config.automatic_reload_time );
    while(key[KEY_MINUS_PAD]);
  }

}