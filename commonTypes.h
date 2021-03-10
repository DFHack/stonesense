#pragma once

#include "common.h"
#include "SpriteColors.h"
#include "df/enabler.h"

enum ShadeBy {
    ShadeNone,
    ShadeXml,
    ShadeNamed,
    ShadeMat,
    ShadeLayer,
    ShadeVein,
    ShadeMatFore,
    ShadeMatBack,
    ShadeLayerFore,
    ShadeLayerBack,
    ShadeVeinFore,
    ShadeVeinBack,
    ShadeBodyPart,
    ShadeJob,
    ShadeBlood,
    ShadeBuilding,
    ShadeGrass,
    ShadeItem,
    ShadeEquip,
    ShadeWood,
    ShadeGrowth
} ;

enum hairstyles {
    hairstyles_invalid = -1,
    NEATLY_COMBED,
    BRAIDED,
    DOUBLE_BRAID,
    PONY_TAILS,
    CLEAN_SHAVEN,
    hairstyles_end
};

enum hairtypes {
    hairtypes_invalid = -1,
    HAIR,
    BEARD,
    MOUSTACHE,
    SIDEBURNS,
    hairtypes_end
};

struct t_subSprite {
    int32_t sheetIndex;
    int32_t fileIndex;
    ALLEGRO_COLOR shadeColor;
    ShadeBy shadeBy;
    char bodyPart[128];
    uint8_t snowMin;
    uint8_t snowMax;
} ;

struct t_SpriteWithOffset {
    int32_t sheetIndex;
    int16_t x;
    int16_t y;
    int32_t fileIndex;
    uint8_t numVariations;
    char animFrames;
    ALLEGRO_COLOR shadeColor;
    bool needOutline;
    std::vector<t_subSprite> subSprites;
    ShadeBy shadeBy;
    char bodyPart[128];
    uint8_t snowMin;
    uint8_t snowMax;
} ;

typedef struct Crd2D {
    int32_t x,y;
} Crd2D;
typedef struct Crd3D {
    int32_t x,y,z;
} Crd3D;

class dfColors
{
public:
    dfColors() {
        memset(colors, 0, sizeof(colors));
        update();
    }
    struct color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        ALLEGRO_COLOR al;
        void update() {
            al = al_map_rgb(red,green,blue);
        }
    } colors[16];
    enum color_name {
        black,
        blue,
        green,
        cyan,
        red,
        magenta,
        brown,
        lgray,
        dgray,
        lblue,
        lgreen,
        lcyan,
        lred,
        lmagenta,
        yellow,
        white
    };
    void update() {
        for (int i = 0; i < 16; i++) {
            colors[i].update();
        }
    }
    color & operator [] (color_name col) {
        return colors[col];
    }
    ALLEGRO_COLOR getDfColor(int color, bool useDfColors) {
        if(color < 0 || color >= 16) {
            return al_map_rgb(255,255,255);
        }
        if (useDfColors)
        {
            return al_map_rgb_f(df::global::enabler->ccolor[color][0], df::global::enabler->ccolor[color][1], df::global::enabler->ccolor[color][2]);
        }
        return colors[ (color_name) color].al;
    }
    ALLEGRO_COLOR getDfColor(int color, int bright, bool useDfColors) {
        return getDfColor(color + (bright * 8), useDfColors);
    }
};


struct GameConfiguration {
    bool overlay_mode;
    bool show_zones;
    bool show_stockpiles;
    bool show_designations;
    bool show_osd;
    bool show_keybinds;
    bool single_layer_view;
    bool shade_hidden_tiles;
    bool show_hidden_tiles;
    bool show_creature_names;
    bool names_use_nick;
    bool names_use_species;
    bool show_all_creatures;
    bool load_ground_materials;
    bool hide_outer_tiles;
    bool debug_mode;
    int lift_segment_offscreen_x;
    int lift_segment_offscreen_y;
    uint8_t truncate_walls;
    bool verbose_logging;
    int viewXoffset;
    int viewYoffset;
    int viewZoffset;
    bool track_screen_center;
    int automatic_reload_time;
    int automatic_reload_step;
    int animation_step;
    int fontsize;
    ALLEGRO_PATH * font;
    bool Fullscreen;
    bool show_intro;
    ALLEGRO_COLOR fogcol;
    ALLEGRO_COLOR backcol;
    bool fogenable;

    bool follow_DFcursor;

    uint8_t track_mode;
    enum trackingmodes : uint8_t {
        TRACKING_NONE,
        TRACKING_CENTER,
        TRACKING_WINDOW,
        TRACKING_FOCUS,

        TRACKING_INVALID
    };

    int bitmapHolds;

    bool saveImageCache;
    bool cache_images;
    int imageCacheSize;
    bool useDfColors;
    dfColors colors;
    bool opengl;
    bool directX;
    bool software;

    uint32_t menustate;
    //DFHack::t_viewscreen viewscreen;

    bool spriteIndexOverlay;
    bool creditScreen;
    int currentSpriteOverlay;

    bool dayNightCycle;

    bool show_creature_moods;
    bool show_creature_jobs;
    uint8_t show_creature_professions;

    bool transparentScreenshots;

    bool fog_of_war;

    bool occlusion;
    bool tile_count;

    uint8_t bloodcutoff;
    uint8_t poolcutoff;
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
    ALLEGRO_COND * readCond;
    ALLEGRO_THREAD * readThread;
    bool threadmade;
    bool threadstarted;

    bool threading_enable;
    int platecount;

    int zoom;
    float scale;
};

struct GameState{
    //properties of the currently viewed portion of the segment
    Crd3D Position;
    int Rotation;

    //the size of the next segment to load, and the map region
    Crd3D Size;
    Crd3D RegionDim;

    //position of the cursor
    Crd3D dfCursor;
    //position of the selection cursor
    Crd3D dfSelection;

    //the width and height of the stonesense window
    int ScreenW;
    int ScreenH;
};

struct FrameTimers{
    float read_time;
    float beautify_time;
    float assembly_time;
    float draw_time;
    float overlay_time;

    clock_t prev_frame_time;
    float frame_total;
};

struct SS_Item {
    DFHack::t_matglossPair item;
    DFHack::t_matglossPair matt;
    DFHack::t_matglossPair dyematt;
};

struct worn_item {
    DFHack::t_matglossPair matt;
    DFHack::t_matglossPair dyematt;
    int8_t rating;
    worn_item();
};

struct unit_inventory {
    //[item_type][item_subtype][item_number]
    std::vector<std::vector<std::vector<worn_item>>> item;
};

struct SS_Unit{
    df::unit * origin;
    uint16_t x;
    uint16_t y;
    uint16_t z;
    int32_t civ;

    uint16_t profession;
    std::string custom_profession;

    struct {
        bool active;
        uint32_t jobId;
        uint8_t jobType;
    } current_job;

    uint32_t stress_level;
    df::unit_attribute strength;
    df::unit_attribute agility;
    df::unit_attribute toughness;
    df::unit_attribute endurance;
    df::unit_attribute recuperation;
    df::unit_attribute disease_resistance;
    int32_t squad_leader_id;
    uint32_t nbcolors;
    uint32_t color[DFHack::Units::MAX_COLORS];

    hairstyles hairstyle[hairtypes_end];
    uint32_t hairlength[hairtypes_end];

    bool isLegend;
    std::unique_ptr<unit_inventory> inv;
};
