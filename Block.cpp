#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "BlockFactory.h"
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

Worn_Item::Worn_Item()
{
    matt.index = -1;
    matt.type = -1;
    dyematt.index = -1;
    dyematt.type = -1;
}

Block::Block(WorldSegment* ownerSegment, df::tiletype type)
{
    //clear out own memory
    memset(this, 0, sizeof(Block));

    this->ownerSegment = ownerSegment;

    building.info.type = (building_type::building_type) BUILDINGTYPE_NA;
    building.index = -1;
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
    visible = true;

    Item.item.type =-1;
    Item.matt.type=-1;
    Item.matt.index=-1;
    Item.dyematt.type=-1;
    Item.dyematt.index=-1;
}

Block::~Block(void)
{
    if( creature ) {
        delete(creature);
    }
    if(inv) {
        delete(inv);
    }
}

void* Block::operator new (size_t size)
{
    return blockFactory.allocateBlock( );
}

void Block::operator delete (void *p)
{
    blockFactory.deleteBlock( (Block*)p );
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

void Block::Draw()
{
    if(!visible) {
        return;
    }

    if((material.type == INORGANIC) && (material.index == -1)) {
        material.index = 0;
    }

    if((!(this->designation.bits.hidden) || config.show_hidden_blocks) && config.block_count) {
        config.tilecount ++;
    }

    bool defaultSnow = 1;
    t_SpriteWithOffset sprite;
    c_sprite* spriteobject;

    drawx = x;
    drawy = y;
    drawz = z;

    correctBlockForSegmetOffset( drawx, drawy, drawz);
    correctBlockForRotation( drawx, drawy, drawz, ownerSegment->rotation);
    pointToScreen((int*)&drawx, (int*)&drawy, drawz);
    drawx -= (TILEWIDTH>>1)*config.scale;

    //TODO the following check should get incorporated into segment beautification
    if(((drawx + TILEWIDTH*config.scale) < 0) || (drawx > al_get_bitmap_width(al_get_target_bitmap())) || ((drawy + (TILEHEIGHT + FLOORHEIGHT)*config.scale) < 0) || (drawy - WALLHEIGHT*config.scale > al_get_bitmap_height(al_get_target_bitmap()))) {
        return;
    }

    bool chopThisBlock = 0;

    if(config.truncate_walls == 1) {
        chopThisBlock = 1;
    } else if(config.truncate_walls == 2 && obscuringCreature == 1) {
        chopThisBlock = 1;
    } else if(config.truncate_walls == 3 && (obscuringCreature == 1 || obscuringBuilding == 1)) {
        chopThisBlock = 1;
    } else if(config.truncate_walls == 4 && obscuringBuilding == 1) {
        chopThisBlock = 1;
    }

    if(building.info.type == BUILDINGTYPE_BLACKBOX) {
        DrawSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy+FLOORHEIGHT*config.scale);
        DrawSpriteFromSheet( SPRITEOBJECT_BLACK, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy);
        return;
    }


    ALLEGRO_COLOR tileBorderColor = al_map_rgb(85,85,85);
    int rando = randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];
    //Draw Floor
    if( tileShapeBasic==tiletype_shape_basic::Floor ||
            tileShapeBasic==tiletype_shape_basic::Wall ||
            tileShapeBasic==tiletype_shape_basic::Ramp ||
            tileShapeBasic==tiletype_shape_basic::Stair ) {

        //If tile has no floor, look for a Filler Floor from it's wall
        if (tileShapeBasic==tiletype_shape_basic::Floor) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic==tiletype_shape_basic::Wall) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic==tiletype_shape_basic::Ramp) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        } else if (tileShapeBasic==tiletype_shape_basic::Stair) {
            spriteobject = GetFloorSpriteMap(tileType, this->material, consForm);
        }
        if(spriteobject->get_sheetindex() != INVALID_INDEX) {
            ////if floor is muddy, override regular floor
            //if( mudlevel && water.index == 0)
            //{
            //	sprite.sheetIndex = SPRITEFLOOR_WATERFLOOR;
            //	sprite.fileIndex = INVALID_INDEX;
            //}
            ////if floor is snowed down, override  regular floor
            //if( snowlevel )
            //{
            //	sprite.sheetIndex = SPRITEFLOOR_SNOW;
            //	sprite.fileIndex = INVALID_INDEX;
            //	spriteOffset = 0;
            //	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
            //}

            if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
                spriteobject->set_sheetindex(SPRITEOBJECT_FLOOR_NA);
                spriteobject->set_fileindex(INVALID_INDEX);
                spriteobject->set_offset(0, WALLHEIGHT);
                spriteobject->draw_world(x, y, z, this);
            } else {
                spriteobject->draw_world(x, y, z, this);
            }
        }
    }

    //Floor Engravings
    if((tileShapeBasic==tiletype_shape_basic::Floor) && engraving_character && engraving_flags.bits.floor) {
        DrawSpriteFromSheet( engraving_character, IMGEngFloorSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
    }

    //draw surf
    //fixme: needs to be scaled
    if(Eff_SeaFoam.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_SeaFoam.matt);
        tint.a*=Eff_SeaFoam.density/100.0f;
        al_draw_tinted_bitmap(sprite_oceanwave,tint, drawx, drawy - (WALLHEIGHT), 0);
    }

    //Draw Ramp
    if(tileShapeBasic==tiletype_shape_basic::Ramp) {
        spriteobject = GetBlockSpriteMap(tileType, material, consForm);
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(0);
            spriteobject->set_fileindex(INVALID_INDEX);
            spriteobject->set_defaultsheet(IMGRampSheet);
        }
        if (spriteobject->get_sheetindex() != INVALID_INDEX) {
            spriteobject->set_size(SPRITEWIDTH, SPRITEHEIGHT);
            spriteobject->set_tile_layout(RAMPBOTTOMTILE);
            spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
        }
        spriteobject->set_tile_layout(BLOCKTILE);
    }

    drawFloorBlood ( this, drawx, drawy );

    //first part of snow
    if(tileShapeBasic!=tiletype_shape_basic::Ramp && tileShapeBasic!=tiletype_shape_basic::Wall && tileShapeBasic!=tiletype_shape_basic::Stair && defaultSnow) {
        if(snowlevel > 75) {
            DrawSpriteFromSheet( 20, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this);
        } else if(snowlevel > 50) {
            DrawSpriteFromSheet( 21, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 25) {
            DrawSpriteFromSheet( 22, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 0) {
            DrawSpriteFromSheet( 23, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        }
    }

    ////vegetation
    //if(tree.index > 0 || tree.type > 0){
    //	c_sprite * vegetationsprite = 0;
    //	vegetationsprite = GetSpriteVegetation( (TileClass) getVegetationType( this->floorType ), tree.index );
    //	if(vegetationsprite)
    //		vegetationsprite->draw_world(x, y, z);
    //}

    //items
    if(Item.item.type >= 0) {
        if(
            contentLoader->itemConfigs[Item.item.type] &&
            (Item.item.index < contentLoader->itemConfigs[Item.item.type]->subItems.size()) &&
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]) {
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]->sprite.draw_world(x, y, z, this);
        } else if (
            contentLoader->itemConfigs[Item.item.type] &&
            contentLoader->itemConfigs[Item.item.type]->configured) {
            contentLoader->itemConfigs[Item.item.type]->default_sprite.draw_world(x, y, z, this);
        } else {
            DrawSpriteFromSheet( 350, IMGObjectSheet, lookupMaterialColor(Item.matt, Item.dyematt), drawx, (tileShapeBasic==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*config.scale)):drawy , this);
        }
    }

    //shadow
    if (shadow > 0) {
        DrawSpriteFromSheet( BASE_SHADOW_TILE + shadow - 1, IMGObjectSheet, al_map_rgb(255,255,255), drawx, (tileShapeBasic==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*config.scale)):drawy , this);
    }

    //Building
    bool skipBuilding =
        (building.info.type == building_type::Civzone && !config.show_stockpiles) ||
        (building.info.type == building_type::Stockpile && !config.show_zones);

    if(building.info.type != BUILDINGTYPE_NA && !skipBuilding) {
        for(uint32_t i=0; i < building.sprites.size(); i++) {
            spriteobject = &building.sprites[i];
            if(building.parent) {
                spriteobject->draw_world(x, y, z, building.parent);
            } else {
                spriteobject->draw_world(x, y, z, this);
            }
        }
    }

    //Draw Stairs
    if(tileShapeBasic==tiletype_shape_basic::Stair) {
        bool mirrored = false;
        if(findWallCloseTo(ownerSegment, this) == eSimpleW) {
            mirrored = true;
        }

        //down part
        spriteobject = GetFloorSpriteMap(tileType, material, consForm);
        if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX) {
            if (mirrored) {
                spriteobject->draw_world_offset(x, y, z, this, 1);
            } else {
                spriteobject->draw_world(x, y, z, this);
            }
        }

        //up part
        spriteobject = GetBlockSpriteMap(tileType, material, consForm);
        if(spriteobject->get_sheetindex() != INVALID_INDEX && spriteobject->get_sheetindex() != UNCONFIGURED_INDEX) {
            if (mirrored) {
                spriteobject->draw_world_offset(x, y, z, this, 1);
            } else {
                spriteobject->draw_world(x, y, z, this);
            }
        }
    }

    if(tileShapeBasic==tiletype_shape_basic::Wall) {
        //draw wall
        spriteobject =  GetBlockSpriteMap(tileType, material, consForm);
        int spriteOffset = 0;
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(SPRITEOBJECT_WALL_NA);
            spriteobject->set_fileindex(INVALID_INDEX);
            spriteobject->set_tile_layout(BLOCKTILE);
            spriteobject->set_defaultsheet(IMGObjectSheet);
        }
        if (spriteobject->get_sheetindex() == INVALID_INDEX) {
            //skip
        } else {
            spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
        }
    }

    //Wall Engravings
    if((tileShapeBasic==tiletype_shape_basic::Wall) && engraving_character) {
        if(ownerSegment->rotation == 0) {
            if(engraving_flags.bits.east) {
                DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.south) {
                DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 1) {
            if(engraving_flags.bits.north) {
                DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.east) {
                DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 2) {
            if(engraving_flags.bits.west) {
                DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.north) {
                DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->rotation == 3) {
            if(engraving_flags.bits.south) {
                DrawSpriteFromSheet( engraving_character, IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.west) {
                DrawSpriteFromSheet( engraving_character, IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
    }


    if(water.index > 0) {
        //if(waterlevel == 7) waterlevel--;
        if(water.type == 0) {
            contentLoader->water[water.index-1].sprite.draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
        } else {
            contentLoader->lava[water.index-1].sprite.draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
        }
    }

    // creature
    // ensure there is *some* creature according to the map data
    // (no guarantee it is the right one)
    if(creaturePresent && (config.show_hidden_blocks || !designation.bits.hidden)) {
        DrawCreature(drawx, drawy, creature, this);
    }

    //second part of snow
    if(tileShapeBasic!=tiletype_shape_basic::Wall && tileShapeBasic!=tiletype_shape_basic::Stair && defaultSnow) {
        if(snowlevel > 75) {
            DrawSpriteFromSheet( 24, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 50) {
            DrawSpriteFromSheet( 25, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        } else if(snowlevel > 25) {
            DrawSpriteFromSheet( 26, IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy, this );
        }
    }

    if(Eff_Web.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Web.matt);
        tint.a*=Eff_Web.density/100.0f;
        DrawSpriteFromSheet(rando%5, sprite_webing, tint, drawx, drawy, this, 4.0f);
        //al_draw_tinted_bitmap(sprite_webing,tint, drawx, drawy - (WALLHEIGHT), 0);
    }
    if(Eff_Miasma.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Miasma.matt);
        draw_particle_cloud(Eff_Miasma.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_miasma, tint);
    }
    if(Eff_Steam.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Steam.matt);
        draw_particle_cloud(Eff_Steam.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
    }
    if(Eff_Mist.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Mist.matt);
        draw_particle_cloud(Eff_Mist.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water2, tint);
    }
    if(Eff_MaterialDust.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialDust.matt);
        draw_particle_cloud(Eff_MaterialDust.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dust, tint);
    }
    if(Eff_MagmaMist.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MagmaMist.matt);
        draw_particle_cloud(Eff_MagmaMist.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_magma, tint);
    }
    if(Eff_Smoke.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Smoke.matt);
        draw_particle_cloud(Eff_Smoke.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke, tint);
    }
    if(Eff_Dragonfire.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Dragonfire.matt);
        tint.a*=Eff_Dragonfire.density/100.0f;
        tint.g*=Eff_Dragonfire.density/100.0f;
        tint.b*=Eff_Dragonfire.density/100.0f;
        int size = 3 - ((Eff_Dragonfire.density-1)/25);
        DrawSpriteFromSheet((((currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
        //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Dragonfire.matt.type, Eff_Dragonfire.matt.index);
        //draw_particle_cloud(Eff_Dragonfire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dragonfire, tint);
    }
    if(Eff_Fire.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Fire.matt);
        tint.a*=Eff_Fire.density/100.0f;
        tint.g*=Eff_Fire.density/100.0f;
        tint.b*=Eff_Fire.density/100.0f;
        int size = 3 - ((Eff_Fire.density-1)/25);
        DrawSpriteFromSheet((((currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
        //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Fire.matt.type, Eff_Fire.matt.index);
        //draw_particle_cloud(Eff_Fire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_fire, tint);
    }
    if(Eff_MaterialGas.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialGas.matt);
        draw_particle_cloud(Eff_MaterialGas.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
    }
    if(Eff_MaterialVapor.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_MaterialVapor.matt);
        draw_particle_cloud(Eff_MaterialVapor.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
    }
    if(Eff_OceanWave.density > 0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(Eff_OceanWave.matt);
        draw_particle_cloud(Eff_OceanWave.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
    }
}

void Block::Drawcreaturetext()
{
    t_SpriteWithOffset sprite;
    /*if(config.hide_outer_blocks){
    if(x == ownerSegment->x || x == ownerSegment->x + ownerSegment->sizex - 1) return;
    if(y == ownerSegment->y || y == ownerSegment->y + ownerSegment->sizey - 1) return;
    }*/

    int32_t drawx = x;
    int32_t drawy = y;
    int32_t drawz = z; //- ownerSegment->sizez + 1;

    correctBlockForSegmetOffset( drawx, drawy, drawz);
    correctBlockForRotation( drawx, drawy, drawz, ownerSegment->rotation);
    pointToScreen((int*)&drawx, (int*)&drawy, drawz);
    drawx -= TILEWIDTH>>1;

    // creature
    // ensure there is *some* creature according to the map data
    // (no guarantee it is the right one)
    if(creaturePresent && (config.show_hidden_blocks || !designation.bits.hidden)) {
        DrawCreatureText(drawx, drawy, creature);
    }

}

void Block::DrawRamptops()
{
    if(!visible) {
        return;
    }
    if (tileShapeBasic==tiletype_shape_basic::Ramp) {

        bool chopThisBlock = 0;

        if(config.truncate_walls == 1) {
            chopThisBlock = 1;
        } else if(config.truncate_walls == 2 && obscuringCreature == 1) {
            chopThisBlock = 1;
        } else if(config.truncate_walls == 3 && (obscuringCreature == 1 || obscuringBuilding == 1)) {
            chopThisBlock = 1;
        } else if(config.truncate_walls == 4 && obscuringBuilding == 1) {
            chopThisBlock = 1;
        }
        //Draw Ramp
        c_sprite * spriteobject = GetBlockSpriteMap(tileType,material, consForm);
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(0);
            spriteobject->set_fileindex(INVALID_INDEX);
        }
        if (spriteobject->get_sheetindex() != INVALID_INDEX) {
            spriteobject->set_size(SPRITEWIDTH, TILEHEIGHT);
            spriteobject->set_offset(0, -(FLOORHEIGHT));
            spriteobject->set_tile_layout(RAMPTOPTILE);
            spriteobject->set_defaultsheet(IMGRampSheet);
            spriteobject->draw_world(x, y, z, this, (chopThisBlock && this->z == ownerSegment->z + ownerSegment->sizez -2));
            spriteobject->set_offset(0, 0);
        }
        spriteobject->set_tile_layout(BLOCKTILE);
    }
}

void Block::DrawPixel(int drawx, int drawy)
{
    ALLEGRO_COLOR temp;
    if(
        tileShapeBasic==tiletype_shape_basic::Floor ||
        tileShapeBasic==tiletype_shape_basic::Wall ||
        tileShapeBasic==tiletype_shape_basic::Ramp ||
        tileShapeBasic==tiletype_shape_basic::Stair
    ) {
        al_put_pixel(drawx, drawy, lookupMaterialColor(this->material));
    }
    if(this->water.index) {
        if(this->water.type == 0) { //water
            al_draw_pixel(drawx, drawy, al_map_rgba_f(0.6f, 0.85f, 0.92f, (float)water.index / 7.0f));
        } else {
            al_draw_pixel(drawx, drawy, al_map_rgba_f(1.0f, 0.5f, 0.15f, (float)water.index / 7.0f));
        }
    }
    //Grass
    if(grasslevel > 0 && (
                (tileMaterial == tiletype_material::GRASS_DARK) ||
                (tileMaterial == tiletype_material::GRASS_DARK) ||
                (tileMaterial == tiletype_material::GRASS_DRY) ||
                (tileMaterial == tiletype_material::GRASS_DEAD))) {
        temp = lookupMaterialColor(WOOD, grassmat);
        al_draw_pixel(drawx, drawy, al_map_rgba_f(temp.r,temp.g, temp.b, (float)grasslevel/100.0f));
    }
}

bool hasWall(Block* b)
{
    if(!b) {
        return false;
    }
    return b->tileShapeBasic==tiletype_shape_basic::Wall;
}

bool hasBuildingOfID(Block* b, int ID)
{
    if(!b) {
        return false;
    }
    return b->building.info.type == ID;
}

bool hasBuildingIdentity(Block* b, uint32_t index, int buildingOcc)
{
    if(!b) {
        return false;
    }
    if (!(b->building.index == index)) {
        return false;
    }
    return b->occ.bits.building == buildingOcc;
}

bool hasBuildingOfIndex(Block* b, uint32_t index)
{
    if(!b) {
        return false;
    }
    return b->building.index == index;
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


void drawFloorBlood ( Block *b, int32_t drawx, int32_t drawy )
{
    t_occupancy occ = b->occ;
    t_SpriteWithOffset sprite;
    int x = b->x, y = b->y, z = b->z;


    if( b->water.index < 1 && (b->bloodlevel)) {
        sprite.fileIndex = INVALID_INDEX;

        // Spatter (should be blood, not blood2) swapped for testing
        if( b->bloodlevel <= config.poolcutoff ) {
            sprite.sheetIndex = 7;
        }

        // Smear (should be blood2, not blood) swapped for testing
        else {
            // if there's no block in the respective direction it's false. if there's no blood in that direction it's false too. should also check to see if there's a ramp below, but since blood doesn't flow, that'd look wrong anyway.
            bool _N = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eUp ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eUp )->bloodlevel > config.poolcutoff) : false ),
                 _S = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eDown ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eDown )->bloodlevel > config.poolcutoff) : false ),
                 _E = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eRight ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eRight )->bloodlevel > config.poolcutoff) : false ),
                 _W = ( b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft ) != NULL ? (b->ownerSegment->getBlockRelativeTo( x, y, z, eLeft )->bloodlevel > config.poolcutoff) : false );

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

        int sheetOffsetX = TILEWIDTH * (sprite.sheetIndex % SHEET_OBJECTSWIDE),
            sheetOffsetY = 0;

        al_draw_tinted_scaled_bitmap( IMGBloodSheet,
                                      premultiply(b->bloodcolor),
                                      sheetOffsetX,
                                      sheetOffsetY,
                                      TILEWIDTH,
                                      TILEHEIGHT+FLOORHEIGHT,
                                      drawx,
                                      drawy,
                                      TILEWIDTH*config.scale,
                                      (TILEHEIGHT+FLOORHEIGHT)*config.scale,
                                      0);
        al_draw_scaled_bitmap(
            IMGBloodSheet,
            sheetOffsetX,
            sheetOffsetY+TILEHEIGHT+FLOORHEIGHT,
            TILEWIDTH,
            TILEHEIGHT+FLOORHEIGHT,
            drawx,
            drawy,
            TILEWIDTH*config.scale,
            (TILEHEIGHT+FLOORHEIGHT)*config.scale,
            0);
    }
}

