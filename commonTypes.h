#pragma once

#include <filesystem>
#include <unordered_map>
#include <concepts>

#include "common.h"
#include "SpriteColors.h"
#include "df/enabler.h"
#include "df/graphic.h"

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

struct Crd2D {
    int32_t x,y;
};
struct Crd3D {
    int32_t x,y,z;
    constexpr Crd3D operator+(const Crd3D rhs)
    {
        return Crd3D{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
};


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
    } colors[16]{
        { 0,0,0 }, // black
        { 13,103,196 }, // blue
        { 68,158,53 }, // green
        { 86,163,205 }, // cyan
        { 151,26,26 }, // red
        { 255,110,187 }, // magenta
        { 120,94,47 }, // brown
        { 185,192,162 }, // light gray
        { 88,83,86 }, // dark gray
        { 145,202,255 }, // light blue
        { 131,212,82 }, // light green
        { 176,223,215 }, // light cyan
        { 255,34,34 }, // light red
        { 255,167,246 }, // light magenta
        { 255,218,90 }, // yellow
        { 255,255,255 } // white
    };
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
    ALLEGRO_COLOR getDfColor(int color, bool useDfColors) const {
        if(color < 0 || color >= 16) {
            return al_map_rgb(255,255,255);
        }
        if (useDfColors)
        {
            return al_map_rgb_f(df::global::gps->ccolor[color][0], df::global::gps->ccolor[color][1], df::global::gps->ccolor[color][2]);
        }
        return colors[ (color_name) color].al;
    }
    ALLEGRO_COLOR getDfColor(int color, int bright, bool useDfColors) const {
        return getDfColor(color + (bright * 8), useDfColors);
    }
};

// this is required because gcc 10 can't handle a dependently typed non-type template argu,ent
#if defined(__GNUC__) && __GNUC__ < 11
template <std::floating_point T>
#else
template <std::floating_point T, T alpha = T{ 0.9 } >
#endif
class RollingAverage
{
private:
#if defined(__GNUC__) && __GNUC__ < 11
    static constexpr T alpha = T{ 0.9 };
#endif
    T store;
    bool empty{ true };
public:
    const T get() const { return store; }
    void update(const T val) { store = empty ? val : store * alpha + val * (T{ 1.0 } - alpha); empty = false; }
    operator T() const { return store; }
};

struct FrameTimers{
    RollingAverage<float> read_time;
    RollingAverage<float> beautify_time;
    RollingAverage<float> assembly_time;
    RollingAverage<float> draw_time;
    RollingAverage<float> frame_total;

    clock_t prev_frame_time{ clock() };
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

struct Stonesense_Unit{
    df::unit * origin;

    uint16_t profession;
    std::string custom_profession;

    int32_t squad_leader_id;
    uint32_t nbcolors;
    uint32_t color[15]; // Was using DFHack::Units::MAX_COLORS for no apparent reason; TODO: Use a better number?

    hairstyles hairstyle[hairtypes_end];
    uint32_t hairlength[hairtypes_end];

    bool isLegend;
    std::unique_ptr<unit_inventory> inv;
};

struct Stonesense_Building
{
    uint32_t x1;
    uint32_t y1;
    uint32_t x2;
    uint32_t y2;
    uint32_t z;
    DFHack::t_matglossPair material;
    df::building_type type;
    union
    {
        int16_t subtype;
        df::civzone_type civzone_type;
        df::furnace_type furnace_type;
        df::workshop_type workshop_type;
        df::construction_type construction_type;
        df::shop_type shop_type;
        df::siegeengine_type siegeengine_type;
        df::trap_type trap_type;
    };
    int32_t custom_type;
    df::building* origin;
};

template <typename Key, typename Val, typename Hash = std::hash<Key>>
class SparseArray {

    std::unordered_map< Key, Val, Hash> map;

public:
    void clear()
    {
        map.clear();
    }
    void add(Key&& k, Val& v)
    {
        auto it = map.find(k);
        if (it != map.end())
        {
            it->second = v;
        }
        else
        {
            map.emplace(k, v);
        }
    }
    std::optional<Val> lookup(Key&& k)
    {
        auto it = map.find(k);
        return it != map.end() ? std::optional{ it->second } : std::nullopt;
    }
};
