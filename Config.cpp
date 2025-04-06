#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "common.h"
#include "commonTypes.h"
#include "Config.h"
#include "GameConfiguration.h"
#include "GameState.h"
#include "StonesenseState.h"

using std::string;

namespace {
    string parseStrFromLine(string keyword, string line)
    {
        string retVal = "";
        string trimString = "";
        trimString += "[";
        trimString += keyword;
        trimString += ":";
        int length = (int)trimString.length();


        if (line.compare(0, length, trimString) == 0) {
            line.replace(0, length, "");
            line.replace(line.length() - 1, 1, "");
            retVal = line;
        }

        return retVal;
    }

    int parseIntFromLine(string keyword, string line)
    {
        auto tmp = parseStrFromLine(keyword, line);
        return tmp.empty() ? 0 : atoi(tmp.c_str());
    }
}

std::optional<std::string> trim_line(std::string line)
{
    if (line.empty())
        return std::nullopt;

    if (!line.starts_with("["))
        return std::nullopt;

    if (line.ends_with("\r"))
        line.resize(line.size() - 1);

    if (!line.ends_with("]"))
        return std::nullopt;

    return std::optional{ line };
}

namespace {
    void parseConfigLine(Config& config, string line)
    {
        auto l = trim_line(line);
        if (!l) return;
        line = *l;

        if (line.find("[CLOSEONESC") != string::npos) {
            string result = parseStrFromLine("CLOSEONESC", line);
            config.closeOnEsc = (result == "YES");
        }

        if (line.find("[WIDTH") != string::npos) {
            int width = parseIntFromLine("WIDTH", line);
            config.defaultScreenWidth = width;
        }
        if (line.find("[HEIGHT") != string::npos) {
            int height = parseIntFromLine("HEIGHT", line);
            config.defaultScreenHeight = height;
        }
        if (line.find("[WINDOWED") != string::npos) {
            string result = parseStrFromLine("WINDOWED", line);
            config.Fullscreen = (result == "NO");
        }
        if (line.find("[SEGMENTSIZE_XY") != string::npos) {
            int value = parseIntFromLine("SEGMENTSIZE_XY", line);
            if (value < 1) {
                value = DEFAULT_SIZE;
            }
            if (value > 100) {
                value = 100;
            }
            //plus 2 to allow edge readings
            config.defaultSegmentSize.x = value;
            config.defaultSegmentSize.y = value;
        }
        if (line.find("[SEGMENTSIZE_Z") != string::npos) {
            int value = parseIntFromLine("SEGMENTSIZE_Z", line);
            if (value < 1) {
                value = DEFAULT_SIZE_Z;
            }
            config.defaultSegmentSize.z = value;
        }
        if (line.find("[ALLCREATURES") != string::npos) {
            string result = parseStrFromLine("ALLCREATURES", line);
            config.show_all_creatures = (result == "YES");
        }
        if (line.find("[AUTO_RELOAD_STEP") != string::npos) {
            int value = parseIntFromLine("AUTO_RELOAD_STEP", line);
            if (value < 50) {
                value = 50;
            }
            config.automatic_reload_step = value;
        }
        if (line.find("[AUTO_RELOAD_TIME") != string::npos) {
            int value = parseIntFromLine("AUTO_RELOAD_TIME", line);
            if (value < 0) {
                value = 0;
            }
            config.automatic_reload_time = value;
        }
        if (line.find("[DEBUGMODE") != string::npos) {
            string result = parseStrFromLine("DEBUGMODE", line);
            config.debug_mode = (result == "YES");
        }
        if (line.find("[TRANSPARENT_SCREENSHOTS") != string::npos) {
            string result = parseStrFromLine("TRANSPARENT_SCREENSHOTS", line);
            config.transparentScreenshots = (result == "YES");
        }
        if (line.find("[LIFTSEGMENT") != string::npos) {
            int value = parseIntFromLine("LIFTSEGMENT", line);
            config.lift_segment = value;
        }
        if (line.find("[ANIMATION_RATE") != string::npos) {
            int value = parseIntFromLine("ANIMATION_RATE", line);
            if (value < 50) {
                value = 50;
            }
            config.animation_step = value;
        }
        if (line.find("[VERBOSE_LOGGING") != string::npos) {
            string result = parseStrFromLine("VERBOSE_LOGGING", line);
            config.verbose_logging = (result == "YES");
        }
        if (line.find("[TRACK_MODE") != string::npos) {
            string result = parseStrFromLine("TRACK_MODE", line);
            if (result == "NONE") {
                config.track_mode = Config::TRACKING_NONE;
            }
        }
        if (line.find("[INVERT_MOUSE_Z") != string::npos) {
            string result = parseStrFromLine("INVERT_MOUSE_Z", line);
            config.invert_mouse_z = (result == "YES");
        }
        if (line.find("[FOLLOW_DF_CURSOR") != string::npos) {
            string result = parseStrFromLine("FOLLOW_DF_CURSOR", line);
            config.follow_DFcursor = (result == "YES");
        }
        if (line.find("[SHOW_CREATURE_NAMES") != string::npos) {
            string result = parseStrFromLine("SHOW_CREATURE_NAMES", line);
            config.show_creature_names = (result == "YES");
        }
        if (line.find("[SHOW_CREATURE_MOODS") != string::npos) {
            string result = parseStrFromLine("SHOW_CREATURE_MOODS", line);
            config.show_creature_moods = (result == "YES");
        }
        if (line.find("[SHOW_CREATURE_JOBS") != string::npos) {
            string result = parseStrFromLine("SHOW_CREATURE_JOBS", line);
            config.show_creature_jobs = (result == "YES");
        }
        if (line.find("[SHOW_CREATURE_PROFESSIONS") != string::npos) {
            int value = parseIntFromLine("SHOW_CREATURE_PROFESSIONS", line);
            config.show_creature_professions = value;
        }
        if (line.find("[NAMES_USE_NICKNAME") != string::npos) {
            string result = parseStrFromLine("NAMES_USE_NICKNAME", line);
            config.names_use_nick = (result == "YES");
        }
        if (line.find("[NAMES_USE_SPECIES") != string::npos) {
            string result = parseStrFromLine("NAMES_USE_SPECIES", line);
            config.names_use_species = (result == "YES");
        }
        if (line.find("[SHOW_OSD") != string::npos) {
            string result = parseStrFromLine("SHOW_OSD", line);
            config.show_osd = (result == "YES");
        }
        if (line.find("[CACHE_IMAGES") != string::npos) {
            string result = parseStrFromLine("CACHE_IMAGES", line);
            config.cache_images = (result == "YES");
        }
        if (line.find("[SHOW_STOCKPILES") != string::npos) {
            string result = parseStrFromLine("SHOW_STOCKPILES", line);
            config.show_stockpiles = (result == "YES");
        }
        if (line.find("[SHOW_ZONES") != string::npos) {
            string result = parseStrFromLine("SHOW_ZONES", line);
            config.show_zones = (result == "YES");
        }
        if (line.find("[INTRO") != string::npos) {
            string result = parseStrFromLine("INTRO", line);
            config.show_intro = !(result == "OFF");
        }
        if (line.find("[FOG_RED") != string::npos) {
            int value = parseIntFromLine("FOG_RED", line);
            config.fogcol.r = value / 255.0;
        }
        if (line.find("[FOG_GREEN") != string::npos) {
            int value = parseIntFromLine("FOG_GREEN", line);
            config.fogcol.g = value / 255.0;
        }
        if (line.find("[FOG_BLUE") != string::npos) {
            int value = parseIntFromLine("FOG_BLUE", line);
            config.fogcol.b = value / 255.0;
        }
        if (line.find("[FOG_ALPHA") != string::npos) {
            int value = parseIntFromLine("FOG_ALPHA", line);
            config.fogcol.a = value / 255.0;
        }
        if (line.find("[SHOW_FOG") != string::npos) {
            string result = parseStrFromLine("SHOW_FOG", line);
            config.fogenable = (result == "YES");
        }
        if (line.find("[BACK_RED") != string::npos) {
            int value = parseIntFromLine("BACK_RED", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.backcol.r = value / 255.0;
        }
        if (line.find("[BACK_GREEN") != string::npos) {
            int value = parseIntFromLine("BACK_GREEN", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.backcol.g = value / 255.0;
        }
        if (line.find("[BACK_BLUE") != string::npos) {
            int value = parseIntFromLine("BACK_BLUE", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.backcol.b = value / 255.0;
        }
        if (line.find("[FOLLOW_OFFSET_X") != string::npos) {
            int value = parseIntFromLine("FOLLOW_OFFSET_X", line);
            if (value > 30) {
                value = 30;
            }
            if (value < -30) {
                value = -30;
            }
            config.viewOffset.x = value;
        }
        if (line.find("[FOLLOW_OFFSET_Y") != string::npos) {
            int value = parseIntFromLine("FOLLOW_OFFSET_Y", line);
            if (value > 30) {
                value = 30;
            }
            if (value < -30) {
                value = -30;
            }
            config.viewOffset.y = value;
        }
        if (line.find("[FOLLOW_OFFSET_Z") != string::npos) {
            int value = parseIntFromLine("FOLLOW_OFFSET_Z", line);
            if (value > 30) {
                value = 30;
            }
            if (value < -30) {
                value = -30;
            }
            config.viewOffset.z = value;
        }
        if (line.find("[BITMAP_HOLDS") != string::npos) {
            int value = parseIntFromLine("BITMAP_HOLDS", line);
            if (value < 1) {
                value = 1;
            }
            config.bitmapHolds = value;
        }
        if (line.find("[LOG_IMAGECACHE") != string::npos) {
            string result = parseStrFromLine("LOG_IMAGECACHE", line);
            config.saveImageCache = (result == "YES");
        }
        if (line.find("[IMAGE_CACHE_SIZE") != string::npos) {
            int value = parseIntFromLine("IMAGE_CACHE_SIZE", line);
            config.imageCacheSize = value;
        }
        if (line.find("[FONTSIZE") != string::npos) {
            int value = parseIntFromLine("FONTSIZE", line);
            if (value < 1) {
                value = 1;
            }
            config.fontsize = value;
        }
        if (line.find("[FONT") != string::npos) {
            string result = parseStrFromLine("FONT", line);
            config.font = std::filesystem::path{ result }.make_preferred();
        }
        if (line.find("[USE_DF_COLORS") != string::npos) {
            string result = parseStrFromLine("USE_DF_COLORS", line);
            config.useDfColors = (result == "YES");
        }
        if (line.find("[BLACK_R") != string::npos) {
            int value = parseIntFromLine("BLACK_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::black].red = value;
        }
        if (line.find("[BLACK_G") != string::npos) {
            int value = parseIntFromLine("BLACK_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::black].green = value;
        }
        if (line.find("[BLACK_B") != string::npos) {
            int value = parseIntFromLine("BLACK_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::black].blue = value;
        }
        if (line.find("[BLUE_R") != string::npos) {
            int value = parseIntFromLine("BLUE_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::blue].red = value;
        }
        if (line.find("[BLUE_G") != string::npos) {
            int value = parseIntFromLine("BLUE_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::blue].green = value;
        }
        if (line.find("[BLUE_B") != string::npos) {
            int value = parseIntFromLine("BLUE_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::blue].blue = value;
        }
        if (line.find("[GREEN_R") != string::npos) {
            int value = parseIntFromLine("GREEN_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::green].red = value;
        }
        if (line.find("[GREEN_G") != string::npos) {
            int value = parseIntFromLine("GREEN_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::green].green = value;
        }
        if (line.find("[GREEN_B") != string::npos) {
            int value = parseIntFromLine("GREEN_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::green].blue = value;
        }
        if (line.find("[CYAN_R") != string::npos) {
            int value = parseIntFromLine("CYAN_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::cyan].red = value;
        }
        if (line.find("[CYAN_G") != string::npos) {
            int value = parseIntFromLine("CYAN_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::cyan].green = value;
        }
        if (line.find("[CYAN_B") != string::npos) {
            int value = parseIntFromLine("CYAN_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::cyan].blue = value;
        }
        if (line.find("[RED_R") != string::npos) {
            int value = parseIntFromLine("RED_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::red].red = value;
        }
        if (line.find("[RED_G") != string::npos) {
            int value = parseIntFromLine("RED_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::red].green = value;
        }
        if (line.find("[RED_B") != string::npos) {
            int value = parseIntFromLine("RED_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::red].blue = value;
        }
        if (line.find("[MAGENTA_R") != string::npos) {
            int value = parseIntFromLine("MAGENTA_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::magenta].red = value;
        }
        if (line.find("[MAGENTA_G") != string::npos) {
            int value = parseIntFromLine("MAGENTA_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::magenta].green = value;
        }
        if (line.find("[MAGENTA_B") != string::npos) {
            int value = parseIntFromLine("MAGENTA_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::magenta].blue = value;
        }
        if (line.find("[BROWN_R") != string::npos) {
            int value = parseIntFromLine("BROWN_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::brown].red = value;
        }
        if (line.find("[BROWN_G") != string::npos) {
            int value = parseIntFromLine("BROWN_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::brown].green = value;
        }
        if (line.find("[BROWN_B") != string::npos) {
            int value = parseIntFromLine("BROWN_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::brown].blue = value;
        }
        if (line.find("[LGRAY_R") != string::npos) {
            int value = parseIntFromLine("LGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgray].red = value;
        }
        if (line.find("[LGRAY_R") != string::npos) {
            int value = parseIntFromLine("LGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgray].green = value;
        }
        if (line.find("[LGRAY_R") != string::npos) {
            int value = parseIntFromLine("LGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgray].blue = value;
        }
        if (line.find("[DGRAY_R") != string::npos) {
            int value = parseIntFromLine("DGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::dgray].red = value;
        }
        if (line.find("[DGRAY_R") != string::npos) {
            int value = parseIntFromLine("DGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::dgray].green = value;
        }
        if (line.find("[DGRAY_R") != string::npos) {
            int value = parseIntFromLine("DGRAY_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::dgray].blue = value;
        }
        if (line.find("[LBLUE_R") != string::npos) {
            int value = parseIntFromLine("LBLUE_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lblue].red = value;
        }
        if (line.find("[LBLUE_G") != string::npos) {
            int value = parseIntFromLine("LBLUE_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lblue].green = value;
        }
        if (line.find("[LBLUE_B") != string::npos) {
            int value = parseIntFromLine("LBLUE_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lblue].blue = value;
        }
        if (line.find("[LGREEN_R") != string::npos) {
            int value = parseIntFromLine("LGREEN_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgreen].red = value;
        }
        if (line.find("[LGREEN_G") != string::npos) {
            int value = parseIntFromLine("LGREEN_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgreen].green = value;
        }
        if (line.find("[LGREEN_B") != string::npos) {
            int value = parseIntFromLine("LGREEN_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lgreen].blue = value;
        }
        if (line.find("[LCYAN_R") != string::npos) {
            int value = parseIntFromLine("LCYAN_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lcyan].red = value;
        }
        if (line.find("[LCYAN_G") != string::npos) {
            int value = parseIntFromLine("LCYAN_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lcyan].green = value;
        }
        if (line.find("[LCYAN_B") != string::npos) {
            int value = parseIntFromLine("LCYAN_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lcyan].blue = value;
        }
        if (line.find("[LRED_R") != string::npos) {
            int value = parseIntFromLine("LRED_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lred].red = value;
        }
        if (line.find("[LRED_G") != string::npos) {
            int value = parseIntFromLine("LRED_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lred].green = value;
        }
        if (line.find("[LRED_B") != string::npos) {
            int value = parseIntFromLine("LRED_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lred].blue = value;
        }
        if (line.find("[LMAGENTA_R") != string::npos) {
            int value = parseIntFromLine("LMAGENTA_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lmagenta].red = value;
        }
        if (line.find("[LMAGENTA_G") != string::npos) {
            int value = parseIntFromLine("LMAGENTA_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lmagenta].green = value;
        }
        if (line.find("[LMAGENTA_B") != string::npos) {
            int value = parseIntFromLine("LMAGENTA_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::lmagenta].blue = value;
        }
        if (line.find("[YELLOW_R") != string::npos) {
            int value = parseIntFromLine("YELLOW_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::yellow].red = value;
        }
        if (line.find("[YELLOW_G") != string::npos) {
            int value = parseIntFromLine("YELLOW_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::yellow].green = value;
        }
        if (line.find("[YELLOW_B") != string::npos) {
            int value = parseIntFromLine("YELLOW_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::yellow].blue = value;
        }
        if (line.find("[WHITE_R") != string::npos) {
            int value = parseIntFromLine("WHITE_R", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::white].red = value;
        }
        if (line.find("[WHITE_G") != string::npos) {
            int value = parseIntFromLine("WHITE_G", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::white].green = value;
        }
        if (line.find("[WHITE_B") != string::npos) {
            int value = parseIntFromLine("WHITE_B", line);
            if (value > 255) {
                value = 255;
            }
            if (value < 0) {
                value = 0;
            }
            config.colors[dfColors::white].blue = value;
        }
        if (line.find("[RENDERER") != string::npos) {
            string result = parseStrFromLine("RENDERER", line);
            config.opengl = (result == "OPENGL");
            config.software = (result == "SOFTWARE");
            config.directX = (result == "DIRECTX");
        }
        if (line.find("[EXTRUDE_TILES") != string::npos) {
            string result = parseStrFromLine("EXTRUDE_TILES", line);
            config.extrude_tiles = (result == "YES");
        }
        if (line.find("[PIXELPERFECT_ZOOM") != string::npos) {
            string result = parseStrFromLine("PIXELPERFECT_ZOOM", line);
            config.pixelperfect_zoom = (result == "YES");
        }
        if (line.find("[NIGHT") != string::npos) {
            string result = parseStrFromLine("NIGHT", line);
            config.dayNightCycle = (result == "YES");
        }
        if (line.find("[FOG_OF_WAR") != string::npos) {
            string result = parseStrFromLine("FOG_OF_WAR", line);
            config.fog_of_war = (result == "YES");
        }
    }
}

bool isViewTracking() {
    auto& ssConfig = stonesenseState.ssConfig;
    return ssConfig.config.track_mode != Config::TRACKING_NONE;
}

bool loadConfigFile()
{
    auto path = std::filesystem::path{} / "dfhack-config" / "stonesense" / "init.txt";
    std::ifstream myfile(path);
    if (myfile.is_open() == false) {
        LogError("cannot find init file\n");
        return false;
    }

    Config newConfig{};
    while (!myfile.eof()) {
        string line;
        getline(myfile, line);
        parseConfigLine(newConfig, line);
    }

    // update allegro colors loaded from file
    newConfig.colors.update();

    // apply new config
    stonesenseState.ssConfig.config = newConfig;

    // apply configuration settings to app state
    stonesenseState.ssState.ScreenH = newConfig.defaultScreenHeight;
    stonesenseState.ssState.ScreenW = newConfig.defaultScreenWidth;
    stonesenseState.ssState.Size = newConfig.defaultSegmentSize + Crd3D{ 2, 2, 0 };
    stonesenseState.lift_segment_offscreen_x = 0;
    stonesenseState.lift_segment_offscreen_y = newConfig.lift_segment;


    return true;
}

// Set the proper scale based on the current zoom level
void GameConfiguration::recalculateScale() {
    if (this->config.pixelperfect_zoom) {
        if (zoom > 0)
            scale = zoom;
        else
            scale = std::pow(0.5, -zoom + 1);
    } else {
        scale = std::pow(SCALEZOOMFACTOR, zoom);
    }
}
