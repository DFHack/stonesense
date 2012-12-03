#include "common.h"
#include "GUI.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include <math.h>
extern int mouse_x, mouse_y, mouse_z;
extern unsigned int mouse_b;
ALLEGRO_MOUSE_STATE mouse;
ALLEGRO_KEYBOARD_STATE board;
extern ALLEGRO_TIMER * reloadtimer;
void mouseProc(int flags)
{
    //int j = 10;
    //if(flags & MOUSE_FLAG_LEFT_DOWN){


    //}
    ////paintboard();
}

void automaticReloadProc()
{
    timeToReloadSegment = true;
}

void initAutoReload()
{
    if( ssConfig.automatic_reload_time > 0 ) {
        if(!reloadtimer) {
            reloadtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(ssConfig.automatic_reload_time));
        } else {
            al_set_timer_speed(reloadtimer, ALLEGRO_MSECS_TO_SECS(ssConfig.automatic_reload_time));
        }
        al_start_timer(reloadtimer);
    }
    //install_int( automaticReloadProc, ssConfig.automatic_reload_time );
}

void abortAutoReload()
{
    ssConfig.automatic_reload_time = 0;
    al_stop_timer(reloadtimer);
    al_destroy_timer(reloadtimer);
    //remove_int( automaticReloadProc );
}

