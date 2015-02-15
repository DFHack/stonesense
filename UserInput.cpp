#include "common.h"
#include "GUI.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "UserInput.h"
#include <math.h>

extern int mouse_x, mouse_y, mouse_z;
extern unsigned int mouse_b;
ALLEGRO_MOUSE_STATE mouse;
ALLEGRO_KEYBOARD_STATE board;
extern ALLEGRO_TIMER * reloadtimer;

/**
 * Returns a keymod bitflag consistent with allegro keycodes for the CTRL, ALT, and SHIFT keys
 * from a given keyboard state.
 */
int32_t getKeyMods(ALLEGRO_KEYBOARD_STATE * keyboardstate){
    int32_t keymod = 0;
    if(al_key_down(keyboardstate,ALLEGRO_KEY_LSHIFT) || al_key_down(keyboardstate,ALLEGRO_KEY_RSHIFT)){
        keymod |= ALLEGRO_KEYMOD_SHIFT;
    }
    if(al_key_down(keyboardstate,ALLEGRO_KEY_LCTRL) || al_key_down(keyboardstate,ALLEGRO_KEY_RCTRL)){
        keymod |= ALLEGRO_KEYMOD_CTRL;
    }
    if(al_key_down(keyboardstate,ALLEGRO_KEY_ALT)){
        keymod |= ALLEGRO_KEYMOD_ALT;
    }
    return keymod;
}

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
    switch(ssState.Rotation) {
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

    if (ssConfig.track_mode != GameConfiguration::TRACKING_NONE) {
        changeRelativeToRotation(ssConfig.viewXoffset, ssConfig.viewYoffset, stepx, stepy );
    }
    //if we're following the DF screen, we DO NOT bound the view, since we have a simple way to get back
    else {
        changeRelativeToRotation(ssState.Position.x, ssState.Position.y, stepx, stepy );
        //bound view to world
        if((int)ssState.Position.x > (int)ssState.RegionDim.x -(int)ssState.Size.x/2) {
            ssState.Position.x = ssState.RegionDim.x -ssState.Size.x/2;
        }
        if((int)ssState.Position.y > (int)ssState.RegionDim.y -(int)ssState.Size.y/2) {
            ssState.Position.y = ssState.RegionDim.y -ssState.Size.y/2;
        }
        if((int)ssState.Position.x < -(int)ssState.Size.x/2) {
            ssState.Position.x = -ssState.Size.x/2;
        }
        if((int)ssState.Position.y < -(int)ssState.Size.y/2) {
            ssState.Position.y = -ssState.Size.y/2;
        }
    }
}


void doMouse()
{
    al_get_mouse_state(&mouse);
    al_get_keyboard_state(&keyboard);
    int32_t keymod = getKeyMods(&keyboard);

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    //mouse_callback = mouseProc;
    static int last_mouse_z;
    if(mouse.z < last_mouse_z) {
        action_decrZ(keymod);
        last_mouse_z = mouse.z;
    }
    if(mouse.z > last_mouse_z) {
        action_incrZ(keymod);
        last_mouse_z = mouse.z;
    }
    if( mouse.buttons & 2 ) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
        int x, y;
        x = mouse.x;
        y = mouse.y;
        int tilex,tiley,tilez;
        ScreenToPoint(x,y,tilex,tiley,tilez);
        int diffx = tilex - ssState.Size.x/2;
        int diffy = tiley - ssState.Size.y/2;
        /*we use changeRelativeToRotation directly, and not through moveViewRelativeToRotation
        because we don't want to move the offset with the mouse. It just feels weird. */
        // changing to +1,+1 which moves the clicked point to one of the 4 surrounding the center of rotation
        changeRelativeToRotation(ssState.Position.x, ssState.Position.y, diffx+1, diffy+1 );
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
            ssState.Position.x = (x-MiniMapTopLeftX-MiniMapSegmentWidth/2)/oneTileInPixels;
            ssState.Position.y = (y-MiniMapTopLeftY-MiniMapSegmentHeight/2)/oneTileInPixels;
        } else {
            int tilex,tiley,tilez;

            //get the point in the segment
            ScreenToPoint(x,y,tilex,tiley,tilez);

            //then remove the segment rotation
            correctForRotation(
                tilex, tiley,
                (4 - ssState.Rotation) % 4,
                ssState.Size.x, ssState.Size.y);

            //Convert to zero as top layer convention
            tilez = tilez - (ssState.Size.z-2);

            //add on the segment offset
            tilex = tilex + ssState.Position.x;
            tiley = tiley + ssState.Position.y;
            tilez = tilez + ssState.Position.z - 1;

            ssState.dfCursor.x = tilex;
            ssState.dfCursor.y = tiley;
            ssState.dfCursor.z = tilez;
        }
        timeToReloadSegment = true;
    }
}

