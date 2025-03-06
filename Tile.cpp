#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "ContentLoader.h"
#include "SpriteColors.h"
#include "TileTypes.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include "df/building_type.h"
#include "df/plant_growth.h"
#include "df/plant_growth_print.h"
#include "df/tiletype.h"
#include "df/world.h"

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

enum growth_locations
{
    LOCATION_NONE = -1,
    LOCATION_TWIGS = 0,
    LOCATION_LIGHT_BRANCHES,
    LOCATION_HEAVY_BRANCHES,
    LOCATION_TRUNK,
    LOCATION_ROOTS,
    LOCATION_CAP,
    LOCATION_SAPLING,
    LOCATION_SHRUB
};

void initRandomCube()
{
    for(int i = 0; i < RANDOM_CUBE; i++)
        for(int j = 0; j < RANDOM_CUBE; j++)
            for(int k = 0; k < RANDOM_CUBE; k++) {
                stonesenseState.randomCube[i][j][k] = rand();
            }
}

worn_item::worn_item()
{
    matt.index = -1;
    matt.type = -1;
    dyematt.index = -1;
    dyematt.type = -1;
}

Tile::Tile()
{
    valid = visible = false;

    ownerSegment = NULL;
    creature = NULL;
    building.info = NULL;
    building.parent = NULL;
}

// Clear all pointers and empty all vectors
void Tile::Reset()
{
    valid = visible = false;

    ownerSegment = NULL;
    creature = NULL;
    building.info = NULL;
    building.parent = NULL;

    building.sprites.clear();
    building.constructed_mats.clear();

    tileType = df::tiletype::Void;
    x = y = z = -30000;
}