void changeRelativeToRotation( int &inputx, int &inputy, int stepx, int stepy )
{
    switch(ssState.DisplayedRotation) {
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

    if (ssConfig.follow_DFscreen) {
        changeRelativeToRotation(ssConfig.viewXoffset, ssConfig.viewYoffset, stepx, stepy );
    }
    //if we're following the DF screen, we DO NOT bound the view, since we have a simple way to get back
    else {
        changeRelativeToRotation(ssState.DisplayedSegmentX, ssState.DisplayedSegmentY, stepx, stepy );
        //bound view to world
        if((int)ssState.DisplayedSegmentX > (int)ssConfig.cellDimX -(int)ssConfig.segmentSize.x/2) {
            ssState.DisplayedSegmentX = ssConfig.cellDimX -ssConfig.segmentSize.x/2;
        }
        if((int)ssState.DisplayedSegmentY > (int)ssConfig.cellDimY -(int)ssConfig.segmentSize.y/2) {
            ssState.DisplayedSegmentY = ssConfig.cellDimY -ssConfig.segmentSize.y/2;
        }
        if((int)ssState.DisplayedSegmentX < -(int)ssConfig.segmentSize.x/2) {
            ssState.DisplayedSegmentX = -ssConfig.segmentSize.x/2;
        }
        if((int)ssState.DisplayedSegmentY < -(int)ssConfig.segmentSize.y/2) {
            ssState.DisplayedSegmentY = -ssConfig.segmentSize.y/2;
        }
    }
}


void doKeys()
{

    al_get_keyboard_state(&keyboard);
    al_get_mouse_state(&mouse);
    char stepsize = (al_key_down(&keyboard,ALLEGRO_KEY_LSHIFT) || al_key_down(&keyboard,ALLEGRO_KEY_RSHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    //mouse_callback = mouseProc;
    static int last_mouse_z;
    if(mouse.z < last_mouse_z) {
        ssConfig.follow_DFscreen = false;
        if(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)) {
            ssConfig.segmentSize.z++;
        } else {
            if (ssConfig.follow_DFscreen) {
                ssConfig.viewZoffset -= stepsize;
            } else {
                ssState.DisplayedSegmentZ -= stepsize;
            }
            if(ssState.DisplayedSegmentZ<0) {
                ssState.DisplayedSegmentZ = 0;
            }
        }
        timeToReloadSegment = true;
        last_mouse_z = mouse.z;
    }
    if(mouse.z > last_mouse_z) {
        ssConfig.follow_DFscreen = false;
        if(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)) {
            ssConfig.segmentSize.z--;
            if(ssConfig.segmentSize.z <= 0) {
                ssConfig.segmentSize.z = 1;
            }
        } else {
            if (ssConfig.follow_DFscreen) {
                ssConfig.viewZoffset += stepsize;
            } else {
                ssState.DisplayedSegmentZ += stepsize;
            }
            if(ssState.DisplayedSegmentZ<0) {
                ssState.DisplayedSegmentZ = 0;
            }
        }
        timeToReloadSegment = true;
        last_mouse_z = mouse.z;
    }
    if( mouse.buttons & 2 ) {
        ssConfig.follow_DFscreen = false;
        int x, y;
        x = mouse.x;
        y = mouse.y;
        int blockx,blocky,blockz;
        ScreenToPoint(x,y,blockx,blocky,blockz);
        int diffx = blockx - ssConfig.segmentSize.x/2;
        int diffy = blocky - ssConfig.segmentSize.y/2;
        /*we use changeRelativeToRotation directly, and not through moveViewRelativeToRotation
        because we don't want to move the offset with the mouse. It just feels weird. */
        // changing to +1,+1 which moves the clicked point to one of the 4 surrounding the center of rotation
        changeRelativeToRotation(ssState.DisplayedSegmentX, ssState.DisplayedSegmentY, diffx+1, diffy+1 );
        //moveViewRelativeToRotation(diffx+1, diffy+1);
        timeToReloadSegment = true;
        //rest(50);
    }
    if( mouse.buttons & 1 ) {
        ssConfig.follow_DFcursor = false;
        int x, y;
        x = mouse.x;//pos >> 16;
        y = mouse.y; //pos & 0x0000ffff;
        if(x >= MiniMapTopLeftX && x <= MiniMapBottomRightX && y >= MiniMapTopLeftY && y <= MiniMapBottomRightY) { // in minimap
            ssState.DisplayedSegmentX = (x-MiniMapTopLeftX-MiniMapSegmentWidth/2)/oneBlockInPixels;
            ssState.DisplayedSegmentY = (y-MiniMapTopLeftY-MiniMapSegmentHeight/2)/oneBlockInPixels;
        } else {
            int blockx,blocky,blockz;
            ScreenToPoint(x,y,blockx,blocky,blockz);
            int diffx = blockx - ssConfig.segmentSize.x/2;
            int diffy = blocky - ssConfig.segmentSize.y/2;
            debugCursor.x = blockx;
            debugCursor.y = blocky;
        }
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_UP)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        moveViewRelativeToRotation( 0, -stepsize );
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_DOWN)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        moveViewRelativeToRotation( 0, stepsize );
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_LEFT)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        moveViewRelativeToRotation( -stepsize, 0 );
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_RIGHT)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        moveViewRelativeToRotation( stepsize, 0 );
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_PGDN) || al_key_down(&keyboard,ALLEGRO_KEY_9)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        if (ssConfig.follow_DFscreen) {
            ssConfig.viewZoffset -= stepsize;
        } else {
            ssState.DisplayedSegmentZ -= stepsize;
        }
        if(ssState.DisplayedSegmentZ<1) {
            ssState.DisplayedSegmentZ = 1;
        }
        timeToReloadSegment = true;
    }
    if(al_key_down(&keyboard,ALLEGRO_KEY_PGUP) || al_key_down(&keyboard,ALLEGRO_KEY_0)) {
        if (!(al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))) {
            ssConfig.follow_DFscreen = false;
        }
        if (ssConfig.follow_DFscreen) {
            ssConfig.viewZoffset += stepsize;
        } else {
            ssState.DisplayedSegmentZ += stepsize;
        }
        timeToReloadSegment = true;
    }
}

