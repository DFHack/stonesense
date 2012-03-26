#pragma once

#include "common.h"
#include "SpriteObjects.h"
#include "TileTypes.h"
#include "df/item_type.h"

class WorldSegment;

struct Effect
{
	DFHack::t_matglossPair matt;
	int16_t density;
};

struct Worn_Item
{
	DFHack::t_matglossPair matt;
	bool dyed;
	int8_t rating;
	Worn_Item();
};

struct Unit_Inventory
{
	//[item_type][item_subtype][item_number]
	std::vector<std::vector<std::vector<Worn_Item>>> item;
};



class Block
{
public:
	Block(WorldSegment* ownerSegment);
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
    DFHack::t_matglossPair ramp;
    DFHack::t_matglossPair water;

	//following are neighbor water levels
    DFHack::t_matglossPair abovewater;
    DFHack::t_matglossPair belowwater;
    DFHack::t_matglossPair rightwater;
    DFHack::t_matglossPair leftwater;

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

	struct BlockEffects //size 40
	{
		uint16_t count;
		uint16_t type;
        DFHack::t_matglossPair material;
		int16_t lifetime;
		int16_t x_direction;
		int16_t y_direction;
		uint8_t canCreateNew;//??
	} blockeffects;

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

	bool IsVisible(){
		return (tileShapeBasic==df::enums::tiletype_shape_basic::Floor || tileShapeBasic==df::enums::tiletype_shape_basic::Wall) != 0;
	}
	void Draw();
	void Drawcreaturetext();
	void DrawRamptops();
	void DrawPixel(int drawx, int drawy);

	uint16_t temp1;
	uint16_t temp2;

	uint16_t consForm;

	bool obscuringCreature;
	bool obscuringBuilding;

	struct SS_Item
	{
		DFHack::t_matglossPair item;
		DFHack::t_matglossPair matt;
		bool dyed;
	} Item;

	//These are actually applied to the creature standing here, but there's only one creature shown, so it's okay.
	Unit_Inventory * inv;


private:

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
void drawFloorBlood ( Block *b, int32_t drawx, int32_t drawy );
