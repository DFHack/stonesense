#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "ContentLoader.h"
#include "SpriteColors.h"
#include "TileTypes.h"
#include "df/building_type.h"

ALLEGRO_BITMAP *sprite_miasma = 0;
ALLEGRO_BITMAP *sprite_water = 0;
ALLEGRO_BITMAP *sprite_water2 = 0;
ALLEGRO_BITMAP *sprite_blood = 0;
ALLEGRO_BITMAP *sprite_dust = 0;
ALLEGRO_BITMAP *sprite_magma = 0;
ALLEGRO_BITMAP *sprite_smoke = 0;
ALLEGRO_BITMAP *sprite_dragonfire = 0;
ALLEGRO_BITMAP *sprite_fire = 0;
ALLEGRO_BITMAP *sprite_webing = 0;
ALLEGRO_BITMAP *sprite_boiling = 0;
ALLEGRO_BITMAP *sprite_oceanwave = 0;

int randomCube[RANDOM_CUBE][RANDOM_CUBE][RANDOM_CUBE];

void initRandomCube()
{
    for(int i = 0; i < RANDOM_CUBE; i++)
        for(int j = 0; j < RANDOM_CUBE; j++)
            for(int k = 0; k < RANDOM_CUBE; k++) {
                randomCube[i][j][k] = rand();
            }
}

worn_item::worn_item()
{
    matt.index = -1;
    matt.type = -1;
    dyematt.index = -1;
    dyematt.type = -1;
}

Tile::Tile(WorldSegment* ownerSegment, df::tiletype type)
{
    Reset(ownerSegment, type);
}

/*
 * resets the state of the Tile, and validates it
 */
void Tile::Reset(WorldSegment* ownerSegment, df::tiletype type)
{
    //clear out own memory
    memset(this, 0, sizeof(Tile));

    valid=true;
    visible = true;

    this->ownerSegment = ownerSegment;

    building.type = (building_type::building_type) BUILDINGTYPE_NA;
    building.parent = 0;

    this->material.type = INVALID_INDEX;
    this->material.index = INVALID_INDEX;
    this->tileType = type;

    wallborders = 0;
    floorborders = 0;
    openborders = 255;
    rampborders = 0;
    upstairborders = 0;
    downstairborders = 0;
    lightborders = 255;
    creature = 0;
    engraving_character = 0;

    Item.item.type =-1;
    Item.matt.type=-1;
    Item.matt.index=-1;
    Item.dyematt.type=-1;
    Item.dyematt.index=-1;
}


Tile::~Tile(void)
{
    if(creature) {
        delete(creature);
        this->creature=NULL;
    }
    if(inv) {
        delete(inv);
        this->inv=NULL;
    }
    building.info = NULL;
}

/**
 * returns the validity of this Tile
 * Tiles that are not valid have undefined behavior
 */
bool Tile::IsValid()
{
    return valid;
}

/**
 * invalidates this Tile, and frees memory of any member objects 
 *  through the deconstructor
 */
bool Tile::Invalidate()
{
    if(!valid) {
        return false;
    }

    this->~Tile();

    valid=false;

    return true;
}

inline ALLEGRO_BITMAP* imageSheet(t_SpriteWithOffset sprite, ALLEGRO_BITMAP* defaultBmp)
{
    if (sprite.fileIndex == -1) {
        return defaultBmp;
    } else {
        return getImgFile(sprite.fileIndex);
    }
}

inline ALLEGRO_BITMAP* imageSheet(t_subSprite sprite, ALLEGRO_BITMAP* defaultBmp)
{
    if (sprite.fileIndex == -1) {
        return defaultBmp;
    } else {
        return getImgFile(sprite.fileIndex);
    }
}

