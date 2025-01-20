#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"

#include "allegro5/color.h"

// constexpr auto GFXMODE = GFX_AUTODETECT_WINDOWED;
constexpr auto DEFAULT_FULLSCREEN_MODE = false;
constexpr auto DEFAULT_RESOLUTION_WIDTH = 800;
constexpr auto DEFAULT_RESOLUTION_HEIGHT = 600;
constexpr auto DEFAULT_SIZE = /*20*/ 70;
constexpr auto DEFAULT_SIZE_Z = /*6*/ 4;

struct GameConfiguration {
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

    // below items are not configurable via the config file
    bool overlay_mode;
    bool show_designations = true;
    bool show_announcements = false;
    bool show_keybinds = false;
    bool single_layer_view;
    bool shade_hidden_tiles = false;
    bool show_hidden_tiles;
    bool load_ground_materials = false;
    bool hide_outer_tiles = false;
    uint8_t truncate_walls;
    bool track_screen_center = true;

    uint32_t menustate;
    //DFHack::t_viewscreen viewscreen;

    bool spriteIndexOverlay;
    bool creditScreen = true;
    int currentSpriteOverlay;

    bool occlusion = true;
    bool tile_count;

    uint8_t bloodcutoff = 100;
    uint8_t poolcutoff = 100;
    //follows are anti-crash things
    bool skipWorld;
    bool skipCreatures;
    bool skipCreatureTypes;
    bool skipCreatureTypesEx;
    bool skipDescriptorColors;
    bool skipBuildings;
    bool skipVegetation;
    bool skipConstructions;
    bool skipMaps;
    bool skipInorganicMats;
    bool skipOrganicMats;

    //following are threading stuff
    ALLEGRO_COND* readCond;
    ALLEGRO_THREAD* readThread;
    bool threadmade = false;
    bool threadstarted;

    bool threading_enable = true;
    int platecount;

    int zoom = 0;
    float scale = 1.0f;
};
