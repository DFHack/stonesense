#include "common.h"
#include "GUI.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GameBuildings.h"
#include "Creatures.h"
extern int mouse_x, mouse_y, mouse_z;
extern unsigned int mouse_b;
ALLEGRO_MOUSE_STATE mouse;
ALLEGRO_KEYBOARD_STATE board;
extern ALLEGRO_TIMER * reloadtimer;
void mouseProc(int flags){
	//int j = 10;
	//if(flags & MOUSE_FLAG_LEFT_DOWN){


	//}
	////paintboard();
}

void automaticReloadProc(){
	timeToReloadSegment = true;
}

void initAutoReload()
{
	if( config.automatic_reload_time > 0 )
	{
		if(!reloadtimer)
			reloadtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(config.automatic_reload_time));
		else
			al_set_timer_speed(reloadtimer, ALLEGRO_MSECS_TO_SECS(config.automatic_reload_time));
		al_start_timer(reloadtimer);
	}
	//install_int( automaticReloadProc, config.automatic_reload_time );
}

void abortAutoReload()
{
	config.automatic_reload_time = 0;
	al_stop_timer(reloadtimer);
	al_destroy_timer(reloadtimer);
	//remove_int( automaticReloadProc );	
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

	al_get_keyboard_state(&keyboard);
	al_get_mouse_state(&mouse);
	char stepsize = (al_key_down(&keyboard,ALLEGRO_KEY_LSHIFT) || al_key_down(&keyboard,ALLEGRO_KEY_RSHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
	//mouse_callback = mouseProc;
	static int last_mouse_z;
	if(mouse.z < last_mouse_z)
	{
		config.follow_DFscreen = false;
		if(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))
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
		last_mouse_z = mouse.z;
	}
	if(mouse.z > last_mouse_z){
		config.follow_DFscreen = false;
		if(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))
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
		last_mouse_z = mouse.z;
	}
	if( mouse.buttons & 2 ){
		config.follow_DFscreen = false;
		int x, y;
		x = mouse.x;
		y = mouse.y;
		int blockx,blocky,blockz;
		ScreenToPoint(x,y,blockx,blocky,blockz);
		int diffx = blockx - config.segmentSize.x/2;
		int diffy = blocky - config.segmentSize.y/2;
		/*we use changeRelativeToRotation directly, and not through moveViewRelativeToRotation 
		because we don't want to move the offset with the mouse. It just feels weird. */
		// changing to +1,+1 which moves the clicked point to one of the 4 surrounding the center of rotation
		changeRelativeToRotation(DisplayedSegmentX, DisplayedSegmentY, diffx+1, diffy+1 );
		//moveViewRelativeToRotation(diffx+1, diffy+1);
		timeToReloadSegment = true;
		//rest(50);
	}
	if( mouse.buttons & 1 ){
		config.follow_DFcursor = false;
		int x, y;
		x = mouse.x;//pos >> 16;
		y = mouse.y; //pos & 0x0000ffff;
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
	if(al_key_down(&keyboard,ALLEGRO_KEY_UP)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		moveViewRelativeToRotation( 0, -stepsize );
		timeToReloadSegment = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_DOWN)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		moveViewRelativeToRotation( 0, stepsize );
		timeToReloadSegment = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_LEFT)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		moveViewRelativeToRotation( -stepsize, 0 );
		timeToReloadSegment = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_RIGHT)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		moveViewRelativeToRotation( stepsize, 0 );
		timeToReloadSegment = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_PGDN) || al_key_down(&keyboard,ALLEGRO_KEY_9)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		if (config.follow_DFscreen)
			config.viewZoffset -= stepsize;
		else
			DisplayedSegmentZ -= stepsize;
		if(DisplayedSegmentZ<1) DisplayedSegmentZ = 1;
		timeToReloadSegment = true;
	}
	if(al_key_down(&keyboard,ALLEGRO_KEY_PGUP) || al_key_down(&keyboard,ALLEGRO_KEY_0)){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			config.follow_DFscreen = false;
		if (config.follow_DFscreen)
			config.viewZoffset += stepsize;
		else
			DisplayedSegmentZ += stepsize;
		timeToReloadSegment = true;
	}
}

void doKeys(int Key)
{
	al_get_keyboard_state(&keyboard);
	if(Key == ALLEGRO_KEY_ENTER){
		DisplayedRotation++;
		DisplayedRotation %= 4;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_R){
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_D){
		paintboard();
	}
	if(Key == ALLEGRO_KEY_G){
		flushImgFiles();
		loadGraphicsFromDisk();
		timeToReloadConfig = true;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_U){
		config.show_stockpiles = !config.show_stockpiles;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_I){
		config.show_zones = !config.show_zones;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_M){
		config.show_creature_moods = !config.show_creature_moods;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_C){
		config.truncate_walls++;
		if (config.truncate_walls > 4) config.truncate_walls = 0;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_F){
		if (al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))
			config.follow_DFcursor = !config.follow_DFcursor;
		else
			config.follow_DFscreen = !config.follow_DFscreen;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_1){
		config.segmentSize.z--;
		if(config.segmentSize.z <= 0) config.segmentSize.z = 1;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_2){
		config.segmentSize.z++;
		//add a limit?
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_S){
		config.single_layer_view = !config.single_layer_view;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_B){
		config.shade_hidden_blocks = !config.shade_hidden_blocks;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_H){
		config.show_hidden_blocks = !config.show_hidden_blocks;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_N){
		config.show_creature_names = !config.show_creature_names;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_F2){
		config.show_osd = !config.show_osd;
		timeToReloadSegment = true;
	}
	if(Key == ALLEGRO_KEY_F5){
		if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)))
			saveScreenshot();
		else{
			saveMegashot();
		}

	} 
	if(Key == ALLEGRO_KEY_PAD_PLUS){
		config.automatic_reload_time += config.automatic_reload_step;
		paintboard();
		initAutoReload();
	}
	if(Key == ALLEGRO_KEY_PAD_MINUS && config.automatic_reload_time){
		if(config.automatic_reload_time > 0)
		{
			config.automatic_reload_time -= config.automatic_reload_step;
		}
		if( config.automatic_reload_time <= 0 ){
			al_stop_timer(reloadtimer);
			config.automatic_reload_time = 0;
		}
		else
			initAutoReload();
		paintboard();
	}
	if(Key == ALLEGRO_KEY_F9)
	{
		config.creditScreen = false;
	}

	if(config.debug_mode)
	{
		if(Key == ALLEGRO_KEY_PAD_8){
			config.follow_DFcursor = false;
			debugCursor.y--;
			paintboard();
		}
		if(Key == ALLEGRO_KEY_PAD_2){
			config.follow_DFcursor = false;
			debugCursor.y++;
			paintboard();
		}
		if(Key == ALLEGRO_KEY_PAD_4){
			config.follow_DFcursor = false;
			debugCursor.x--;
			paintboard();
		}
		if(Key == ALLEGRO_KEY_PAD_6){
			config.follow_DFcursor = false;
			debugCursor.x++;
			paintboard();
		}

		if(Key == ALLEGRO_KEY_F10)
		{
			if(config.spriteIndexOverlay == false)
			{
				config.spriteIndexOverlay = true;
				config.currentSpriteOverlay = -1;
			}
			else
			{
				config.currentSpriteOverlay ++;
				if(config.currentSpriteOverlay >= IMGFilenames.size())
					config.currentSpriteOverlay = -1;
			}
		}
		if(Key == ALLEGRO_KEY_SPACE)
		{
			if(config.spriteIndexOverlay == true)
			{
				config.spriteIndexOverlay = false;
			}
		}
	}
}