void Tile::AssembleParticleCloud(int count, float centerX, float centerY, float rangeX, float rangeY, ALLEGRO_BITMAP *sprite, ALLEGRO_COLOR tint)
{
    for(int i = 0; i < count; i++) {
        int width = al_get_bitmap_width(sprite);
        int height = al_get_bitmap_height(sprite);
        float drawx = centerX + ((((float)rand() / RAND_MAX) - 0.5) * rangeX * ssConfig.scale);
        float drawy = centerY + ((((float)rand() / RAND_MAX) - 0.5) * rangeY * ssConfig.scale);
        AssembleSprite(sprite, tint, 0, 0, width, height, drawx, drawy,width*ssConfig.scale, height*ssConfig.scale, 0);
    }
}

void Tile::AssembleSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet, ALLEGRO_COLOR color, float x, float y, Tile * b, float in_scale)
{
    int sheetx = spriteNum % SHEET_OBJECTSWIDE;
    int sheety = spriteNum / SHEET_OBJECTSWIDE;
    AssembleSprite(
        spriteSheet,
        premultiply(b ? shadeAdventureMode(color, b->fog_of_war, b->designation.bits.outside) : color),
        sheetx * SPRITEWIDTH * in_scale,
        sheety * SPRITEHEIGHT * in_scale,
        SPRITEWIDTH * in_scale,
        SPRITEHEIGHT * in_scale,
        x,
        y - (WALLHEIGHT)*ssConfig.scale,
        SPRITEWIDTH*ssConfig.scale,
        SPRITEHEIGHT*ssConfig.scale,
        0);
}

void Tile::AssembleSprite(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR tint, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags)
{
    draw_event d = {TintedScaledBitmap, bitmap, tint, sx, sy, sw, sh, dx, dy, dw, dh, flags};
    ownerSegment->AssembleSprite(d);
}

