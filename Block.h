#pragma once

#include "common.h"
#include "SpriteObjects.h"
class WorldSegment;


class Block
{
public:
	Block(WorldSegment* ownerSegment);
	~Block(void);
	static void* operator new (size_t size); 
	static void operator delete (void *p);

	WorldSegment* ownerSegment;

	uint32_t x, y, z;
	int floorType;
	int wallType;
	int stairType;
	t_matglossPair material;
	t_matglossPair layerMaterial;
	t_matglossPair veinMaterial;
	bool hasVein;

	bool depthBorderNorth;
	bool depthBorderWest;
	bool depthBorderDown;
	int shadow;

	bool creaturePresent;
	bool buildingPresent;
	t_matglossPair ramp;
	t_matglossPair water;
	t_designation designation;
	t_occupancy occ;
	t_creature * creature;
	t_matglossPair tree;

	int mudlevel;
	int snowlevel;
	int bloodlevel;
	ALLEGRO_COLOR bloodcolor;
	struct BlockEffects //size 40
	{
		uint16_t count;
		uint16_t type;
		t_matglossPair material;
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
		t_building info;
		vector<c_sprite> sprites;
		uint32_t index;
	} building;

	bool IsVisible(){
		return (floorType || wallType) != 0;
	}
	void Draw();
	void Drawcreaturetext();
	void DrawRamptops();

	uint16_t temp1;
	uint16_t temp2;

	uint16_t consForm;


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

ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial);
ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial);