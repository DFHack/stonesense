#include <cmath>

#include "common.h"
#include "GUI.h"
#include "BuildingConfiguration.h"
#include "ContentLoader.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "UserInput.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include <math.h>

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
    stonesenseState.timeToReloadSegment = true;
}

void initAutoReload()
{
    auto& ssConfig = stonesenseState.ssConfig;

    if( ssConfig.config.automatic_reload_time > 0 ) {
        if(!stonesenseState.reloadtimer) {
            stonesenseState.reloadtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(ssConfig.config.automatic_reload_time));
        } else {
            al_set_timer_speed(stonesenseState.reloadtimer, ALLEGRO_MSECS_TO_SECS(ssConfig.config.automatic_reload_time));
        }
        al_start_timer(stonesenseState.reloadtimer);
    }
    //install_int( automaticReloadProc, ssConfig.config.automatic_reload_time );
}

void abortAutoReload()
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.automatic_reload_time = 0;
    al_stop_timer(stonesenseState.reloadtimer);
    al_destroy_timer(stonesenseState.reloadtimer);
    //remove_int( automaticReloadProc );
}

void changeRelativeToRotation( int16_t &inputx, int16_t &inputy, int stepx, int stepy )
{
    auto& ssState = stonesenseState.ssState;

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
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    if (ssConfig.config.track_mode != Config::TRACKING_NONE) {
        changeRelativeToRotation(ssConfig.config.viewOffset.x, ssConfig.config.viewOffset.y, stepx, stepy );
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
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    al_get_mouse_state(&stonesenseState.mouse);
    al_get_keyboard_state(&stonesenseState.keyboard);
    int32_t keymod = getKeyMods(&stonesenseState.keyboard);

    // char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    //mouse_callback = mouseProc;
    static int last_mouse_z;
    auto& mouse = stonesenseState.mouse;
    if(mouse.z < last_mouse_z) {
        if(ssConfig.config.invert_mouse_z) {
            action_incrZ(keymod);
        }
        else {
            action_decrZ(keymod);
        }
        last_mouse_z = mouse.z;
    }
    if(mouse.z > last_mouse_z) {
        if(ssConfig.config.invert_mouse_z) {
            action_decrZ(keymod);
        }
        else {
            action_incrZ(keymod);
        }
        last_mouse_z = mouse.z;
    }
    if( mouse.buttons & 2 ) {
        ssConfig.config.track_mode = Config::TRACKING_NONE;
        int x, y;
        x = mouse.x;
        y = mouse.y;
        df::coord tileCoord{};
        ScreenToPoint(x,y,tileCoord);
        int diffx = tileCoord.x - ssState.Size.x/2;
        int diffy = tileCoord.y - ssState.Size.y/2;
        /*we use changeRelativeToRotation directly, and not through moveViewRelativeToRotation
        because we don't want to move the offset with the mouse. It just feels weird. */
        // changing to +1,+1 which moves the clicked point to one of the 4 surrounding the center of rotation
        changeRelativeToRotation(ssState.Position.x,ssState.Position.y, diffx+1, diffy+1 );
        //moveViewRelativeToRotation(diffx+1, diffy+1);
        stonesenseState.timeToReloadSegment = true;
        //rest(50);
    }
    if( mouse.buttons & 1 ) {
        ssConfig.config.follow_DFcursor = false;
        int x, y;
        x = mouse.x;//pos >> 16;
        y = mouse.y; //pos & 0x0000ffff;
        if(x >= stonesenseState.MiniMapTopLeftX &&
            x <= stonesenseState.MiniMapBottomRightX &&
            y >= stonesenseState.MiniMapTopLeftY &&
            y <= stonesenseState.MiniMapBottomRightY) { // in minimap
            ssState.Position.x = (x- stonesenseState.MiniMapTopLeftX- stonesenseState.MiniMapSegmentWidth/2)/ stonesenseState.oneTileInPixels;
            ssState.Position.y = (y- stonesenseState.MiniMapTopLeftY- stonesenseState.MiniMapSegmentHeight/2)/ stonesenseState.oneTileInPixels;
        }
        else {
            df::coord tileCoord{};

            //get the point in the segment
            ScreenToPoint(x, y, tileCoord);

            //then remove the segment rotation

            correctForRotation(tileCoord.x,tileCoord.y,
                (4 - ssState.Rotation) % 4,
                ssState.Size.x, ssState.Size.y);

            //Convert to zero as top layer convention
            tileCoord.z = tileCoord.z - (ssState.Size.z-2);

            //add on the segment offset
            tileCoord.x = tileCoord.x + ssState.Position.x;
            tileCoord.y = tileCoord.y + ssState.Position.y;
            tileCoord.z = tileCoord.z + ssState.Position.z - 1;

            ssState.dfCursor.x = tileCoord.x;
            ssState.dfCursor.y = tileCoord.y;
            ssState.dfCursor.z = tileCoord.z;
        }
        stonesenseState.timeToReloadSegment = true;
    }
}

void action_incrrotation(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;
    ssState.Rotation++;
    ssState.Rotation %= 4;
    stonesenseState.timeToReloadSegment = true;
}

void action_reloadsegment(uint32_t keymod)
{
    stonesenseState.timeToReloadSegment = true;
}

void action_paintboard(uint32_t keymod)
{
    paintboard();
}

void action_togglestockpiles(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_stockpiles = !ssConfig.config.show_stockpiles;
    stonesenseState.timeToReloadSegment = true;
}

void action_toggledesignations(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.show_designations = !ssConfig.show_designations;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglezones(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_zones = !ssConfig.config.show_zones;
    stonesenseState.timeToReloadSegment = true;
}

void action_toggleocclusion(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.occlusion = !ssConfig.occlusion;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglefog(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.fogenable = !ssConfig.config.fogenable;
}

void action_togglecreaturemood(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_creature_moods = !ssConfig.config.show_creature_moods;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglecreatureprof(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_creature_professions++;
    ssConfig.config.show_creature_professions = ssConfig.config.show_creature_professions % 4;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglecreaturejob(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_creature_jobs = !ssConfig.config.show_creature_jobs;
    stonesenseState.timeToReloadSegment = true;
}

void action_chopwall(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.truncate_walls++;
    if (ssConfig.truncate_walls > 4) {
        ssConfig.truncate_walls = 0;
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_cycletrackingmode(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    if (keymod & ALLEGRO_KEYMOD_CTRL) {
        ssConfig.config.follow_DFcursor = !ssConfig.config.follow_DFcursor;
    }
    else {
        ssConfig.config.track_mode = (Config::trackingmode)(ssConfig.config.track_mode + 1);
        if (ssConfig.config.track_mode > Config::TRACKING_CENTER) {
            ssConfig.config.track_mode = Config::TRACKING_NONE;
        }
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_resetscreen(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    if (ssConfig.config.track_mode != Config::TRACKING_NONE) {
        ssConfig.config.viewOffset.x = 0;
        ssConfig.config.viewOffset.y = 0;
        ssConfig.config.viewOffset.z = 0;
    } else {
        ssState.Position.x = (ssState.RegionDim.x -ssState.Size.x)/2;
        ssState.Position.y = (ssState.RegionDim.y -ssState.Size.y)/2;
    }
}

void action_decrsegmentX(uint32_t keymod)
{
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    auto& ssState = stonesenseState.ssState;

    ssState.Size.x -= stepsize;
    if(ssState.Size.x <= 0) {
        ssState.Size.x = 1;
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_incrsegmentX(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.x += stepsize;
    //add a limit?
    stonesenseState.timeToReloadSegment = true;
}

void action_decrsegmentY(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.y -= stepsize;
    if(ssState.Size.y <= 0) {
        ssState.Size.y = 1;
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_incrsegmentY(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.y += stepsize;
    //add a limit?
    stonesenseState.timeToReloadSegment = true;
}

void action_decrsegmentZ(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.z -= stepsize;
    if(ssState.Size.z <= 0) {
        ssState.Size.z = 1;
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_incrsegmentZ(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    ssState.Size.z += stepsize;
    //add a limit?
    stonesenseState.timeToReloadSegment = true;
}

void action_togglesinglelayer(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.single_layer_view = !ssConfig.single_layer_view;
    stonesenseState.timeToReloadSegment = true;
}

void action_toggleshadehidden(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.shade_hidden_tiles = !ssConfig.shade_hidden_tiles;
    stonesenseState.timeToReloadSegment = true;
}

void action_toggleshowhidden(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.show_hidden_tiles = !ssConfig.show_hidden_tiles;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglecreaturenames(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_creature_names = !ssConfig.config.show_creature_names;
    stonesenseState.timeToReloadSegment = true;
}

void action_toggleosd(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ssConfig.config.show_osd = !ssConfig.config.show_osd;
    stonesenseState.timeToReloadSegment = true;
}

void action_togglekeybinds(uint32_t keymod){
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.show_keybinds = !ssConfig.show_keybinds;
}

void action_toggleannouncements(uint32_t keymod) {
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.show_announcements = !ssConfig.show_announcements;
}

void action_toggledebug(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.config.debug_mode = !ssConfig.config.debug_mode;
}

void action_incrzoom(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.zoom++;
    ssConfig.recalculateScale();
}

void action_decrzoom(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.zoom--;
    ssConfig.recalculateScale();
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
    auto& ssConfig = stonesenseState.ssConfig;
    ssConfig.config.automatic_reload_time += ssConfig.config.automatic_reload_step;
    paintboard();
    initAutoReload();
}

void action_decrreloadtime(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
    if(ssConfig.config.automatic_reload_time) {
        if(ssConfig.config.automatic_reload_time > 0) {
            ssConfig.config.automatic_reload_time -= ssConfig.config.automatic_reload_step;
        }
        if( ssConfig.config.automatic_reload_time <= 0 ) {
            al_stop_timer(stonesenseState.reloadtimer);
            ssConfig.config.automatic_reload_time = 0;
        } else {
            initAutoReload();
        }
        paintboard();
    }
}

void action_credits(uint32_t keymod)
{
    auto& ssConfig = stonesenseState.ssConfig;
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
        stonesenseState.ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    moveViewRelativeToRotation( 0, -stepsize );
    stonesenseState.timeToReloadSegment = true;
}

void action_incrY(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentY(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        stonesenseState.ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    moveViewRelativeToRotation( 0, stepsize );
    stonesenseState.timeToReloadSegment = true;
}

void action_decrX(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_decrsegmentX(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        stonesenseState.ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    moveViewRelativeToRotation( -stepsize, 0 );
    stonesenseState.timeToReloadSegment = true;
}

void action_incrX(uint32_t keymod)
{
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentX(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        stonesenseState.ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    moveViewRelativeToRotation( stepsize, 0 );
    stonesenseState.timeToReloadSegment = true;
}

void action_decrZ(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    auto& ssConfig = stonesenseState.ssConfig;
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_decrsegmentZ(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    if (ssConfig.config.track_mode != Config::TRACKING_NONE) {
        ssConfig.config.viewOffset.z -= stepsize;
    } else {
        ssState.Position.z -= stepsize;
    }
    if(ssState.Position.z<1) {
        ssState.Position.z = 1;
    }
    stonesenseState.timeToReloadSegment = true;
}

void action_incrZ(uint32_t keymod)
{
    auto& ssState = stonesenseState.ssState;

    auto& ssConfig = stonesenseState.ssConfig;
    if(keymod&ALLEGRO_KEYMOD_CTRL){
        action_incrsegmentZ(keymod);
        return;
    }
    char stepsize = ((keymod&ALLEGRO_KEYMOD_SHIFT) ? MAPNAVIGATIONSTEPBIG : MAPNAVIGATIONSTEP);
    if (!(keymod&ALLEGRO_KEYMOD_ALT)) {
        ssConfig.config.track_mode = Config::TRACKING_NONE;
    }
    if (ssConfig.config.track_mode != Config::TRACKING_NONE) {
        ssConfig.config.viewOffset.z += stepsize;
    } else {
        ssState.Position.z += stepsize;
    }
    stonesenseState.timeToReloadSegment = true;
}

void doRepeatActions()
{
    al_get_keyboard_state(&stonesenseState.keyboard);
    int32_t keymod = getKeyMods(&stonesenseState.keyboard);

    for(int keycode=0; keycode<ALLEGRO_KEY_UNKNOWN; keycode++) {
        if(isRepeatable(keycode) && al_key_down(&stonesenseState.keyboard,keycode)) {
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
    //if(ssConfig.config.debug_mode) {
    //    if(Key == ALLEGRO_KEY_PAD_8) {
    //        ssConfig.config.follow_DFcursor = false;
    //        debugCursor.y--;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_2) {
    //        ssConfig.config.follow_DFcursor = false;
    //        debugCursor.y++;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_4) {
    //        ssConfig.config.follow_DFcursor = false;
    //        debugCursor.x--;
    //        paintboard();
    //    }
    //    if(Key == ALLEGRO_KEY_PAD_6) {
    //        ssConfig.config.follow_DFcursor = false;
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
