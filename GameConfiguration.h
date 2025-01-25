#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"
#include "Config.h"

#include "allegro5/color.h"

struct GameConfiguration {
    // Loadable configuration
    Config config;

    // items not configurable via the config file
    bool overlay_mode;
    bool show_designations = true;
    bool show_announcements = false;
    bool show_keybinds = false;
    bool single_layer_view;
    bool shade_hidden_tiles = false;
    bool show_hidden_tiles;
    bool load_ground_materials = false;
    bool hide_outer_tiles = false;
    bool debug_mode = false;
    bool autosize_segmentX = false;
    bool autosize_segmentY = false;
    int lift_segment_offscreen_x = 0;
    int lift_segment_offscreen_y = 0;

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

    void recalculateScale();
};
