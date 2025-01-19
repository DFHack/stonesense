#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"

#include "allegro5/color.h"

struct GameConfiguration {
    bool overlay_mode;
    bool show_zones;
    bool show_stockpiles;
    bool show_designations = true;
    bool show_osd = false;
    bool show_announcements = false;
    bool show_keybinds = false;
    bool closeOnEsc = true;
    bool single_layer_view;
    bool shade_hidden_tiles = false;
    bool show_hidden_tiles;
    bool show_creature_names = false;
    bool names_use_nick;
    bool names_use_species;
    bool show_all_creatures;
    bool load_ground_materials = false;
    bool hide_outer_tiles = false;
    bool debug_mode = false;
    bool autosize_segmentX = false;
    bool autosize_segmentY = false;
    int lift_segment_offscreen_x = 0;
    int lift_segment_offscreen_y = 0;
    uint8_t truncate_walls;
    bool verbose_logging;
    int viewXoffset;
    int viewYoffset;
    int viewZoffset;
    bool track_screen_center = true;
    int automatic_reload_time = 0;
    int automatic_reload_step = 500;
    int animation_step = 300;
    int fontsize = 10;
    std::filesystem::path font =
        std::filesystem::path{ } / "data" / "art" / "font.ttf";
    bool Fullscreen = DEFAULT_FULLSCREEN_MODE;
    bool show_intro = true;
    ALLEGRO_COLOR fogcol = al_map_rgba(255, 255, 255, 255);
    ALLEGRO_COLOR backcol = al_map_rgb(95, 95, 160);
    bool fogenable = true;

    bool follow_DFcursor;

    enum trackingmode : uint8_t {
        TRACKING_NONE,
        TRACKING_CENTER,
        TRACKING_FOCUS,

        TRACKING_INVALID
    };
    trackingmode track_mode = TRACKING_CENTER;

    bool invert_mouse_z;

    int bitmapHolds = 4096;

    bool saveImageCache;
    bool cache_images;
    int imageCacheSize = 4096;
    bool useDfColors = false;
    dfColors colors;
    bool opengl;
    bool directX;
    bool software;

    uint32_t menustate;
    //DFHack::t_viewscreen viewscreen;

    bool spriteIndexOverlay;
    bool creditScreen = true;
    int currentSpriteOverlay;

    bool dayNightCycle;

    bool show_creature_moods;
    bool show_creature_jobs;
    uint8_t show_creature_professions;

    bool transparentScreenshots;

    bool fog_of_war = true;

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