void doKeys(int Key)
{
    al_get_keyboard_state(&keyboard);
    if(Key == ALLEGRO_KEY_ENTER) {
        ssState.DisplayedRotation++;
        ssState.DisplayedRotation %= 4;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_R) {
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_D) {
        paintboard();
    }
    if(Key == ALLEGRO_KEY_G) {
        //contentLoader.reload_configs();
        //timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_I) {
        ssConfig.show_stockpiles = !ssConfig.show_stockpiles;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_U) {
        ssConfig.show_zones = !ssConfig.show_zones;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_O) {
        ssConfig.occlusion = !ssConfig.occlusion;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_M) {
        ssConfig.show_creature_moods = !ssConfig.show_creature_moods;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_P) {
        ssConfig.show_creature_professions = !ssConfig.show_creature_professions;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_J) {
        ssConfig.show_creature_jobs = !ssConfig.show_creature_jobs;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_C) {
        ssConfig.truncate_walls++;
        if (ssConfig.truncate_walls > 4) {
            ssConfig.truncate_walls = 0;
        }
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_F) {
        if (al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL)) {
            ssConfig.follow_DFcursor = !ssConfig.follow_DFcursor;
        } else {
            ssConfig.follow_DFscreen = !ssConfig.follow_DFscreen;
        }
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_Z) {
        if (ssConfig.follow_DFscreen) {
            ssConfig.viewXoffset = 0;
            ssConfig.viewYoffset = 0;
            ssConfig.viewZoffset = 0;
        } else {
            ssState.DisplayedSegmentX = (ssConfig.cellDimX -ssConfig.segmentSize.x)/2;
            ssState.DisplayedSegmentY = (ssConfig.cellDimY -ssConfig.segmentSize.y)/2;
        }
    }
    if(Key == ALLEGRO_KEY_1) {
        ssConfig.segmentSize.z--;
        if(ssConfig.segmentSize.z <= 0) {
            ssConfig.segmentSize.z = 1;
        }
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_2) {
        ssConfig.segmentSize.z++;
        //add a limit?
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_S) {
        ssConfig.single_layer_view = !ssConfig.single_layer_view;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_B) {
        ssConfig.shade_hidden_blocks = !ssConfig.shade_hidden_blocks;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_H) {
        ssConfig.show_hidden_blocks = !ssConfig.show_hidden_blocks;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_N) {
        ssConfig.show_creature_names = !ssConfig.show_creature_names;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_F2) {
        ssConfig.show_osd = !ssConfig.show_osd;
        timeToReloadSegment = true;
    }
    if(Key == ALLEGRO_KEY_FULLSTOP) {
        ssConfig.zoom++;
        ssConfig.scale = pow(2.0f, ssConfig.zoom);
    }
    if(Key == ALLEGRO_KEY_COMMA) {
        ssConfig.zoom--;
        ssConfig.scale = pow(2.0f, ssConfig.zoom);
    }
    if(Key == ALLEGRO_KEY_F5) {
        if (al_key_down(&keyboard,ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard,ALLEGRO_KEY_RCTRL))
            if (al_key_down(&keyboard,ALLEGRO_KEY_LSHIFT) || al_key_down(&keyboard,ALLEGRO_KEY_RSHIFT)) {
                saveMegashot(true);
            } else {
                saveMegashot(false);
            }
        else if (al_key_down(&keyboard,ALLEGRO_KEY_ALT) || al_key_down(&keyboard,ALLEGRO_KEY_ALTGR)) {
            dumpSegment();
        } else {
            saveScreenshot();
        }
    }
    if(Key == ALLEGRO_KEY_PAD_PLUS) {
        ssConfig.automatic_reload_time += ssConfig.automatic_reload_step;
        paintboard();
        initAutoReload();
    }
    if(Key == ALLEGRO_KEY_PAD_MINUS && ssConfig.automatic_reload_time) {
        if(ssConfig.automatic_reload_time > 0) {
            ssConfig.automatic_reload_time -= ssConfig.automatic_reload_step;
        }
        if( ssConfig.automatic_reload_time <= 0 ) {
            al_stop_timer(reloadtimer);
            ssConfig.automatic_reload_time = 0;
        } else {
            initAutoReload();
        }
        paintboard();
    }
    if(Key == ALLEGRO_KEY_F9) {
        ssConfig.creditScreen = false;
    }

    if(ssConfig.debug_mode) {
        if(Key == ALLEGRO_KEY_PAD_8) {
            ssConfig.follow_DFcursor = false;
            debugCursor.y--;
            paintboard();
        }
        if(Key == ALLEGRO_KEY_PAD_2) {
            ssConfig.follow_DFcursor = false;
            debugCursor.y++;
            paintboard();
        }
        if(Key == ALLEGRO_KEY_PAD_4) {
            ssConfig.follow_DFcursor = false;
            debugCursor.x--;
            paintboard();
        }
        if(Key == ALLEGRO_KEY_PAD_6) {
            ssConfig.follow_DFcursor = false;
            debugCursor.x++;
            paintboard();
        }

        if(Key == ALLEGRO_KEY_F10) {
            if(ssConfig.spriteIndexOverlay == false) {
                ssConfig.spriteIndexOverlay = true;
                ssConfig.currentSpriteOverlay = -1;
            } else {
                ssConfig.currentSpriteOverlay ++;
                if(ssConfig.currentSpriteOverlay >= IMGFilenames.size()) {
                    ssConfig.currentSpriteOverlay = -1;
                }
            }
        }
        if(Key == ALLEGRO_KEY_SPACE) {
            if(ssConfig.spriteIndexOverlay == true) {
                ssConfig.spriteIndexOverlay = false;
            }
        }
    }
}