void action_incrrotation(uint32_t keymod)
{
    ssState.Rotation++;
    ssState.Rotation %= 4;
    timeToReloadSegment = true;
}

void action_reloadsegment(uint32_t keymod)
{
    timeToReloadSegment = true;
}

void action_paintboard(uint32_t keymod)
{
    paintboard();
}

void action_togglestockpiles(uint32_t keymod)
{
    ssConfig.show_stockpiles = !ssConfig.show_stockpiles;
    timeToReloadSegment = true;
}

void action_toggledesignations(uint32_t keymod)
{
    ssConfig.show_designations = !ssConfig.show_designations;
    timeToReloadSegment = true;
}

void action_togglezones(uint32_t keymod)
{
    ssConfig.show_zones = !ssConfig.show_zones;
    timeToReloadSegment = true;
}

void action_toggleocclusion(uint32_t keymod)
{
    ssConfig.occlusion = !ssConfig.occlusion;
    timeToReloadSegment = true;
}

void action_togglecreaturemood(uint32_t keymod)
{
    ssConfig.show_creature_moods = !ssConfig.show_creature_moods;
    timeToReloadSegment = true;
}

void action_togglecreatureprof(uint32_t keymod)
{
    ssConfig.show_creature_professions++;
    ssConfig.show_creature_professions = ssConfig.show_creature_professions % 4;
    timeToReloadSegment = true;
}

void action_togglecreaturejob(uint32_t keymod)
{
    ssConfig.show_creature_jobs = !ssConfig.show_creature_jobs;
    timeToReloadSegment = true;
}

void action_chopwall(uint32_t keymod)
{
    ssConfig.truncate_walls++;
    if (ssConfig.truncate_walls > 4) {
        ssConfig.truncate_walls = 0;
    }
    timeToReloadSegment = true;
}

void action_cycletrackingmode(uint32_t keymod)
{
    if (keymod&ALLEGRO_KEYMOD_CTRL) {
        ssConfig.follow_DFcursor = !ssConfig.follow_DFcursor;
    } else {
        ssConfig.track_mode++;
        if(ssConfig.track_mode >= GameConfiguration::TRACKING_INVALID) {
            ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
        }
    }
    timeToReloadSegment = true;
}

void action_resetscreen(uint32_t keymod)
{
    if (ssConfig.track_mode != GameConfiguration::TRACKING_NONE) {
        ssConfig.viewXoffset = 0;
        ssConfig.viewYoffset = 0;
        ssConfig.viewZoffset = 0;
    } else {
        ssState.Position.x = (ssState.RegionDim.x -ssState.Size.x)/2;
        ssState.Position.y = (ssState.RegionDim.y -ssState.Size.y)/2;
    }
}

void action_decrsegmentX(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.x -= stepsize;
    if(ssState.Size.x <= 0) {
        ssState.Size.x = 1;
    }
    timeToReloadSegment = true;
}

void action_incrsegmentX(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.x += stepsize;
    //add a limit?
    timeToReloadSegment = true;
}

void action_decrsegmentY(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.y -= stepsize;
    if(ssState.Size.y <= 0) {
        ssState.Size.y = 1;
    }
    timeToReloadSegment = true;
}

void action_incrsegmentY(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.y += stepsize;
    //add a limit?
    timeToReloadSegment = true;
}

void action_decrsegmentZ(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.z -= stepsize;
    if(ssState.Size.z <= 0) {
        ssState.Size.z = 1;
    }
    timeToReloadSegment = true;
}

void action_incrsegmentZ(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.z += stepsize;
    //add a limit?
    timeToReloadSegment = true;
}

void action_togglesinglelayer(uint32_t keymod)
{
    ssConfig.single_layer_view = !ssConfig.single_layer_view;
    timeToReloadSegment = true;
}

void action_toggleshadehidden(uint32_t keymod)
{
    ssConfig.shade_hidden_tiles = !ssConfig.shade_hidden_tiles;
    timeToReloadSegment = true;
}

void action_toggleshowhidden(uint32_t keymod)
{
    ssConfig.show_hidden_tiles = !ssConfig.show_hidden_tiles;
    timeToReloadSegment = true;
}

void action_togglecreaturenames(uint32_t keymod)
{
    ssConfig.show_creature_names = !ssConfig.show_creature_names;
    timeToReloadSegment = true;
}