void Tile::Attach(WorldSegment* segment, df::tiletype type, int32_t _x, int32_t _y, int32_t _z)
{
    const DFHack::t_matglossPair NO_MATGLOSS = { INVALID_INDEX, INVALID_INDEX };

    if (valid)
        Reset();

    valid = visible = true;

    ownerSegment = segment;

    x = _x;
    y = _y;
    z = _z;

    tileType = type;

    material = NO_MATGLOSS;
    layerMaterial = NO_MATGLOSS;
    veinMaterial = NO_MATGLOSS;
    hasVein = false;

    depthBorderNorth = depthBorderWest = depthBorderDown = false;

    shadow = 0;

    wallborders = floorborders = rampborders = upstairborders = downstairborders = 0;
    openborders = 255;
    lightborders = 255;

    fog_of_war = false;

    rampindex = 0;

    deepwater = false;

    flow_direction = df::tile_liquid_flow_dir::inactive;

    designation.whole = 0;
    occ.whole = 0;

    tree.type = 0; // this field actually holds df::plant_flags, not a material type
    tree.index = INVALID_INDEX;
    tree_tile.whole = 0;

    mudlevel = snowlevel = bloodlevel = 0;
    bloodcolor = al_map_rgba(0,0,0,0);

    grasslevel = 0;
    grassmat = INVALID_INDEX;

    engraving_character = 0;
    engraving_flags.whole = 0;
    engraving_quality = 0;

    consForm = 0;

    obscuringCreature = obscuringBuilding = false;

    tileeffect.matt = NO_MATGLOSS;
    tileeffect.density = 0;
    tileeffect.type = (df::flow_type) INVALID_INDEX;

    Item.item = NO_MATGLOSS;
    Item.matt = NO_MATGLOSS;
    Item.dyematt = NO_MATGLOSS;

    building.type = df::building_type::NONE;
    building.special = 0;
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
    auto& ssConfig = stonesenseState.ssConfig;

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
    auto& ssConfig = stonesenseState.ssConfig;

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

void Tile::GetDrawLocation(int32_t& drawx, int32_t& drawy)
{
    drawx = x;
    drawy = y;
    int32_t drawz = z;

    ownerSegment->CorrectTileForSegmentOffset( drawx, drawy, drawz);
    ownerSegment->CorrectTileForSegmentRotation( drawx, drawy, drawz);
    pointToScreen((int*)&drawx, (int*)&drawy, drawz);
    drawx -= (TILEWIDTH>>1)*stonesenseState.ssConfig.scale;
}

void Tile::DrawGrowth(c_sprite * spriteobject, bool top=true)
{
    using df::tiletype_material, df::tiletype_shape;
    //Draw Growths that appear over branches
    if (tileMaterial() == tiletype_material::ROOT
        || tileMaterial() == tiletype_material::TREE
        || tileMaterial() == tiletype_material::MUSHROOM)
    {
        // Don't try to render a growth if there's no tree associated with it yet
        if (tree.index == INVALID_INDEX)
            return;
        df::plant_raw* plantRaw = df::global::world->raws.plants.all[tree.index];
        for (size_t i = 0; i < plantRaw->growths.size(); i++)
        {
            df::plant_growth * growth = plantRaw->growths[i];
            if (!growth->locations.whole)
                continue; //we need at least one location.
            int32_t time = *df::global::cur_year_tick;
            if (growth->timing_1 >= 0 && growth->timing_1 > time)
                continue;
            if (growth->timing_2 >= 0 && growth->timing_2 < time)
                continue;
            growth_locations loca = LOCATION_NONE;
            if (growth->locations.bits.cap && tileMaterial() == tiletype_material::MUSHROOM)
                loca = LOCATION_CAP;
            if (growth->locations.bits.heavy_branches && (tileShape() == tiletype_shape::BRANCH && tileType != df::tiletype::TreeBranches))
                loca = LOCATION_HEAVY_BRANCHES;
            if (growth->locations.bits.roots && tileMaterial() == tiletype_material::ROOT)
                loca = LOCATION_ROOTS;
            if (growth->locations.bits.light_branches && tileType == df::tiletype::TreeBranches)
                loca = LOCATION_LIGHT_BRANCHES;
            if (growth->locations.bits.sapling && tileShape() == tiletype_shape::SAPLING)
                loca = LOCATION_SAPLING;
            if (growth->locations.bits.trunk && tileShape() == tiletype_shape::WALL)
                loca = LOCATION_TRUNK;
            if (growth->locations.bits.twigs && tileShape() == tiletype_shape::TWIG)
                loca = LOCATION_TWIGS;

            if (loca == LOCATION_NONE)
                continue;

            DFHack::t_matglossPair fakeMat{
                .type = int16_t(i * 10 + loca),
                .index = tree.index
            };
            auto& contentLoader = stonesenseState.contentLoader;

            if (top)
                spriteobject = contentLoader->growthTopConfigs.get(fakeMat);
            else
                spriteobject = contentLoader->growthBottomConfigs.get(fakeMat);

            if (spriteobject)
            {
                DFHack::t_matglossPair growthMat
                {
                    .type = growth->mat_type,
                    .index = growth->mat_index
                };
                ALLEGRO_COLOR growCol = lookupMaterialColor(growthMat);
                if (growth->prints.size() > 1)
                {
                    df::plant_growth_print * basePrint = growth->prints[0];
                    df::plant_growth_print * currentPrint = basePrint;
                    for (size_t k = 0; k < growth->prints.size(); k++)
                    {
                        if (growth->prints[k]->timing_start >= 0 && growth->prints[k]->timing_start > time)
                            continue;
                        if (growth->prints[k]->timing_end >= 0 && growth->prints[k]->timing_end < time)
                            continue;
                        currentPrint = growth->prints[k];
                    }
                    auto& ssConfig = stonesenseState.ssConfig;
                    growCol = morph_color(growCol,
                        ssConfig.config.colors.getDfColor(basePrint->color[0], basePrint->color[2], ssConfig.config.useDfColors),
                        ssConfig.config.colors.getDfColor(currentPrint->color[0], currentPrint->color[2], ssConfig.config.useDfColors));
                }
                spriteobject->set_growthColor(growCol);
                spriteobject->assemble_world(x, y, z, this);
            }
        }
    }
}

void Tile::AssembleTile( void )
{

    if(!visible) {
        return;
    }

    if((material.type == INORGANIC) && (material.index == -1)) {
        material.index = 0;
    }

    bool defaultSnow = 1;
    t_SpriteWithOffset sprite;
    c_sprite* spriteobject = 0;

    int32_t drawx = 0;
    int32_t drawy = 0;
    GetDrawLocation(drawx, drawy);

    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    //TODO the following check should get incorporated into segment beautification
    if(((drawx + TILEWIDTH*ssConfig.scale) < 0) || (drawx > ssState.ScreenW) || ((drawy + (TILETOPHEIGHT + FLOORHEIGHT)*ssConfig.scale) < 0) || (drawy - WALLHEIGHT*ssConfig.scale > ssState.ScreenH)) {
        visible = false;
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
        AssembleSpriteFromSheet( SPRITEOBJECT_BLACK, stonesenseState.IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy+FLOORHEIGHT*ssConfig.scale);
        AssembleSpriteFromSheet( SPRITEOBJECT_BLACK, stonesenseState.IMGObjectSheet, al_map_rgb(255,255,255), drawx, drawy);
        return;
    }

    int rando = stonesenseState.randomCube[x%RANDOM_CUBE][y%RANDOM_CUBE][z%RANDOM_CUBE];

    DrawGrowth(spriteobject, false);

    //Draw Ramp Tops
    if(tileType == df::tiletype::RampTop) {
        Tile * b = this->ownerSegment->getTile(this->x, this->y, this->z - 1);
        if ( b && b->building.type != BUILDINGTYPE_BLACKBOX && b->tileShapeBasic() == df::tiletype_shape_basic::Ramp ) {
            spriteobject = GetTileSpriteMap(b->tileType, b->material, b->consForm);
            if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
                spriteobject->set_sheetindex(0);
                spriteobject->set_fileindex(INVALID_INDEX);
                spriteobject->set_defaultsheet(stonesenseState.IMGRampSheet);
            }
            if (spriteobject->get_sheetindex() != INVALID_INDEX) {
                spriteobject->set_size(SPRITEWIDTH, TILETOPHEIGHT);
                spriteobject->set_plate_layout(RAMPTOPPLATE);
                spriteobject->set_offset(0, WALLHEIGHT);
                spriteobject->assemble_world_offset(x, y, z, 0, b);
                spriteobject->set_offset(0, 0);
            }
            spriteobject->set_plate_layout(TILEPLATE);
        } else {
            return;
        }
    }

    using df::tiletype_shape, df::tiletype_shape_basic;

    //Draw Floor
    if( tileShapeBasic()==tiletype_shape_basic::Floor ||
            tileShapeBasic()==tiletype_shape_basic::Wall ||
            tileShapeBasic()==tiletype_shape_basic::Ramp ||
            tileShapeBasic()==tiletype_shape_basic::Stair ||
            tileShape()==tiletype_shape::TWIG) {

        //If plate has no floor, look for a Filler Floor from it's wall
        if (tileShapeBasic() == tiletype_shape_basic::Floor || tileShape() == tiletype_shape::TWIG) {
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
        AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngFloorSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
    }

    //Draw Ramp
    if(tileShapeBasic()==tiletype_shape_basic::Ramp) {
        spriteobject = GetTileSpriteMap(tileType, material, consForm);
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            spriteobject->set_sheetindex(0);
            spriteobject->set_fileindex(INVALID_INDEX);
            spriteobject->set_defaultsheet(stonesenseState.IMGRampSheet);
        }
        if (spriteobject->get_sheetindex() != INVALID_INDEX) {
            spriteobject->set_size(SPRITEWIDTH, SPRITEHEIGHT);
            spriteobject->set_plate_layout(RAMPBOTTOMPLATE);
            spriteobject->assemble_world_offset(x, y, z, 0, this, (chopThisTile && int(this->z) == ownerSegment->segState.Position.z + ownerSegment->segState.Size.z -2));
        }
        spriteobject->set_plate_layout(TILEPLATE);
    }

    if(snowlevel <= bloodlevel) {
        AssembleFloorBlood ( drawx, drawy );
    }

    //first part of snow
    if(tileShapeBasic()!=tiletype_shape_basic::Ramp
        && tileShapeBasic()!=tiletype_shape_basic::Wall
        && tileShapeBasic()!=tiletype_shape_basic::Stair
        && defaultSnow
        && snowlevel>bloodlevel) {
        if(snowlevel > 75) {
            AssembleSpriteFromSheet( 20, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this);
        } else if(snowlevel > 50) {
            AssembleSpriteFromSheet( 21, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        } else if(snowlevel > 25) {
            AssembleSpriteFromSheet( 22, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        } else if(snowlevel > 0) {
            AssembleSpriteFromSheet( 23, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        }
    }

    ////vegetation
    //if(tree.index > 0 || tree.type > 0){
    //    c_sprite * vegetationsprite = 0;
    //    vegetationsprite = GetSpriteVegetation( (TileClass) getVegetationType( this->floorType ), tree.index );
    //    if(vegetationsprite)
    //        vegetationsprite->assemble_world(x, y, z);
    //}


    //shadow
    if (shadow > 0) {
        AssembleSpriteFromSheet( BASE_SHADOW_PLATE + shadow - 1, stonesenseState.IMGObjectSheet, al_map_rgb(255,255,255), drawx, (tileShapeBasic()==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*ssConfig.scale)):drawy , this);
    }

    //Building
    bool skipBuilding =
        (building.type == df::building_type::Civzone && !ssConfig.config.show_zones) ||
        (building.type == df::building_type::Stockpile && !ssConfig.config.show_stockpiles);

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

    //items
    if(Item.item.type >= 0) {
        if(
            auto& contentLoader = stonesenseState.contentLoader;
            contentLoader->itemConfigs[Item.item.type] &&
            (size_t(Item.item.index) < contentLoader->itemConfigs[Item.item.type]->subItems.size()) &&
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]) {
            contentLoader->itemConfigs[Item.item.type]->subItems[Item.item.index]->sprite.assemble_world(x, y, z, this);
        } else if (
            contentLoader->itemConfigs[Item.item.type] &&
            contentLoader->itemConfigs[Item.item.type]->configured) {
            contentLoader->itemConfigs[Item.item.type]->default_sprite.assemble_world(x, y, z, this);
        } else {
            AssembleSpriteFromSheet( 350, stonesenseState.IMGObjectSheet, lookupMaterialColor(Item.matt, Item.dyematt), drawx, (tileShapeBasic()==tiletype_shape_basic::Ramp)?(drawy - ((WALLHEIGHT/2)*ssConfig.scale)):drawy , this);
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

    if(tileShapeBasic()==tiletype_shape_basic::Floor ||
            tileShapeBasic()==tiletype_shape_basic::Wall) {
        //draw wall
        spriteobject =  GetTileSpriteMap(tileType, material, consForm);
        if (spriteobject->get_sheetindex() == UNCONFIGURED_INDEX) {
            if(tileShapeBasic()==tiletype_shape_basic::Wall){
                spriteobject->set_sheetindex(SPRITEOBJECT_WALL_NA);
                spriteobject->set_fileindex(INVALID_INDEX);
                spriteobject->set_plate_layout(TILEPLATE);
                spriteobject->set_defaultsheet(stonesenseState.IMGObjectSheet);
            } else {
                //unconfigured non-walls are not valid
                spriteobject->set_sheetindex(INVALID_INDEX);
            }
        }
        if (spriteobject->get_sheetindex() == INVALID_INDEX ) {
            //skip
        } else {
            spriteobject->assemble_world(x, y, z, this, (chopThisTile && int(this->z) == ownerSegment->segState.Position.z + ownerSegment->segState.Size.z -2));
        }
    }

    //Wall Engravings
    if((tileShapeBasic()==tiletype_shape_basic::Wall) && engraving_character) {
        if(ownerSegment->segState.Rotation == 0) {
            if(engraving_flags.bits.east) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.south) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->segState.Rotation == 1) {
            if(engraving_flags.bits.north) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.east) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->segState.Rotation == 2) {
            if(engraving_flags.bits.west) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.north) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
        if(ownerSegment->segState.Rotation == 3) {
            if(engraving_flags.bits.south) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngRightSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
            if(engraving_flags.bits.west) {
                AssembleSpriteFromSheet( engraving_character, stonesenseState.IMGEngLeftSheet, al_map_rgba_f(1.0,1.0,1.0,((engraving_quality + 5.0f) / 10.0f)), drawx, drawy, this );
            }
        }
    }

    DrawGrowth(spriteobject, true);
    if(designation.bits.flow_size > 0) {
        auto& contentLoader = stonesenseState.contentLoader;
        //if(waterlevel == 7) waterlevel--;
        uint32_t waterlevel = designation.bits.flow_size + (deepwater ? 1 : 0);
        if(designation.bits.liquid_type == 0) {
            contentLoader->water[waterlevel-1].sprite.assemble_world(x, y, z, this, (chopThisTile && int(this->z) == ownerSegment->segState.Position.z + ownerSegment->segState.Size.z -2));
        } else {
            contentLoader->lava[waterlevel-1].sprite.assemble_world(x, y, z, this, (chopThisTile && int(this->z) == ownerSegment->segState.Position.z + ownerSegment->segState.Size.z -2));
        }
    }

    // creature
    if(occ.bits.unit && creature && (ssConfig.show_hidden_tiles || !designation.bits.hidden)) {
        AssembleCreature(drawx, drawy, creature, this);
    }

    //second part of snow
    if(tileShapeBasic()!=tiletype_shape_basic::Wall
        && tileShapeBasic()!=tiletype_shape_basic::Stair
        && defaultSnow
        && snowlevel>bloodlevel) {
        if(snowlevel > 75) {
            AssembleSpriteFromSheet( 24, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        } else if(snowlevel > 50) {
            AssembleSpriteFromSheet( 25, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        } else if(snowlevel > 25) {
            AssembleSpriteFromSheet( 26, stonesenseState.IMGObjectSheet, bloodcolor, drawx, drawy, this );
        }
    }

    if(tileeffect.density>0) {
        ALLEGRO_COLOR tint = lookupMaterialColor(tileeffect.matt);
        int size = 0;
        switch(tileeffect.type){
        case df::flow_type::Miasma:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_miasma, tint);
            break;
        case df::flow_type::Steam:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
            break;
        case df::flow_type::Mist:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water2, tint);
            break;
        case df::flow_type::MaterialDust:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dust, tint);
            break;
        case df::flow_type::MagmaMist:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_magma, tint);
            break;
        case df::flow_type::Smoke:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_smoke, tint);
            break;
        case df::flow_type::Dragonfire:
            tint *= tileeffect.density / 100.f;
            size = 3 - ((tileeffect.density-1)/25);
            AssembleSpriteFromSheet((((stonesenseState.currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
            //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Dragonfire.matt.type, Eff_Dragonfire.matt.index);
            //draw_particle_cloud(Eff_Dragonfire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_dragonfire, tint);
            break;
        case df::flow_type::Fire:
            tint *= tileeffect.density / 100.0f;
            size = 3 - ((tileeffect.density-1)/25);
            AssembleSpriteFromSheet((((stonesenseState.currentFrameLong+rando)%8)*20+size), sprite_dragonfire, tint, drawx, drawy, this, 2.0f);
            //ALLEGRO_COLOR tint = lookupMaterialColor(Eff_Fire.matt.type, Eff_Fire.matt.index);
            //draw_particle_cloud(Eff_Fire.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_fire, tint);
            break;
        case df::flow_type::Web:
            tint.a*=tileeffect.density/100.0f;
            AssembleSpriteFromSheet(rando%5, sprite_webing, tint, drawx, drawy, this, 4.0f);
            //al_draw_tinted_bitmap(sprite_webing,tint, drawx, drawy - (WALLHEIGHT), 0);
            break;
        case df::flow_type::MaterialGas:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
            break;
        case df::flow_type::MaterialVapor:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_boiling, tint);
            break;
        case df::flow_type::SeaFoam:
            tint.a*=tileeffect.density/100.0f;
            AssembleSprite(sprite_oceanwave, tint, 0, 0, al_get_bitmap_width(sprite_oceanwave), al_get_bitmap_height(sprite_oceanwave),
                drawx, drawy - (WALLHEIGHT)*ssConfig.scale, SPRITEWIDTH*ssConfig.scale, SPRITEHEIGHT*ssConfig.scale, 0);
            break;
        case df::flow_type::OceanWave:
            AssembleParticleCloud(tileeffect.density, drawx, drawy - (SPRITEHEIGHT/2), SPRITEWIDTH, SPRITEHEIGHT, sprite_water, tint);
            break;
        case df::flow_type::ItemCloud:
            // TODO
            break;

        }
    }

    // Creature Names / Info
    if(occ.bits.unit && creature && (ssConfig.show_hidden_tiles || !designation.bits.hidden)) {
        AssembleCreatureText(drawx, drawy, creature, ownerSegment);
    }
}

bool hasWall(Tile* b)
{
    if(!b) {
        return false;
    }
    return b->tileShapeBasic()==df::tiletype_shape_basic::Wall;
}

bool hasBuildingOfID(Tile* b, int ID)
{
    if(!b) {
        return false;
    }
    return b->building.type == ID;
}

bool hasBuildingIdentity(Tile* b, Stonesense_Building* index, df::tile_building_occ buildingOcc)
{
    if(!b) {
        return false;
    }
    if (!(b->building.info == index)) {
        return false;
    }
    return b->occ.bits.building == buildingOcc;
}

bool hasBuildingOfIndex(Tile* b, Stonesense_Building* index)
{
    if(!b) {
        return false;
    }
    return b->building.info == index;
}

bool wallShouldNotHaveBorders( df::tiletype in )
{
    switch( in ) {
    case df::tiletype::StoneFortification:
    case df::tiletype::MineralFortification:
    case df::tiletype::LavaFortification:
    case df::tiletype::FeatureFortification:
    case df::tiletype::ConstructedFortification:
        return true;
    default:
        return false;
    };
}

bool containsDesignations( df::tile_designation des, df::tile_occupancy occ )
{
    if (!df::global::gamemode || *df::global::gamemode == df::game_mode::ADVENTURE)
        return false;
    if(des.bits.dig != df::tile_dig_designation::No) {
        return true;
    } else if(des.bits.smooth != 0) {
        return true;
    } else if(occ.bits.carve_track_east
        || occ.bits.carve_track_north
        || occ.bits.carve_track_south
        || occ.bits.carve_track_west) {
            return true;
    }
    return false;
}

void createEffectSprites()
{
    sprite_miasma        = CreateSpriteFromSheet( 180, stonesenseState.IMGObjectSheet);
    sprite_water        = CreateSpriteFromSheet( 181, stonesenseState.IMGObjectSheet);
    sprite_water2        = CreateSpriteFromSheet( 182, stonesenseState.IMGObjectSheet);
    sprite_blood        = CreateSpriteFromSheet( 183, stonesenseState.IMGObjectSheet);
    sprite_dust            = CreateSpriteFromSheet( 182, stonesenseState.IMGObjectSheet);
    sprite_magma        = CreateSpriteFromSheet( 185, stonesenseState.IMGObjectSheet);
    sprite_smoke        = CreateSpriteFromSheet( 186, stonesenseState.IMGObjectSheet);
    sprite_dragonfire    = load_bitmap_withWarning("stonesense/Effect_flames.png");
    sprite_fire            = CreateSpriteFromSheet( 188, stonesenseState.IMGObjectSheet);
    sprite_webing        = load_bitmap_withWarning("stonesense/Effect_web.png");
    sprite_boiling        = CreateSpriteFromSheet( 190, stonesenseState.IMGObjectSheet);
    sprite_oceanwave    = CreateSpriteFromSheet( 191, stonesenseState.IMGObjectSheet);
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

bool isNeighborBloody(Tile* b, dirRelative dir) {
    // Checks if the neighbor tile exists, and has blood pooled.
    Tile* neighbor = b->ownerSegment->getTileRelativeTo( b->x, b->y, b->z, dir );
    if (neighbor != nullptr && neighbor->bloodlevel >= stonesenseState.ssConfig.poolcutoff)
        return true;
    return false;
}

int Tile::GetBloodSpriteOffset() {
    if( this->designation.bits.flow_size < 1 && (this->bloodlevel)) {
        if( this->bloodlevel < stonesenseState.ssConfig.poolcutoff ) {
            return 16;
        }
        else {
            int _N = isNeighborBloody(this, eUp),
                _S = isNeighborBloody(this, eDown),
                _E = isNeighborBloody(this, eRight),
                _W = isNeighborBloody(this, eLeft);
            // Spritesheet is aranged as to make this the proper offset
            return (_N << 3) | (_E << 2) | (_S << 1) | _W;
        }
    }
    return 0;
}

void Tile::AssembleFloorBlood ( int32_t drawx, int32_t drawy )
{
    auto& ssConfig = stonesenseState.ssConfig;
    t_SpriteWithOffset sprite;

    if( designation.bits.flow_size < 1 && (bloodlevel)) {
        sprite.fileIndex = INVALID_INDEX;

        sprite.sheetIndex = this->GetBloodSpriteOffset();

        int sheetOffsetX = TILEWIDTH * (sprite.sheetIndex % SHEET_OBJECTSWIDE),
            sheetOffsetY = 0;

        AssembleSprite(
            stonesenseState.IMGBloodSheet,
            premultiply(bloodcolor),
            sheetOffsetX,
            sheetOffsetY,
            TILEWIDTH,
            TILETOPHEIGHT+FLOORHEIGHT,
            drawx,
            drawy,
            TILEWIDTH*ssConfig.scale,
            (TILETOPHEIGHT+FLOORHEIGHT)*ssConfig.scale,
            0);
        AssembleSprite(
            stonesenseState.IMGBloodSheet,
            al_map_rgb(255,255,255),
            sheetOffsetX,
            sheetOffsetY+TILETOPHEIGHT+FLOORHEIGHT,
            TILEWIDTH,
            TILETOPHEIGHT+FLOORHEIGHT,
            drawx,
            drawy,
            TILEWIDTH*ssConfig.scale,
            (TILETOPHEIGHT+FLOORHEIGHT)*ssConfig.scale,
            0);
    }
}

df::tiletype_shape Tile::tileShape()
{
    return ENUM_ATTR(tiletype, shape, tileType);
}

df::tiletype_special Tile::tileSpecial()
{
    return ENUM_ATTR(tiletype, special, tileType);
}

df::tiletype_material Tile::tileMaterial()
{
    return ENUM_ATTR(tiletype, material, tileType);
}
