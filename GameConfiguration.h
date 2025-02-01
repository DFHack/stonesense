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
    bool immersive_mode;
    bool show_designations = true;
    bool show_announcements = false;
    bool show_keybinds = false;
    bool single_layer_view;
    bool shade_hidden_tiles = true;
    bool show_hidden_tiles = false;
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

    void recalculateScale();

    void reset()
    {
        config = {};

        show_announcements = true;
        hide_outer_tiles = false;
        shade_hidden_tiles = true;
        load_ground_materials = true;
        show_designations = true;
        show_keybinds = false;
        track_screen_center = true;
        creditScreen = true;
        bloodcutoff = 100;
        poolcutoff = 100;
        threadmade = 0;
        threading_enable = 1;
        occlusion = 1;
        zoom = 0;
        scale = 1.0f;
    }
};