void action_toggleosd(uint32_t keymod)
{
    ssConfig.show_osd = !ssConfig.show_osd;
    timeToReloadSegment = true;
}

void action_togglekeybinds(uint32_t keymod){
    ssConfig.show_keybinds = !ssConfig.show_keybinds;
}

void action_incrzoom(uint32_t keymod)
{
    ssConfig.zoom++;
    ssConfig.scale = pow(2.0f, ssConfig.zoom);
}

void action_decrzoom(uint32_t keymod)
{
    ssConfig.zoom--;
    ssConfig.scale = pow(2.0f, ssConfig.zoom);
}

void action_screenshot(uint32_t keymod)
{
    if (keymod&ALLEGRO_KEYMOD_CTRL) {
        if (keymod&ALLEGRO_KEYMOD_SHIFT) {
            saveMegashot(true);
        } else {
            saveMegashot(false);
        }
    } else if (keymod&ALLEGRO_KEYMOD_ALT) {
        dumpSegment();
    } else {
        saveScreenshot();
    }
}

void action_incrreloadtime(uint32_t keymod)
{
    ssConfig.automatic_reload_time += ssConfig.automatic_reload_step;
    paintboard();
    initAutoReload();
}

void action_decrreloadtime(uint32_t keymod)
{
    if(ssConfig.automatic_reload_time) {
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
}

void action_credits(uint32_t keymod)
{
    ssConfig.creditScreen = false;
}

void action_decrY(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_decrsegmentY(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    moveViewRelativeToRotation( 0, -stepsize );
    timeToReloadSegment = true;
}

void action_incrY(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentY(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    moveViewRelativeToRotation( 0, stepsize );
    timeToReloadSegment = true;
}

void action_decrX(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_decrsegmentX(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    moveViewRelativeToRotation( -stepsize, 0 );
    timeToReloadSegment = true;
}

void action_incrX(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentX(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    moveViewRelativeToRotation( stepsize, 0 );
    timeToReloadSegment = true;
}

void action_decrZ(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_decrsegmentZ(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    if (ssConfig.track_mode != GameConfiguration::TRACKING_NONE) {
        ssConfig.viewZoffset -= stepsize;
    } else {
        ssState.Position.z -= stepsize;
    }
    if(ssState.Position.z<1) {
        ssState.Position.z = 1;
    }
    timeToReloadSegment = true;
}

void action_incrZ(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentZ(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    }
    if (ssConfig.track_mode != GameConfiguration::TRACKING_NONE) {
        ssConfig.viewZoffset += stepsize;
    } else {
        ssState.Position.z += stepsize;
    }
    timeToReloadSegment = true;
}

void doRepeatActions()
{
    al_get_keyboard_state(&keyboard);
    int32_t keymod = getKeyMods(&keyboard);

    for(int keycode=0; keycode<ALLEGRO_KEY_UNKNOWN; keycode++) {
        if(isRepeatable(keycode) && al_key_down(&keyboard,keycode)) {
            doKey(keycode, keymod);
        }
    }
}

void doKeys(int32_t key, uint32_t keymod)
{
    if(!isRepeatable(key)) {
        doKey(key,keymod);
    }
    return;

    //WAITING TO BE MOVED OVER
    //if(ssConfig.debug_mode) {
    //    if(Key == ALLEGRO_KEY_PAD_8) {
    //        ssConfig.follow_DFcursor = false;
    //        debugCursor.y--;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_2) {
    //        ssConfig.follow_DFcursor = false;
    //        debugCursor.y++;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_4) {
    //        ssConfig.follow_DFcursor = false;
    //        debugCursor.x--;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_6) {
    //        ssConfig.follow_DFcursor = false;
    //        debugCursor.x++;
    //        paintboard();
    //    }

    //    if(Key == ALLEGRO_KEY_F10) {
    //        if(ssConfig.spriteIndexOverlay == false) {
    //            ssConfig.spriteIndexOverlay = true;
    //            ssConfig.currentSpriteOverlay = -1;
    //        } else {
    //            ssConfig.currentSpriteOverlay ++;
    //            if(ssConfig.currentSpriteOverlay >= IMGFilenames.size()) {
    //                ssConfig.currentSpriteOverlay = -1;
    //            }
    //        }
    //    }
    //    if(Key == ALLEGRO_KEY_SPACE) {
    //        if(ssConfig.spriteIndexOverlay == true) {
    //            ssConfig.spriteIndexOverlay = false;
    //        }
    //    }
    //}
}
