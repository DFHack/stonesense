#pragma once

#include "common.h"
#include "SpriteColors.h"

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
    ShadeEquip
} ;
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
    ALLEGRO_COLOR getDfColor(int color) {
        if(color < 0 || color >= 16) {
            return al_map_rgb(255,255,255);
        }
        return colors[ (color_name) color].al;
    }
    ALLEGRO_COLOR getDfColor(int color, int bright) {
        return getDfColor(color+(bright*8));
    }
};


struct GameConfiguration {
    bool show_zones;
    bool show_stockpiles;
    bool show_osd;
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
    bool track_center;
    int lift_segment_offscreen;
    uint8_t truncate_walls;
    bool follow_DFscreen;
    bool verbose_logging;
    int viewXoffset;
    int viewYoffset;
    int viewZoffset;
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
    Crd3D segmentSize;

    bool follow_DFcursor;
    int dfCursorX;
    int dfCursorY;
    int dfCursorZ;
    unsigned int blockDimX;
    unsigned int blockDimY;
    unsigned int blockDimZ;

    int bitmapHolds;

    bool saveImageCache;
    bool cache_images;
    int imageCacheSize;
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
    bool show_creature_professions;
    bool show_creature_jobs;

    bool transparentScreenshots;

    bool fog_of_war;

    bool occlusion;
    bool tile_count;

    uint16_t bloodcutoff;
    uint16_t poolcutoff;
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

    ALLEGRO_MUTEX * readMutex;
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
    int DisplayedSegmentX;
    int DisplayedSegmentY;
    int DisplayedSegmentZ;
    int DisplayedRotation;

    //the width and height of the stonesense window
    int ScreenW;
    int ScreenH;
};

struct FrameTimers{
    float read_time;
    float beautify_time;
    float assembly_time;
    float draw_time;

    clock_t prev_frame_time;
    float frame_total;
};


enum enumCreatureSex {
    eCreatureSex_NA,
    eCreatureSex_Male,
    eCreatureSex_Female
};