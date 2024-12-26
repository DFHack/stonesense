#include <iostream>
#include <fstream>
#include <string>
#include "common.h"
#include "commonTypes.h"
#include "Config.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

string parseStrFromLine( string keyword, string line )
{
    string retVal = "";
    string trimString = "";
    trimString += "[";
    trimString += keyword;
    trimString += ":";
    int length = (int)trimString.length();


    if( line.compare(0,length, trimString) == 0) {
        line.replace(0,length,"");
        line.replace(line.length()-1,1,"");
        retVal = line;
    }

    return retVal;
}

int parseIntFromLine( string keyword, string line )
{
    int retVal = 0;
    string trimString = "";
    trimString += "[";
    trimString += keyword;
    trimString += ":";
    int length = (int)trimString.length();


    if( line.compare(0,length, trimString) == 0) {
        line.replace(0,length,"");
        line.replace(line.length()-1,1,"");
        retVal = atoi( line.c_str() );
    }

    return retVal;
}


void parseConfigLine( string line )
{
    if(line.empty()) {
        return;
    }
    char c = line[0];
    if( c != '[') {
        return;
    }

    //some systems don't remove the \r char as a part of the line change:
    if(line.size() > 0 &&  line[line.size() -1 ] == '\r' ) {
        line.resize(line.size() -1);
    }

    c = line[ line.length() -1 ];
    if( c != ']' ) {
        return;
    }

    if( line.find("[WIDTH") != string::npos) {
        int width = parseIntFromLine( "WIDTH", line );
        ssState.ScreenW = width;
    }
    if( line.find("[HEIGHT") != string::npos) {
        int height = parseIntFromLine( "HEIGHT", line );
        ssState.ScreenH = height;
    }
    if( line.find("[WINDOWED") != string::npos) {
        string result = parseStrFromLine( "WINDOWED", line );
        ssConfig.Fullscreen = (result == "NO");
    }
    if (line.find("[SEGMENTSIZE_X") != string::npos) {
        int value = parseIntFromLine("SEGMENTSIZE_X", line);
        if (value < 1) {
            value = DEFAULT_SIZE;
        }
        if (value > 100) {
            value = 100;
        }
        //plus 2 to allow edge readings
        ssState.Size.x = value + 2;
    }
    if (line.find("[SEGMENTSIZE_Y") != string::npos) {
        int value = parseIntFromLine("SEGMENTSIZE_Y", line);
        if (value < 1) {
            value = DEFAULT_SIZE;
        }
        if (value > 100) {
            value = 100;
        }
        //plus 2 to allow edge readings
        ssState.Size.y = value + 2;
    }
    if( line.find("[SEGMENTSIZE_Z") != string::npos) {
        int value = parseIntFromLine( "SEGMENTSIZE_Z", line );
        if(value < 1) {
            value = DEFAULT_SIZE_Z;
        }
        ssState.Size.z = value;
    }

    if( line.find("[ALLCREATURES") != string::npos) {
        string result = parseStrFromLine( "ALLCREATURES", line );
        ssConfig.show_all_creatures = (result == "YES");
    }
    if( line.find("[AUTO_RELOAD_STEP") != string::npos) {
        int value = parseIntFromLine( "AUTO_RELOAD_STEP", line);
        if(value < 50) {
            value = 50;
        }
        ssConfig.automatic_reload_step = value;
    }
    if( line.find("[AUTO_RELOAD_TIME") != string::npos) {
        int value = parseIntFromLine( "AUTO_RELOAD_TIME", line);
        if(value < 0) {
            value = 0;
        }
        ssConfig.automatic_reload_time = value;
    }
    if( line.find("[DEBUGMODE") != string::npos) {
        string result = parseStrFromLine( "DEBUGMODE", line );
        ssConfig.debug_mode = (result == "YES");
    }
    if( line.find("[TRANSPARENT_SCREENSHOTS") != string::npos) {
        string result = parseStrFromLine( "TRANSPARENT_SCREENSHOTS", line );
        ssConfig.transparentScreenshots = (result == "YES");
    }
    if( line.find("[LIFTSEGMENT") != string::npos) {
        int value = parseIntFromLine( "LIFTSEGMENT", line);
        ssConfig.lift_segment_offscreen_y = value;
    }
    if( line.find("[ANIMATION_RATE") != string::npos) {
        int value = parseIntFromLine( "ANIMATION_RATE", line );
        if(value < 50) {
            value = 50;
        }
        ssConfig.animation_step = value;
    }
    if( line.find("[VERBOSE_LOGGING") != string::npos) {
        string result = parseStrFromLine( "VERBOSE_LOGGING", line );
        ssConfig.verbose_logging = (result == "YES");
    }
    if( line.find("[TRACK_MODE") != string::npos) {
        string result = parseStrFromLine( "TRACK_MODE", line );
        if(result == "CENTER") {
            ssConfig.track_mode = GameConfiguration::TRACKING_CENTER;
        } else if (result == "NONE") {
            ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
        }  else if (result == "FOCUS") {
            ssConfig.track_mode = GameConfiguration::TRACKING_FOCUS;
        }
    }
    if( line.find("[INVERT_MOUSE_Z") != string::npos) {
        string result = parseStrFromLine( "INVERT_MOUSE_Z", line );
        ssConfig.invert_mouse_z = (result == "YES");
    }
    if( line.find("[FOLLOW_DF_CURSOR") != string::npos) {
        string result = parseStrFromLine( "FOLLOW_DF_CURSOR", line );
        ssConfig.follow_DFcursor = (result == "YES");
    }
    if( line.find("[SHOW_CREATURE_NAMES") != string::npos) {
        string result = parseStrFromLine( "SHOW_CREATURE_NAMES", line );
        ssConfig.show_creature_names = (result == "YES");
    }
    if( line.find("[NAMES_USE_NICKNAME") != string::npos) {
        string result = parseStrFromLine( "NAMES_USE_NICKNAME", line );
        ssConfig.names_use_nick = (result == "YES");
    }
    if( line.find("[NAMES_USE_SPECIES") != string::npos) {
        string result = parseStrFromLine( "NAMES_USE_SPECIES", line );
        ssConfig.names_use_species = (result == "YES");
    }
    if( line.find("[SHOW_OSD") != string::npos) {
        string result = parseStrFromLine( "SHOW_OSD", line );
        ssConfig.show_osd = (result == "YES");
    }
    if( line.find("[CACHE_IMAGES") != string::npos) {
        string result = parseStrFromLine( "CACHE_IMAGES", line );
        ssConfig.cache_images = (result == "YES");
    }
    if( line.find("[SHOW_STOCKPILES") != string::npos) {
        string result = parseStrFromLine( "SHOW_STOCKPILES", line );
        ssConfig.show_stockpiles = (result == "YES");
    }
    if( line.find("[SHOW_ZONES") != string::npos) {
        string result = parseStrFromLine( "SHOW_ZONES", line );
        ssConfig.show_zones = (result == "YES");
    }
    if( line.find("[INTRO") != string::npos) {
        string result = parseStrFromLine( "INTRO", line );
        ssConfig.show_intro = !(result == "OFF");
    }
    if( line.find("[FOG_RED") != string::npos) {
        int value = parseIntFromLine( "FOG_RED", line);
        ssConfig.fogcol.r = value / 255.0;
    }
    if( line.find("[FOG_GREEN") != string::npos) {
        int value = parseIntFromLine( "FOG_GREEN", line);
        ssConfig.fogcol.g = value / 255.0;
    }
    if( line.find("[FOG_BLUE") != string::npos) {
        int value = parseIntFromLine( "FOG_BLUE", line);
        ssConfig.fogcol.b = value / 255.0;
    }
    if( line.find("[FOG_ALPHA") != string::npos) {
        int value = parseIntFromLine( "FOG_ALPHA", line);
        ssConfig.fogcol.a = value / 255.0;
    }
    if( line.find("[SHOW_FOG") != string::npos) {
        string result = parseStrFromLine( "SHOW_FOG", line );
        ssConfig.fogenable = (result == "YES");
    }
    if( line.find("[BACK_RED") != string::npos) {
        int value = parseIntFromLine( "BACK_RED", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.backcol.r = value / 255.0;
    }
    if( line.find("[BACK_GREEN") != string::npos) {
        int value = parseIntFromLine( "BACK_GREEN", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.backcol.g = value / 255.0;
    }
    if( line.find("[BACK_BLUE") != string::npos) {
        int value = parseIntFromLine( "BACK_BLUE", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.backcol.b = value / 255.0;
    }
    if( line.find("[FOLLOW_OFFSET_X") != string::npos) {
        int value = parseIntFromLine( "FOLLOW_OFFSET_X", line);
        if(value > 30) {
            value = 30;
        }
        if(value < -30) {
            value = -30;
        }
        ssConfig.viewXoffset = value;
    }
    if( line.find("[FOLLOW_OFFSET_Y") != string::npos) {
        int value = parseIntFromLine( "FOLLOW_OFFSET_Y", line);
        if(value > 30) {
            value = 30;
        }
        if(value < -30) {
            value = -30;
        }
        ssConfig.viewYoffset = value;
    }
    if( line.find("[FOLLOW_OFFSET_Z") != string::npos) {
        int value = parseIntFromLine( "FOLLOW_OFFSET_Z", line);
        if(value > 30) {
            value = 30;
        }
        if(value < -30) {
            value = -30;
        }
        ssConfig.viewZoffset = value;
    }
    if( line.find("[BITMAP_HOLDS") != string::npos) {
        int value = parseIntFromLine( "BITMAP_HOLDS", line);
        if(value < 1) {
            value = 1;
        }
        ssConfig.bitmapHolds = value;
    }
    if( line.find("[LOG_IMAGECACHE") != string::npos) {
        string result = parseStrFromLine( "LOG_IMAGECACHE", line );
        ssConfig.saveImageCache = (result == "YES");
    }
    if( line.find("[IMAGE_CACHE_SIZE") != string::npos) {
        int value = parseIntFromLine( "IMAGE_CACHE_SIZE", line);
        ssConfig.imageCacheSize = value;
    }
    if( line.find("[FONTSIZE") != string::npos) {
        int value = parseIntFromLine( "FONTSIZE", line);
        if(value < 1) {
            value = 1;
        }
        ssConfig.fontsize = value;
    }
    if( line.find("[FONT") != string::npos) {
        string result = parseStrFromLine( "FONT", line );
        ssConfig.font = al_create_path(result.c_str());
    }
    if (line.find("[USE_DF_COLORS") != string::npos){
        string result = parseStrFromLine("USE_DF_COLORS", line);
        ssConfig.useDfColors = (result == "YES");
    }
    if( line.find("[BLACK_R") != string::npos) {
        int value = parseIntFromLine( "BLACK_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::black].red = value;
    }
    if( line.find("[BLACK_G") != string::npos) {
        int value = parseIntFromLine( "BLACK_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::black].green = value;
    }
    if( line.find("[BLACK_B") != string::npos) {
        int value = parseIntFromLine( "BLACK_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::black].blue = value;
    }
    if( line.find("[BLUE_R") != string::npos) {
        int value = parseIntFromLine( "BLUE_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::blue].red = value;
    }
    if( line.find("[BLUE_G") != string::npos) {
        int value = parseIntFromLine( "BLUE_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::blue].green = value;
    }
    if( line.find("[BLUE_B") != string::npos) {
        int value = parseIntFromLine( "BLUE_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::blue].blue = value;
    }
    if( line.find("[GREEN_R") != string::npos) {
        int value = parseIntFromLine( "GREEN_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::green].red = value;
    }
    if( line.find("[GREEN_G") != string::npos) {
        int value = parseIntFromLine( "GREEN_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::green].green = value;
    }
    if( line.find("[GREEN_B") != string::npos) {
        int value = parseIntFromLine( "GREEN_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::green].blue = value;
    }
    if( line.find("[CYAN_R") != string::npos) {
        int value = parseIntFromLine( "CYAN_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::cyan].red= value;
    }
    if( line.find("[CYAN_G") != string::npos) {
        int value = parseIntFromLine( "CYAN_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::cyan].green= value;
    }
    if( line.find("[CYAN_B") != string::npos) {
        int value = parseIntFromLine( "CYAN_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::cyan].blue= value;
    }
    if( line.find("[RED_R") != string::npos) {
        int value = parseIntFromLine( "RED_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::red].red= value;
    }
    if( line.find("[RED_G") != string::npos) {
        int value = parseIntFromLine( "RED_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::red].green= value;
    }
    if( line.find("[RED_B") != string::npos) {
        int value = parseIntFromLine( "RED_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::red].blue= value;
    }
    if( line.find("[MAGENTA_R") != string::npos) {
        int value = parseIntFromLine( "MAGENTA_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::magenta].red= value;
    }
    if( line.find("[MAGENTA_G") != string::npos) {
        int value = parseIntFromLine( "MAGENTA_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::magenta].green= value;
    }
    if( line.find("[MAGENTA_B") != string::npos) {
        int value = parseIntFromLine( "MAGENTA_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::magenta].blue= value;
    }
    if( line.find("[BROWN_R") != string::npos) {
        int value = parseIntFromLine( "BROWN_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::brown].red= value;
    }
    if( line.find("[BROWN_G") != string::npos) {
        int value = parseIntFromLine( "BROWN_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::brown].green= value;
    }
    if( line.find("[BROWN_B") != string::npos) {
        int value = parseIntFromLine( "BROWN_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::brown].blue= value;
    }
    if( line.find("[LGRAY_R") != string::npos) {
        int value = parseIntFromLine( "LGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgray].red= value;
    }
    if( line.find("[LGRAY_R") != string::npos) {
        int value = parseIntFromLine( "LGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgray].green= value;
    }
    if( line.find("[LGRAY_R") != string::npos) {
        int value = parseIntFromLine( "LGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgray].blue= value;
    }
    if( line.find("[DGRAY_R") != string::npos) {
        int value = parseIntFromLine( "DGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::dgray].red= value;
    }
    if( line.find("[DGRAY_R") != string::npos) {
        int value = parseIntFromLine( "DGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::dgray].green= value;
    }
    if( line.find("[DGRAY_R") != string::npos) {
        int value = parseIntFromLine( "DGRAY_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::dgray].blue= value;
    }
    if( line.find("[LBLUE_R") != string::npos) {
        int value = parseIntFromLine( "LBLUE_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lblue].red= value;
    }
    if( line.find("[LBLUE_G") != string::npos) {
        int value = parseIntFromLine( "LBLUE_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lblue].green= value;
    }
    if( line.find("[LBLUE_B") != string::npos) {
        int value = parseIntFromLine( "LBLUE_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lblue].blue= value;
    }
    if( line.find("[LGREEN_R") != string::npos) {
        int value = parseIntFromLine( "LGREEN_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgreen].red= value;
    }
    if( line.find("[LGREEN_G") != string::npos) {
        int value = parseIntFromLine( "LGREEN_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgreen].green= value;
    }
    if( line.find("[LGREEN_B") != string::npos) {
        int value = parseIntFromLine( "LGREEN_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lgreen].blue= value;
    }
    if( line.find("[LCYAN_R") != string::npos) {
        int value = parseIntFromLine( "LCYAN_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lcyan].red= value;
    }
    if( line.find("[LCYAN_G") != string::npos) {
        int value = parseIntFromLine( "LCYAN_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lcyan].green= value;
    }
    if( line.find("[LCYAN_B") != string::npos) {
        int value = parseIntFromLine( "LCYAN_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lcyan].blue= value;
    }
    if( line.find("[LRED_R") != string::npos) {
        int value = parseIntFromLine( "LRED_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lred].red= value;
    }
    if( line.find("[LRED_G") != string::npos) {
        int value = parseIntFromLine( "LRED_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lred].green= value;
    }
    if( line.find("[LRED_B") != string::npos) {
        int value = parseIntFromLine( "LRED_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lred].blue= value;
    }
    if( line.find("[LMAGENTA_R") != string::npos) {
        int value = parseIntFromLine( "LMAGENTA_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lmagenta].red= value;
    }
    if( line.find("[LMAGENTA_G") != string::npos) {
        int value = parseIntFromLine( "LMAGENTA_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lmagenta].green= value;
    }
    if( line.find("[LMAGENTA_B") != string::npos) {
        int value = parseIntFromLine( "LMAGENTA_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::lmagenta].blue= value;
    }
    if( line.find("[YELLOW_R") != string::npos) {
        int value = parseIntFromLine( "YELLOW_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::yellow].red= value;
    }
    if( line.find("[YELLOW_G") != string::npos) {
        int value = parseIntFromLine( "YELLOW_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::yellow].green= value;
    }
    if( line.find("[YELLOW_B") != string::npos) {
        int value = parseIntFromLine( "YELLOW_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::yellow].blue= value;
    }
    if( line.find("[WHITE_R") != string::npos) {
        int value = parseIntFromLine( "WHITE_R", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::white].red= value;
    }
    if( line.find("[WHITE_G") != string::npos) {
        int value = parseIntFromLine( "WHITE_G", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::white].green= value;
    }
    if( line.find("[WHITE_B") != string::npos) {
        int value = parseIntFromLine( "WHITE_B", line);
        if(value > 255) {
            value = 255;
        }
        if(value < 0) {
            value = 0;
        }
        ssConfig.colors[dfColors::white].blue= value;
    }
    if( line.find("[RENDERER") != string::npos) {
        string result = parseStrFromLine( "RENDERER", line );
        ssConfig.opengl = (result == "OPENGL");
        ssConfig.software = (result == "SOFTWARE");
        ssConfig.directX = (result == "DIRECTX");
    }
    if( line.find("[NIGHT") != string::npos) {
        string result = parseStrFromLine( "NIGHT", line );
        ssConfig.dayNightCycle = (result == "YES");
    }
    if( line.find("[FOG_OF_WAR") != string::npos) {
        string result = parseStrFromLine( "FOG_OF_WAR", line );
        ssConfig.fog_of_war = (result == "YES");
    }
}


bool loadConfigFile()
{
    string line;
    ALLEGRO_PATH * p =al_create_path("dfhack-config/stonesense/stoneinit.txt");
    const char * path = al_path_cstr(p,ALLEGRO_NATIVE_PATH_SEP);
    ifstream myfile (path);
    if (myfile.is_open() == false) {
        LogError( "cannot find init file\n" );
        al_destroy_path(p);
        return false;
    }

    while ( !myfile.eof() ) {
        getline (myfile,line);
        parseConfigLine( line );
    }
    // update allegro colors loaded from file
    ssConfig.colors.update();
    //close file, etc.
    myfile.close();
    al_destroy_path(p);
    return true;
}
