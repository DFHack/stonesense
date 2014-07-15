#pragma once

#include "common.h"
#include "SpriteObjects.h"
#include "TileTypes.h"
#include "df/item_type.h"
#include <vector>

class WorldSegment;

class Tile
{
private:
    bool valid;

    //Functions start here.
    
	//do not directly call constructor or deconstructor, use CleanCreateAndValidate or InvalidateAndDestroy
    Tile(WorldSegment* ownerSegment, df::tiletype type);
    ~Tile(void);

public:
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

    bool fog_of_war;

    uint8_t rampindex;

    //DFHack::t_matglossPair water;//contained in designation
    bool deepwater;
    
	df::tile_liquid_flow_dir flow_direction;

    DFHack::t_designation designation;
    DFHack::t_occupancy occ;

    SS_Unit * creature;
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

    uint16_t consForm;

    bool obscuringCreature;
    bool obscuringBuilding;

    struct SS_Effect {
        DFHack::t_matglossPair matt;
        int16_t density;
        df::flow_type type;
    } tileeffect;

	SS_Item Item;

    struct SS_Building {
        DFHack::Buildings::t_building* info;
        df::building_type type;
        std::vector<c_sprite> sprites;
        Tile* parent;
		std::vector<worn_item> constructed_mats;
		uint8_t special;
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

    //tile sprite assembly and drawing functions
	void GetDrawLocation(int32_t& drawx, int32_t& drawy);
    void AssembleTile();
    void AddRamptop();
	void AssembleDesignationMarker( int32_t drawx, int32_t drawy );
    void AssembleFloorBlood ( int32_t drawx, int32_t drawy );
    void AssembleParticleCloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint);
    void AssembleSpriteFromSheet(int spriteNum, ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Tile * b=NULL, float in_scale=1.0f);
    void AssembleSprite(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags);

    bool IsValid();
    bool Invalidate();
    static bool InvalidateAndDestroy(Tile*);
	static bool CleanCreateAndValidate(Tile*, WorldSegment*, df::tiletype);
};

void createEffectSprites();
void destroyEffectSprites();
void initRandomCube();

//find a better place for these
bool hasWall(Tile* b);
bool hasBuildingOfID(Tile* b, int ID);
bool hasBuildingIdentity(Tile* b, Buildings::t_building* index, int buildingOcc);
bool hasBuildingOfIndex(Tile* b, Buildings::t_building* index);
bool wallShouldNotHaveBorders( int in );
bool containsDesignations( df::tile_designation, df::tile_occupancy );

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
    return (!FlowPassable( (tiletype::tiletype) in ));
}