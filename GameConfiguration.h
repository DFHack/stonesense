#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"

#include "allegro5/color.h"

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
    bool show_all_creatures;
    int automatic_reload_time = 0;
    int automatic_reload_step = 500;
    bool debug_mode = false;
    bool transparentScreenshots;
    int lift_segment = 0;
    int animation_step = 300;
    bool verbose_logging;
    trackingmode track_mode = TRACKING_CENTER;
    bool invert_mouse_z;
    bool follow_DFcursor;
    bool show_creature_names = false;
    bool show_creature_moods;
    bool show_creature_jobs;
    uint8_t show_creature_professions;
    bool names_use_nick;
    bool names_use_species;
    bool show_osd = false;
    bool cache_images;
    bool show_stockpiles;
    bool show_zones;
    bool show_intro = true;
    ALLEGRO_COLOR fogcol = al_map_rgba(255, 255, 255, 255);
    bool fogenable = true;
    ALLEGRO_COLOR backcol = al_map_rgb(95, 95, 160);
    int viewXoffset;
    int viewYoffset;
    int viewZoffset;
    int bitmapHolds = 4096;
    bool saveImageCache;
    int fontsize = 10;
    std::filesystem::path font =
        std::filesystem::path{ } / "data" / "art" / "font.ttf";
    bool useDfColors = false;
    dfColors colors;
    bool opengl;
    bool directX;
    bool software;
    bool dayNightCycle;
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
