#pragma once

#include "common.h"
#include "SpriteObjects.h"
#include "TileTypes.h"
#include "df/item_type.h"


//not sure where to put these...

inline bool IDisWall(int in)
{
    return isWallTerrain( (tiletype::tiletype) in );
}

inline bool IDisFloor(int in)
{
    return isFloorTerrain( (tiletype::tiletype) in );
}

inline bool IDhasOpaqueFloor(int in)
{
    return !FlowPassableDown( (tiletype::tiletype) in );
}

inline bool IDhasOpaqueSides(int in)
{
    return (!FlowPassable( (tiletype::tiletype) in )) && in != tiletype::Tree;
}

class WorldSegment;

struct Effect {
    DFHack::t_matglossPair matt;
    int16_t density;
};

struct Worn_Item {
    DFHack::t_matglossPair matt;
    DFHack::t_matglossPair dyematt;
    int8_t rating;
    Worn_Item();
};

struct Unit_Inventory {
    //[item_type][item_subtype][item_number]
    std::vector<std::vector<std::vector<Worn_Item>>> item;
};

struct Draw_Event{
    ALLEGRO_BITMAP *bitmap;
    ALLEGRO_COLOR tint;
    float sx;
    float sy;
    float sw;
    float sh;
    float dx;
    float dy;
    float dw;
    float dh;
    int flags;
};


class Block
{
public:
    Block(WorldSegment* ownerSegment, df::tiletype type);
    ~Block(void);
    static void* operator new (size_t size);
    static void operator delete (void *p);
    bool visible;

    WorldSegment* ownerSegment;

    uint32_t x, y, z;
    int drawx, drawy, drawz;
    df::tiletype_shape_basic tileShapeBasic;
    df::tiletype_shape tileShape;
    df::tiletype_special tileSpecial;
    df::tiletype_material tileMaterial;
    df::tiletype tileType;
    DFHack::t_matglossPair material;
    DFHack::t_matglossPair layerMaterial;
    DFHack::t_matglossPair veinMaterial;
    bool hasVein;

    bool depthBorderNorth;
    bool depthBorderWest;
    bool depthBorderDown;
    int shadow;

    uint8_t wallborders;
    uint8_t floorborders;
    uint8_t openborders;
    uint8_t rampborders;
    uint8_t upstairborders;
    uint8_t downstairborders;
    uint8_t lightborders;

    bool creaturePresent;
    bool buildingPresent;
    bool fog_of_war;
    DFHack::t_matglossPair ramp;
    DFHack::t_matglossPair water;

    //following are neighbor water levels (unused)
    //DFHack::t_matglossPair abovewater;
    //DFHack::t_matglossPair belowwater;
    //DFHack::t_matglossPair rightwater;
    //DFHack::t_matglossPair leftwater;

    DFHack::t_designation designation;
    DFHack::t_occupancy occ;
    DFHack::Units::t_unit * creature;
    DFHack::t_matglossPair tree;

    int mudlevel;
    int snowlevel;
    int bloodlevel;
    ALLEGRO_COLOR bloodcolor;

    uint8_t grasslevel;
    uint32_t grassmat;

    uint8_t engraving_character;
    df::engraving_flags engraving_flags;
    uint8_t engraving_quality;

    //vector<uint8_t> grasslevels;
    //vector<uint32_t> grassmats;

    //struct BlockEffects //size 40
    //{
    //	uint16_t count;
    //	uint16_t type;
    //  DFHack::t_matglossPair material;
    //	int16_t lifetime;
    //	int16_t x_direction;
    //	int16_t y_direction;
    //	uint8_t canCreateNew;//??
    //} blockeffects;

    //individual effects
    Effect Eff_Miasma;
    Effect Eff_Steam;
    Effect Eff_Mist;
    Effect Eff_MaterialDust;
    Effect Eff_MagmaMist;
    Effect Eff_Smoke;
    Effect Eff_Dragonfire;
    Effect Eff_Fire;
    Effect Eff_Web;
    Effect Eff_MaterialGas;
    Effect Eff_MaterialVapor;
    Effect Eff_OceanWave;
    Effect Eff_SeaFoam;
    struct {
        DFHack::Buildings::t_building info;
        std::vector<c_sprite> sprites;
        uint32_t index;
        Block * parent;
    } building;

    bool IsVisible() {
        return IDisWall(tileType) || IDisFloor(tileType);
    }
    void Assemble();
    void Drawcreaturetext();
    void AddRamptop();
    void AssembleFloorBlood ( int32_t drawx, int32_t drawy );
    void AssembleParticleCloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint);
    void AssembleSpriteFromSheet(int spriteNum, ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Block * b=NULL, float in_scale=1.0f);
    void AssembleSprite(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);
    void Draw();
    //void DrawPixel(int drawx, int drawy);

    uint16_t temp1;
    uint16_t temp2;

    uint16_t consForm;

    bool obscuringCreature;
    bool obscuringBuilding;

    struct SS_Item {
        DFHack::t_matglossPair item;
        DFHack::t_matglossPair matt;
        DFHack::t_matglossPair dyematt;
    } Item;

    //These are actually applied to the creature standing here, but there's only one creature shown, so it's okay.
    Unit_Inventory * inv;


private:
    std::vector<Draw_Event> todraw;

};
void createEffectSprites();
void destroyEffectSprites();
void initRandomCube();

//find a better place for these
bool hasWall(Block* b);
bool hasBuildingOfID(Block* b, int ID);
bool hasBuildingIdentity(Block* b, uint32_t index, int buildingOcc);
bool hasBuildingOfIndex(Block* b, uint32_t index);
bool wallShouldNotHaveBorders( int in );