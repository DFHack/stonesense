#pragma once
#include <optional>
#include <string>

#include "common.h"

constexpr auto KEYMOD_NONE = 0;

constexpr bool DEFAULT_FULLSCREEN_MODE = false;
constexpr int DEFAULT_RESOLUTION_WIDTH = 800;
constexpr int DEFAULT_RESOLUTION_HEIGHT = 600;
constexpr int DEFAULT_SIZE = 70;
constexpr int DEFAULT_SIZE_Z = 4;

class Config {
public:
    enum trackingmode : uint8_t {
        TRACKING_NONE,
        TRACKING_CENTER,
        TRACKING_FOCUS,
        TRACKING_INVALID
    };

    bool closeOnEsc = true;
    int defaultScreenWidth{ DEFAULT_RESOLUTION_WIDTH };
    int defaultScreenHeight{ DEFAULT_RESOLUTION_HEIGHT };
    bool Fullscreen{ DEFAULT_FULLSCREEN_MODE };
    Crd3D defaultSegmentSize{
        .x = DEFAULT_SIZE,
        .y = DEFAULT_SIZE,
        .z = DEFAULT_SIZE_Z
    };
    bool show_all_creatures = false;
    int automatic_reload_time = 50;
    int automatic_reload_step = 50;
    bool debug_mode = false;
    bool transparentScreenshots = false;
    int lift_segment = 0;
    int animation_step = 196;
    bool verbose_logging = false;
    trackingmode track_mode = TRACKING_FOCUS;
    bool invert_mouse_z = false;
    bool follow_DFcursor = true;
    bool show_creature_names = false;
    bool show_creature_moods = false;
    bool show_creature_jobs = false;
    uint8_t show_creature_professions;
    bool names_use_nick = true;
    bool names_use_species = true;
    bool show_osd = false;
    bool cache_images = false;
    bool show_stockpiles = true;
    bool show_zones = true;
    bool show_intro = false;
    ALLEGRO_COLOR fogcol = al_map_rgba(128, 158, 177, 30);
    bool fogenable = true;
    ALLEGRO_COLOR backcol = al_map_rgb(128, 158, 177);
    Crd3D viewOffset{ 0,0,0 };
    int bitmapHolds = 4096;
    bool saveImageCache = false;
    int fontsize = 10;
    std::filesystem::path font = std::filesystem::path{ "DejaVuSans.ttf" };
    bool useDfColors = false;
    dfColors colors;
    bool opengl = false;
    bool directX = false;
    bool software = false;
    bool dayNightCycle = false;
    int imageCacheSize = 4096;
    bool fog_of_war = true;
    bool extrude_tiles = true;
    bool pixelperfect_zoom = false;
};

struct action_name_mapper {
    std::string name;
    void (*func)(uint32_t);
};

bool isViewTracking();
bool loadConfigFile();
std::optional<std::string> trim_line(std::string line);
