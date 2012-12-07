#pragma once

#include "common.h"
#include "SpriteObjects.h"
#include "TileTypes.h"
#include "df/item_type.h"

class WorldSegment;

struct effect {
    DFHack::t_matglossPair matt;
    int16_t density;
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

class Tile
{
public:
    Tile(WorldSegment* ownerSegment, df::tiletype type);
    ~Tile(void);

    bool visible;

    WorldSegment* ownerSegment;

    uint32_t x, y, z;
    //int drawx, drawy, drawz;
    df::tiletype tileType;
    DFHack::t_matglossPair material;
    DFHack::t_matglossPair layerMaterial;
    DFHack::t_matglossPair veinMaterial;
    bool hasVein;

    bool depthBorderNorth;
    bool depthBorderWest;
    bool depthBorderDown;

    uint8_t shadow;

    uint8_t wallborders;
    uint8_t floorborders;
    uint8_t openborders;
    uint8_t rampborders;
    uint8_t upstairborders;
    uint8_t downstairborders;
    uint8_t lightborders;

    bool fog_of_war;//contained in designation

    uint8_t rampindex;

    //DFHack::t_matglossPair water;//contained in designation
    bool deepwater;

    //following are neighbor water levels (unused)
    //DFHack::t_matglossPair abovewater;
    //DFHack::t_matglossPair belowwater;
    //DFHack::t_matglossPair rightwater;
    //DFHack::t_matglossPair leftwater;
    
    DFHack::t_designation designation;
    DFHack::t_occupancy occ;

    DFHack::Units::t_unit * creature;
    DFHack::t_matglossPair tree;

    uint8_t mudlevel;
    uint8_t snowlevel;
    uint8_t bloodlevel;
    ALLEGRO_COLOR bloodcolor;

    uint8_t grasslevel;
    uint32_t grassmat;

    uint8_t engraving_character;
    df::engraving_flags engraving_flags;
    uint8_t engraving_quality;

    //vector<uint8_t> grasslevels;
    //vector<uint32_t> grassmats;

    //struct TileEffects //size 40
    //{
    //	uint16_t count;
    //	uint16_t type;
    //  DFHack::t_matglossPair material;
    //	int16_t lifetime;
    //	int16_t x_direction;
    //	int16_t y_direction;
    //	uint8_t canCreateNew;//??
    //} tileeffects;
    
    bool haseffect;
    //individual effects
    effect Eff_Miasma;
    effect Eff_Steam;
    effect Eff_Mist;
    effect Eff_MaterialDust;
    effect Eff_MagmaMist;
    effect Eff_Smoke;
    effect Eff_Dragonfire;
    effect Eff_Fire;
    effect Eff_Web;
    effect Eff_MaterialGas;
    effect Eff_MaterialVapor;
    effect Eff_OceanWave;
    effect Eff_SeaFoam;

    uint16_t consForm;

    bool obscuringCreature;
    bool obscuringBuilding;

    struct SS_Item {
        DFHack::t_matglossPair item;
        DFHack::t_matglossPair matt;
        DFHack::t_matglossPair dyematt;
    } Item;

    //These are actually applied to the creature standing here, but there's only one creature shown, so it's okay.
    unit_inventory * inv;

    struct {
        DFHack::Buildings::t_building info;
        std::vector<c_sprite> sprites;
        uint32_t index;
        Tile * parent;
    } building;

    //tile information loading
    inline df::tiletype_shape_basic tileShapeBasic()
    {
        return DFHack::tileShapeBasic(DFHack::tileShape(tileType));
    }

    inline df::tiletype_shape tileShape()
    {
        return DFHack::tileShape(tileType);
    }

    inline df::tiletype_special tileSpecial()
    {
        return DFHack::tileSpecial(tileType);
    }

    inline df::tiletype_material tileMaterial()
    {
        return DFHack::tileMaterial(tileType);
    }

    void Reset(WorldSegment* ownerSegment, df::tiletype type);
    //tile sprite assembly and drawing functions
    void AssembleTile();
    void Drawcreaturetext();
    void AddRamptop();
    void AssembleFloorBlood ( int32_t drawx, int32_t drawy );
    void AssembleParticleCloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint);
    void AssembleSpriteFromSheet(int spriteNum, ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Tile * b=NULL, float in_scale=1.0f);
    void AssembleSprite(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);

private:

};
void createEffectSprites();
void destroyEffectSprites();
void initRandomCube();

//find a better place for these
bool hasWall(Tile* b);
bool hasBuildingOfID(Tile* b, int ID);
bool hasBuildingIdentity(Tile* b, uint32_t index, int buildingOcc);
bool hasBuildingOfIndex(Tile* b, uint32_t index);
bool wallShouldNotHaveBorders( int in );

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