void Tile::AssembleTile()
{

    if(!visible) {
        return;
    }

    if((material.type == INORGANIC) && (material.index == -1)) {
        material.index = 0;
    }

    bool defaultSnow = 1;
    t_SpriteWithOffset sprite;
    c_sprite* spriteobject;

    int32_t drawx = x;
    int32_t drawy = y;
    int32_t drawz = z;
    
    ownerSegment->CorrectTileForSegmentOffset( drawx, drawy, drawz);
    ownerSegment->CorrectTileForSegmentRotation( drawx, drawy, drawz);
    pointToScreen((int*)&drawx, (int*)&drawy, drawz);
    drawx -= (PLATEWIDTH>>1)*ssConfig.scale;
    
    //TODO the following check should get incorporated into segment beautification
    if(((drawx + PLATEWIDTH*ssConfig.scale) < 0) || (drawx > ssState.ScreenW) || ((drawy + (PLATEHEIGHT + FLOORHEIGHT)*ssConfig.scale) < 0) || (drawy - WALLHEIGHT*ssConfig.scale > ssState.ScreenH)) {
        return;
    }

    bool chopThisTile = 0;

    if(ssConfig.truncate_walls == 1) {
        chopThisTile = 1;
    } else if(ssConfig.truncate_walls == 2 && obscuringCreature == 1) {
        chopThisTile = 1;
    } else if(ssConfig.truncate_walls == 3 && (obscuringCreature == 1 || obscuringBuilding == 1)) {
        chopThisTile = 1;
    } else if(ssConfig.truncate_walls == 4 && obscuringBuilding == 1) {
        chopThisTile = 1;
    }

    if(building.type == BUILDINGTYPE_BLACKBOX) {
        AssembleSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy+FLOORHEIGHT*ssConfig.scale);
        AssembleSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy);
        return;
    }

    ALLEGRO_COLOR plateBorderColor = al_map_rgb(85,85,85);
    int rando = randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];


    //Draw Ramp Tops
    if(tileType == tiletype::RampTop){
        Tile * b = this->ownerSegment->getTile(this->x, this->y, this->z-1);
        if(b && b->tileShapeBasic() == tiletype_shape_basic::Ramp) {
            spriteobject = GetTileSpriteMap(b->tileType, b->material, b->consForm);
            if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
                spriteobject->set_sheetindex(0);
                spriteobject->set_fileindex(INVALID_INDEX);
                spriteobject->set_defaultsheet(IMGRampSheet);
            }
            if (spriteobject->get_sheetindex() != INVALID_INDEX) {
                spriteobject->set_size(SPRITEWIDTH, PLATEHEIGHT);
                spriteobject->set_plate_layout(RAMPTOPPLATE);
                spriteobject->set_offset(0, WALLHEIGHT);
                spriteobject->assemble_world_offset_src(x, y, z, 0, this, b, (chopThisTile && this->z == ownerSegment->pos.z + ownerSegment->size.z -2));
                spriteobject->set_offset(0, 0);
            }
            spriteobject->set_plate_layout(TILEPLATE);
        }
    }

    //Draw Floor
    if( tileShapeBasic()==tiletype_shape_basic::Floor ||
            tileShapeBasic()==tiletype_shape_basic::Wall ||
            tileShapeBasic()==tiletype_shape_basic::Ramp ||
            tileShapeBasic()==tiletype_shape_basic::Stair) {

        //If plate has no floor, look for a Filler Floor from it's wall
        if (tileShapeBasic()==tiletype_shape_basic::Floor) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic()==tiletype_shape_basic::Wall) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic()==tiletype_shape_basic::Ramp) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic()==tiletype_shape_basic::Stair) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        }
        if(spriteobject->get_sheetindex() != INVALID_INDEX) {
            if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
                spriteobject->set_sheetindex(SPRITEOBJECT_FLOOR_NA);
                spriteobject->set_fileindex(INVALID_INDEX);
                spriteobject->set_offset(0, WALLHEIGHT);
                spriteobject->assemble_world(x, y, z, this);
            } else {
                spriteobject->assemble_world(x, y, z, this);
            }
        }
    }

    //Floor Engravings
    if((tileShapeBasic()==tiletype_shape_basic::Floor) && engraving_character && engraving_flags.bits.floor) {
        AssembleSpriteFromSheet( engraving_character, IMGEngFloorSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
    }

    //draw surf
    //fixme: needs to be scaled
    if(Eff_SeaFoam.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_SeaFoam.matt);
        tint.a*=Eff_SeaFoam.density/100.0f;
        int foamw=al_get_bitmap_width(sprite_oceanwave);
        int foamh=al_get_bitmap_height(sprite_oceanwave);
        AssembleSprite(
            sprite_oceanwave,
            tint,
            0,
            0,
            foamw,
            foamh,
            drawx,
            drawy - (WALLHEIGHT)*ssConfig.scale,
            SPRITEWIDTH*ssConfig.scale,
            SPRITEHEIGHT*ssConfig.scale,
            0);
    }

    //Draw Ramp
    if(tileShapeBasic()==tiletype_shape_basic::Ramp) {
        spriteobject = GetTileSpriteMap(tileType, material, consForm);
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(0);
            spriteobject->set_fileindex(INVALID_INDEX);
            spriteobject->set_defaultsheet(IMGRampSheet);
        }
        if (spriteobject->get_sheetindex() != INVALID_INDEX) {
            spriteobject->set_size(SPRITEWIDTH, SPRITEHEIGHT);
            spriteobject->set_plate_layout(RAMPBOTTOMPLATE);
            spriteobject->assemble_world_offset(x, y, z, 0, this, (chopThisTile && this->z == ownerSegment->pos.z + ownerSegment->size.z -2));
        }
        spriteobject->set_plate_layout(TILEPLATE);
    }

    AssembleFloorBlood ( drawx, drawy );

    //first part of snow
    if(tileShapeBasic()!=tiletype_shape_basic::Ramp && tileShapeBasic()!=tiletype_shape_basic::Wall && tileShapeBasic()!=tiletype_shape_basic::Stair && defaultSnow) {
        if(snowlevel > 75) {
            AssembleSpriteFromSheet( 20, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this);
        } else if(snowlevel > 50) {
            AssembleSpriteFromSheet( 21, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 25) {
            AssembleSpriteFromSheet( 22, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 0) {
            AssembleSpriteFromSheet( 23, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        }
    }

    ////vegetation
    //if(tree.index > 0 || tree.type > 0){
    //	c_sprite * vegetationsprite = 0;
    //	vegetationsprite = GetSpriteVegetation( (TileClass) getVegetationType( this->floorType ), tree.index );
    //	if(vegetationsprite)
    //		vegetationsprite->assemble_world(x, y, z);
    //}

    //items
    if(Item.item.type >= 0) {
        if(
            contentLoader->itemConfigs[Item.item.type] &&
            (Item.item.index < contentLoader->itemConfigs[Item.item.type]->subItems.size()) &&
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]) {
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]->sprite.assemble_world(x, y, z, this);
        } else if (
            contentLoader->itemConfigs[Item.item.type] &&
            contentLoader->itemConfigs[Item.item.type]->configured) {
            contentLoader->itemConfigs[Item.item.type]->default_sprite.assemble_world(x, y, z, this);
        } else {
            AssembleSpriteFromSheet( 350, IMGObjectSheet, lookupMaterialColor(Item.matt, Item.dyematt), drawx, (tileShapeBasic()==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*ssConfig.scale)):drawy , this);
        }
    }

    //shadow
    if (shadow > 0) {
        AssembleSpriteFromSheet( BASE_SHADOW_PLATE + shadow - 1, IMGObjectSheet, al_map_rgb(255,255,255), drawx, (tileShapeBasic()==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*ssConfig.scale)):drawy , this);
    }

    //Building
    bool skipBuilding =
        (building.type == building_type::Civzone && !ssConfig.show_stockpiles) ||
        (building.type == building_type::Stockpile && !ssConfig.show_zones);

    if(building.type != BUILDINGTYPE_NA && !skipBuilding) {
        for(uint32_t i=0; i < building.sprites.size(); i++) {
            spriteobject = &building.sprites[i];
            if(building.parent) {
                spriteobject->assemble_world(x, y, z, building.parent);
            } else {
                spriteobject->assemble_world(x, y, z, this);
            }
        }
    }

    //Draw Stairs
    if(tileShapeBasic()==tiletype_shape_basic::Stair) {
        bool mirrored = false;
        if(findWallCloseTo(ownerSegment, this) == eSimpleW) {
            mirrored = true;
        }

        //down part
        spriteobject = GetFloorSpriteMap(tileType, material, consForm);
        if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX) {
            if (mirrored) {
                spriteobject->assemble_world_offset(x, y, z, 1, this);
            } else {
                spriteobject->assemble_world(x, y, z, this);
            }
        }

        //up part
        spriteobject = GetTileSpriteMap(tileType, material, consForm);
        if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX) {
            if (mirrored) {
                spriteobject->assemble_world_offset(x, y, z, 1, this);
            } else {
                spriteobject->assemble_world(x, y, z, this);
            }
        }
    }

    if(tileShapeBasic()==tiletype_shape_basic::Wall) {
        //draw wall
        spriteobject =  GetTileSpriteMap(tileType, material, consForm);
        int spriteOffset = 0;
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(SPRITEOBJECT_WALL_NA);
            spriteobject->set_fileindex(INVALID_INDEX);
            spriteobject->set_plate_layout(TILEPLATE);
            spriteobject->set_defaultsheet(IMGObjectSheet);
        }
        if (spriteobject->get_sheetindex() == INVALID_INDEX) {
            //skip
        } else {
            spriteobject->assemble_world(x, y, z, this, (chopThisTile && this->z == ownerSegment->pos.z + ownerSegment->size.z -2));
        }
    }

    //Wall Engravings
    if((tileShapeBasic()==tiletype_shape_basic::Wall) && engraving_character) {
        if(ownerSegment->rotation == 0) {
            if(engraving_flags.bits.east) {
                AssembleSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.south) {
                AssembleSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 1) {
            if(engraving_flags.bits.north) {
                AssembleSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.east) {
                AssembleSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 2) {
            if(engraving_flags.bits.west) {
                AssembleSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.north) {
                AssembleSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 3) {
            if(engraving_flags.bits.south) {
                AssembleSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.west) {
                AssembleSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
    }


    if(designation.bits.flow_size > 0) {
        //if(waterlevel == 7) waterlevel--;
        uint32_t waterlevel = designation.bits.flow_size + (deepwater ? 1 : 0);
        if(designation.bits.liquid_type == 0) {
            contentLoader->water[waterlevel-1].sprite.assemble_world(x, y, z, this, (chopThisTile && this->z == ownerSegment->pos.z + ownerSegment->size.z -2));
        } else {
            contentLoader->lava[waterlevel-1].sprite.assemble_world(x, y, z, this, (chopThisTile && this->z == ownerSegment->pos.z + ownerSegment->size.z -2));
        }
    }

    // creature
    if(occ.bits.unit && creature && (ssConfig.show_hidden_tiles || !designation.bits.hidden)) {
        AssembleCreature(drawx, drawy, creature, this);
    }

    //second part of snow
    if(tileShapeBasic()!=tiletype_shape_basic::Wall && tileShapeBasic()!=tiletype_shape_basic::Stair && defaultSnow) {
        if(snowlevel > 75) {
            AssembleSpriteFromSheet( 24, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 50) {
            AssembleSpriteFromSheet( 25, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 25) {
            AssembleSpriteFromSheet( 26, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        }
    }

    if(Eff_Web.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Web.matt);
        tint.a*=Eff_Web.density/100.0f;
        AssembleSpriteFromSheet(rando%5, sprite_webing, tint, drawx, drawy, this, 4.0f);
        //al_draw_tinted_bitmap(sprite_webing,tint, drawx, drawy - (WALLHEIGHT), 0);
    }
    if(Eff_Miasma.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Miasma.matt);
        AssembleParticleCloud(Eff_Miasma.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_miasma, tint);
    }
    if(Eff_Steam.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Steam.matt);
        AssembleParticleCloud(Eff_Steam.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
    }
    if(Eff_Mist.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Mist.matt);
        AssembleParticleCloud(Eff_Mist.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water2, tint);
    }
    if(Eff_MaterialDust.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialDust.matt);
        AssembleParticleCloud(Eff_MaterialDust.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dust, tint);
    }
    if(Eff_MagmaMist.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MagmaMist.matt);
        AssembleParticleCloud(Eff_MagmaMist.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_magma, tint);
    }
    if(Eff_Smoke.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Smoke.matt);
        AssembleParticleCloud(Eff_Smoke.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke, tint);
    }
    if(Eff_Dragonfire.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Dragonfire.matt);
        tint.a*=Eff_Dragonfire.density/100.0f;
        tint.g*=Eff_Dragonfire.density/100.0f;
        tint.b*=Eff_Dragonfire.density/100.0f;
        int size = 3 - ((Eff_Dragonfire.density-1)/25);
        AssembleSpriteFromSheet((((currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
        //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Dragonfire.matt.type, Eff_Dragonfire.matt.index);
        //draw_particle_cloud(Eff_Dragonfire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dragonfire, tint);
    }
    if(Eff_Fire.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Fire.matt);
        tint.a*=Eff_Fire.density/100.0f;
        tint.g*=Eff_Fire.density/100.0f;
        tint.b*=Eff_Fire.density/100.0f;
        int size = 3 - ((Eff_Fire.density-1)/25);
        AssembleSpriteFromSheet((((currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
        //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Fire.matt.type, Eff_Fire.matt.index);
        //draw_particle_cloud(Eff_Fire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_fire, tint);
    }
    if(Eff_MaterialGas.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialGas.matt);
        AssembleParticleCloud(Eff_MaterialGas.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
    }
    if(Eff_MaterialVapor.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialVapor.matt);
        AssembleParticleCloud(Eff_MaterialVapor.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
    }
    if(Eff_OceanWave.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_OceanWave.matt);
        AssembleParticleCloud(Eff_OceanWave.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
    }
    
    if(occ.bits.unit && creature && (ssConfig.show_hidden_tiles || !designation.bits.hidden)) {
        AssembleCreatureText(drawx, drawy, creature, ownerSegment);
    }
}

//void Tile::DrawPixel(int drawx, int drawy)
//{
//    ALLEGRO_COLOR temp;
//    if(
//        tileShapeBasic==tiletype_shape_basic::Floor ||
//        tileShapeBasic==tiletype_shape_basic::Wall ||
//        tileShapeBasic==tiletype_shape_basic::Ramp ||
//        tileShapeBasic==tiletype_shape_basic::Stair
//    ) {
//        al_put_pixel(drawx, drawy, lookupMaterialColor(this->material));
//    }
//    if(this->water.index) {
//        if(this->water.type == 0) { //water
//            al_draw_pixel(drawx, drawy, al_map_rgba_f(0.6f, 0.85f, 0.92f, (float)water.index / 7.0f));
//        } else {
//            al_draw_pixel(drawx, drawy, al_map_rgba_f(1.0f, 0.5f, 0.15f, (float)water.index / 7.0f));
//        }
//    }
//    //Grass
//    if(grasslevel > 0 && (
//                (tileMaterial == tiletype_material::GRASS_DARK) ||
//                (tileMaterial == tiletype_material::GRASS_DARK) ||
//                (tileMaterial == tiletype_material::GRASS_DRY) ||
//                (tileMaterial == tiletype_material::GRASS_DEAD))) {
//        temp = lookupMaterialColor(WOOD, grassmat);
//        al_draw_pixel(drawx, drawy, al_map_rgba_f(temp.r,temp.g, temp.b, (float)grasslevel/100.0f));
//    }
//}

bool hasWall(Tile* b)
{
    if(!b) {
        return false;
    }
    return b->tileShapeBasic()==tiletype_shape_basic::Wall;
}

bool hasBuildingOfID(Tile* b, int ID)
{
    if(!b) {
        return false;
    }
    return b->building.type == ID;
}

bool hasBuildingIdentity(Tile* b, Buildings::t_building* index, int buildingOcc)
{
    if(!b) {
        return false;
    }
    if (!(b->building.info == index)) {
        return false;
    }
    return b->occ.bits.building == buildingOcc;
}

bool hasBuildingOfIndex(Tile* b, Buildings::t_building* index)
{
    if(!b) {
        return false;
    }
    return b->building.info == index;
}

bool wallShouldNotHaveBorders( int in )
{
    switch( in ) {
    case 65: //stone fortification
    case 436: //minstone fortification
    case 326: //lavastone fortification
    case 327: //featstone fortification
    case 494: //constructed fortification
        return true;
        break;
    };
    return false;
}

void createEffectSprites()
{
    sprite_miasma		= CreateSpriteFromSheet( 180, IMGObjectSheet);
    sprite_water		= CreateSpriteFromSheet( 181, IMGObjectSheet);
    sprite_water2		= CreateSpriteFromSheet( 182, IMGObjectSheet);
    sprite_blood		= CreateSpriteFromSheet( 183, IMGObjectSheet);
    sprite_dust			= CreateSpriteFromSheet( 182, IMGObjectSheet);
    sprite_magma		= CreateSpriteFromSheet( 185, IMGObjectSheet);
    sprite_smoke		= CreateSpriteFromSheet( 186, IMGObjectSheet);
    sprite_dragonfire	= load_bitmap_withWarning("stonesense/Effect_flames.png");
    sprite_fire			= CreateSpriteFromSheet( 188, IMGObjectSheet);
    sprite_webing		= load_bitmap_withWarning("stonesense/Effect_web.png");
    sprite_boiling		= CreateSpriteFromSheet( 190, IMGObjectSheet);
    sprite_oceanwave	= CreateSpriteFromSheet( 191, IMGObjectSheet);
}

void destroyEffectSprites()
{
    al_destroy_bitmap(sprite_miasma);
    al_destroy_bitmap(sprite_water);
    al_destroy_bitmap(sprite_water2);
    al_destroy_bitmap(sprite_blood);
    al_destroy_bitmap(sprite_dust);
    al_destroy_bitmap(sprite_magma);
    al_destroy_bitmap(sprite_smoke);
    al_destroy_bitmap(sprite_dragonfire);
    al_destroy_bitmap(sprite_fire);
    al_destroy_bitmap(sprite_webing);
    al_destroy_bitmap(sprite_boiling);
    al_destroy_bitmap(sprite_oceanwave);
}


void Tile::AssembleFloorBlood ( int32_t drawx, int32_t drawy )
{
    t_SpriteWithOffset sprite;

    if( designation.bits.flow_size < 1 && (bloodlevel)) {
        sprite.fileIndex = INVALID_INDEX;

        // Spatter (should be blood, not blood2) swapped for testing
        if( bloodlevel <= ssConfig.poolcutoff ) {
            sprite.sheetIndex = 7;
        }

        // Smear (should be blood2, not blood) swapped for testing
        else {
            // if there's no tile in the respective direction it's false. if there's no blood in that direction it's false too. should also check to see if there's a ramp below, but since blood doesn't flow, that'd look wrong anyway.
            bool _N = ( ownerSegment->getTileRelativeTo( x, y, z, eUp ) != NULL ? (ownerSegment->getTileRelativeTo( x, y, z, eUp )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _S = ( ownerSegment->getTileRelativeTo( x, y, z, eDown ) != NULL ? (ownerSegment->getTileRelativeTo( x, y, z, eDown )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _E = ( ownerSegment->getTileRelativeTo( x, y, z, eRight ) != NULL ? (ownerSegment->getTileRelativeTo( x, y, z, eRight )->bloodlevel > ssConfig.poolcutoff) : false ),
                 _W = ( ownerSegment->getTileRelativeTo( x, y, z, eLeft ) != NULL ? (ownerSegment->getTileRelativeTo( x, y, z, eLeft )->bloodlevel > ssConfig.poolcutoff) : false );

            // do rules-based puddling
            if( _N || _S || _E || _W ) {
                if( _E ) {
                    if( _N && _S ) {
                        sprite.sheetIndex = 5;
                    } else if( _S ) {
                        sprite.sheetIndex = 3;
                    } else if( _W ) {
                        sprite.sheetIndex = 1;
                    } else {
                        sprite.sheetIndex = 6;
                    }
                } else if( _W ) {
                    if( _S && _N) {
                        sprite.sheetIndex = 5;
                    } else if( _S ) {
                        sprite.sheetIndex = 2;
                    } else {
                        sprite.sheetIndex = 0;
                    }
                } else if ( _N ) {
                    sprite.sheetIndex = 4;
                } else {
                    sprite.sheetIndex = 2;
                }
            } else {
                sprite.sheetIndex = 8;
            }
        }

        int sheetOffsetX = PLATEWIDTH * (sprite.sheetIndex % SHEET_OBJECTSWIDE),
            sheetOffsetY = 0;

        AssembleSprite( 
            IMGBloodSheet,
            premultiply(bloodcolor),
            sheetOffsetX,
            sheetOffsetY,
            PLATEWIDTH,
            PLATEHEIGHT+FLOORHEIGHT,
            drawx,
            drawy,
            PLATEWIDTH*ssConfig.scale,
            (PLATEHEIGHT+FLOORHEIGHT)*ssConfig.scale,
            0);
        AssembleSprite(
            IMGBloodSheet,
            al_map_rgb(255,255,255),
            sheetOffsetX,
            sheetOffsetY+PLATEHEIGHT+FLOORHEIGHT,
            PLATEWIDTH,
            PLATEHEIGHT+FLOORHEIGHT,
            drawx,
            drawy,
            PLATEWIDTH*ssConfig.scale,
            (PLATEHEIGHT+FLOORHEIGHT)*ssConfig.scale,
            0);
    }
}

