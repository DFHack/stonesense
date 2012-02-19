#ifndef BLOCK_H
#define BLOCK_H

#include "common.h"
#include "SpriteObjects.h"
#include "TileTypes.h"
class WorldSegment;


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
    DFHack::Simple::Units::t_unit * creature;
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
	/*int16_t eff_miasma;
	int16_t eff_water;
	int16_t eff_water2;
	int16_t eff_blood;
	int16_t eff_dust;
	int16_t eff_magma;
	int16_t eff_smoke;
	int16_t eff_dragonfire;
	int16_t eff_fire;
	int16_t eff_boiling;
	int16_t eff_webing;
	int16_t eff_oceanwave;*/
	struct {
        DFHack::Simple::Buildings::t_building info;
		std::vector<c_sprite> sprites;
		uint32_t index;
		int32_t custom_building_type;
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

ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, DFHack::t_matglossPair material, DFHack::t_matglossPair layerMaterial, DFHack::t_matglossPair veinMaterial);
ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, DFHack::t_matglossPair material, DFHack::t_matglossPair layerMaterial, DFHack::t_matglossPair veinMaterial);

#endif