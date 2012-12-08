#include <assert.h>
#include <vector>

using namespace std;

#pragma once

#include "common.h"
#include "Tile.h"
#include "GUI.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "MapLoading.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include "Tile.h"

#include "df/ui.h"
#include "df/building_actual.h"

#include "df/itemdef.h"
#include "df/itemdef_weaponst.h"
#include "df/itemdef_trapcompst.h"
#include "df/itemdef_toyst.h"
#include "df/itemdef_toolst.h"
#include "df/itemdef_instrumentst.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_ammost.h"
#include "df/itemdef_siegeammost.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_pantsst.h"
#include "df/itemdef_foodst.h"

#define color_segmentoutline al_map_rgb(0,0,0)

extern ALLEGRO_FONT *font;

int MiniMapTopLeftX = 0;
int MiniMapTopLeftY = 0;
int MiniMapBottomRightX = 0;
int MiniMapBottomRightY = 0;
int MiniMapSegmentWidth =0;
int MiniMapSegmentHeight =0;
double oneTileInPixels = 0;

ALLEGRO_BITMAP* IMGObjectSheet;
ALLEGRO_BITMAP* IMGCreatureSheet;
ALLEGRO_BITMAP* IMGRampSheet;
ALLEGRO_BITMAP* IMGStatusSheet;
ALLEGRO_BITMAP* IMGProfSheet;
ALLEGRO_BITMAP* IMGJobSheet;
ALLEGRO_BITMAP* IMGBloodSheet;
ALLEGRO_BITMAP* IMGEngFloorSheet;
ALLEGRO_BITMAP* IMGEngLeftSheet;
ALLEGRO_BITMAP* IMGEngRightSheet;
ALLEGRO_BITMAP* IMGLetterSheet;

ALLEGRO_BITMAP* buffer = 0;
ALLEGRO_BITMAP* bigFile = 0;
vector<ALLEGRO_BITMAP*> IMGCache;
vector<ALLEGRO_BITMAP*> IMGFilelist;
vector<string*> IMGFilenames;
GLhandleARB tinter;
GLhandleARB tinter_shader;
Crd3D debugCursor;

const char * get_item_subtype(item_type::item_type type, int subtype)
{
    if (subtype < 0) {
        return "?";
    }
    switch(type) {
    case item_type::WEAPON:
        return df::global::world->raws.itemdefs.weapons[subtype]->id.c_str();
    case item_type::TRAPCOMP:
        return df::global::world->raws.itemdefs.trapcomps[subtype]->id.c_str();
    case item_type::TOY:
        return df::global::world->raws.itemdefs.toys[subtype]->id.c_str();
    case item_type::TOOL:
        return df::global::world->raws.itemdefs.tools[subtype]->id.c_str();
    case item_type::INSTRUMENT:
        return df::global::world->raws.itemdefs.instruments[subtype]->id.c_str();
    case item_type::ARMOR:
        return df::global::world->raws.itemdefs.armor[subtype]->id.c_str();
    case item_type::AMMO:
        return df::global::world->raws.itemdefs.ammo[subtype]->id.c_str();
    case item_type::SIEGEAMMO:
        return df::global::world->raws.itemdefs.siege_ammo[subtype]->id.c_str();
    case item_type::GLOVES:
        return df::global::world->raws.itemdefs.gloves[subtype]->id.c_str();
    case item_type::SHOES:
        return df::global::world->raws.itemdefs.shoes[subtype]->id.c_str();
    case item_type::SHIELD:
        return df::global::world->raws.itemdefs.shields[subtype]->id.c_str();
    case item_type::HELM:
        return df::global::world->raws.itemdefs.helms[subtype]->id.c_str();
    case item_type::PANTS:
        return df::global::world->raws.itemdefs.pants[subtype]->id.c_str();
    case item_type::FOOD:
        return df::global::world->raws.itemdefs.food[subtype]->id.c_str();
    default:
        return "?";
    }
}

void draw_diamond(float x, float y, ALLEGRO_COLOR color)
{
    al_draw_filled_triangle(x, y, x+4, y+4, x-4, y+4, color);
    al_draw_filled_triangle(x+4, y+4, x, y+8, x-4, y+4, color);
}

void draw_borders(float x, float y, uint8_t borders)
{
    if(borders & 1) {
        draw_diamond(x, y, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x, y, al_map_rgb(0,0,0));
    }

    if(borders & 2) {
        draw_diamond(x+4, y+4, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x+4, y+4, al_map_rgb(0,0,0));
    }

    if(borders & 4) {
        draw_diamond(x+8, y+8, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x+8, y+8, al_map_rgb(0,0,0));
    }

    if(borders & 8) {
        draw_diamond(x+4, y+12, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x+4, y+12, al_map_rgb(0,0,0));
    }

    if(borders & 16) {
        draw_diamond(x, y+16, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x, y+16, al_map_rgb(0,0,0));
    }

    if(borders & 32) {
        draw_diamond(x-4, y+12, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x-4, y+12, al_map_rgb(0,0,0));
    }

    if(borders & 64) {
        draw_diamond(x-8, y+8, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x-8, y+8, al_map_rgb(0,0,0));
    }

    if(borders & 128) {
        draw_diamond(x-4, y+4, al_map_rgb(255,255,255));
    } else {
        draw_diamond(x-4, y+4, al_map_rgb(0,0,0));
    }

}

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1)
{
    //assume z of 0
    x-=ssState.ScreenW / 2;
    y-=ssState.ScreenH / 2;
    x/=ssConfig.scale;
    y/=ssConfig.scale;
    x+=ssState.ScreenW / 2;
    y+=ssState.ScreenH / 2;
    x-=PLATEWIDTH/2;
    y+=PLATEWIDTH/2;
    z1 = -3;
    y+= z1*TILEHEIGHT/2;
    //y-=TILEHEIGHT;
    x+=PLATEWIDTH>>1;
    int offx = ssState.ScreenW /2;
    int offy = (-20)-(TILEHEIGHT * ssConfig.lift_segment_offscreen);
    y-=offy;
    x-=offx;
    y1=y*2-x;
    x1=x*2+y1;
    x1/=PLATEWIDTH;
    y1/=PLATEWIDTH;

}

int get_textf_width(const ALLEGRO_FONT *font, const char *format, ...)
{
    ALLEGRO_USTR *buf;
    va_list ap;
    const char *s;

    int width;
    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(ap, format);
        s = va_arg(ap, const char *);
        width = al_get_text_width(font, s);
        va_end(ap);
        return width;
    }

    va_start(ap, format);
    buf = al_ustr_new("");
    al_ustr_vappendf(buf, format, ap);
    va_end(ap);

    width = al_get_text_width(font, al_cstr(buf));

    al_ustr_free(buf);
    return width;
}

void draw_text_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *ustr)
{
    int xx, yy, ww, hh;
    al_get_text_dimensions(font, ustr, &xx, &yy, &ww, &hh);
    if(flags & ALLEGRO_ALIGN_CENTRE) {
        xx -= ww/2;
    } else if(flags & ALLEGRO_ALIGN_RIGHT) {
        xx -= ww;
    }
    al_draw_filled_rectangle(x+xx, y+yy, x+xx+ww, y+yy+hh, al_map_rgba_f(0.0,0.0,0.0,0.75));
    al_draw_text(font, color, x, y, flags, ustr);
}

void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...)
{
    ALLEGRO_USTR *buf;
    va_list arglist;
    const char *s;

    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(arglist, format);
        s = va_arg(arglist, const char *);
        draw_text_border(font, color, x, y, flags, s);
        va_end(arglist);
        return;
    }


    va_start(arglist, format);
    buf = al_ustr_new("");
    al_ustr_vappendf(buf, format, arglist);
    va_end(arglist);
    draw_ustr_border(font, color, x, y, flags, buf);
    al_ustr_free(buf);
}

void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr)
{
    int xx, yy, ww, hh;
    al_get_ustr_dimensions(font, ustr, &xx, &yy, &ww, &hh);
    if(flags & ALLEGRO_ALIGN_CENTRE) {
        xx -= ww/2;
    } else if(flags & ALLEGRO_ALIGN_RIGHT) {
        xx -= ww;
    }
    al_draw_filled_rectangle(x+xx, y+yy, x+xx+ww, y+yy+hh, al_map_rgba_f(0.0,0.0,0.0,0.75));
    al_draw_ustr(font, color, x, y, flags, ustr);
}

void pointToScreen(int *inx, int *iny, int inz)
{
    int z = inz-1;

    int x = *inx-*iny;
    x = x * PLATEWIDTH / 2;
    x *= ssConfig.scale;
    x+=ssState.ScreenW / 2;

    int y = *inx+*iny;
    y = y*PLATEHEIGHT / 2;
    y -= z*TILEHEIGHT;
    y -= PLATEHEIGHT*5/4;
    y -= TILEHEIGHT*ssConfig.lift_segment_offscreen;
    y *= ssConfig.scale;

    *inx=x;
    *iny=y;
}

/**
 * Corrects the coordinate (x,y) for rotation in a region of size (szx, szy).
 */
void correctForRotation(int32_t& x, int32_t& y, unsigned char rot, int32_t szx, int32_t szy){
    int32_t oldx = x;
    int32_t oldy = y;

    if(rot == 1) {
        x = szy - oldy -1;
        y = oldx;
    }
    if(rot == 2) {
        x = szx - oldx -1;
        y = szy - oldy -1;
    }
    if(rot == 3) {
        x = oldy;
        y = szx - oldx -1;
    }
}

Crd2D WorldTileToScreen(int32_t x, int32_t y, int32_t z)
{
    correctTileForDisplayedOffset( x, y, z);
    return LocalTileToScreen(x, y, z-1);
}

Crd2D LocalTileToScreen(int32_t x, int32_t y, int32_t z)
{
    pointToScreen((int*)&x, (int*)&y, z);
    Crd2D result;
    result.x = x;
    result.y = y;
    return result;
}

void DrawCurrentLevelOutline(bool backPart)
{
    int x = ssState.DisplayedSegmentX+1;
    int y = ssState.DisplayedSegmentY+1;
    int z = ssState.DisplayedSegmentZ;
    int sizex = ssConfig.segmentSize.x-2;
    int sizey = ssConfig.segmentSize.y-2;

    if(ssConfig.hide_outer_tiles) {
        x++;
        y++;
        sizex -= 2;
        sizey -= 2;
    }

    Crd2D p1 = WorldTileToScreen(x, y, z);
    Crd2D p2 = WorldTileToScreen(x, y + sizey , z);
    Crd2D p3 = WorldTileToScreen(x + sizex , y, z);
    Crd2D p4 = WorldTileToScreen(x + sizex , y + sizey , z);
    p1.y += FLOORHEIGHT*ssConfig.scale;
    p2.y += FLOORHEIGHT*ssConfig.scale;
    p3.y += FLOORHEIGHT*ssConfig.scale;
    p4.y += FLOORHEIGHT*ssConfig.scale;
    if(backPart) {
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, color_segmentoutline, 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
        al_draw_line(p2.x, p2.y, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);

        al_draw_line(p1.x, p1.y, p3.x, p3.y, color_segmentoutline, 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
        al_draw_line(p3.x, p3.y, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
    } else {
        al_draw_line(p4.x, p4.y, p4.x, p4.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
        al_draw_line(p4.x, p4.y, p2.x, p2.y, color_segmentoutline ,0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline ,0);

        al_draw_line(p4.x, p4.y, p3.x, p3.y, color_segmentoutline, 0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, color_segmentoutline, 0);
    }
}

void drawDebugCursorAndInfo(WorldSegment * segment)
{
    using df::global::ui;

    if((ssConfig.dfCursorX != -30000) && ssConfig.follow_DFcursor) {
        int x = ssConfig.dfCursorX;
        int y = ssConfig.dfCursorY;
        int z = ssConfig.dfCursorZ;
        correctTileForDisplayedOffset(x,y,z);
        segment->CorrectTileForSegmentRotation( x, y, z );
        debugCursor.x = x;
        debugCursor.y = y;
        debugCursor.z = z;
    } else {
        debugCursor.z = 0;
    }
    Crd2D point = LocalTileToScreen(debugCursor.x, debugCursor.y, debugCursor.z);
    int sheetx = SPRITEOBJECT_CURSOR % SHEET_OBJECTSWIDE;
    int sheety = SPRITEOBJECT_CURSOR / SHEET_OBJECTSWIDE;
    al_draw_tinted_scaled_bitmap(
        IMGObjectSheet,
        al_map_rgb(255,255,255),
        sheetx * SPRITEWIDTH,
        sheety * SPRITEHEIGHT,
        SPRITEWIDTH,
        SPRITEHEIGHT,
        point.x-((SPRITEWIDTH/2)*ssConfig.scale),
        point.y - (WALLHEIGHT)*ssConfig.scale,
        SPRITEWIDTH*ssConfig.scale,
        SPRITEHEIGHT*ssConfig.scale,
        0);

    //get tile info
    Tile* b = segment->getTileLocal( debugCursor.x, debugCursor.y, debugCursor.z+segment->size.z-2);
    int i = 10;
    draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0, "Tile 0x%x", b);

    draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                      "Coord:(%i,%i,%i)", debugCursor.x, debugCursor.y, debugCursor.z);

    if(!b) {
        return;
    }
    int ttype;
    const char* tform = NULL;
    if (b->tileShapeBasic()==tiletype_shape_basic::Floor) {
        ttype=b->tileType;
        tform="floor";
    } else if (b->tileShapeBasic()==tiletype_shape_basic::Wall) {
        ttype=b->tileType;
        tform="wall";
    } else if (b->tileShapeBasic()==tiletype_shape_basic::Ramp || b->tileType==tiletype::RampTop) {
        ttype=b->tileType;
        tform="ramp";
    } else if (b->tileShapeBasic()==tiletype_shape_basic::Stair) {
        ttype=b->tileType;
        tform="stair";
    }

    switch(ui->main.mode) {
    case ui_sidebar_mode::BuildingItems:
        if(b->building.info && b->building.type != BUILDINGTYPE_NA && b->building.type != BUILDINGTYPE_BLACKBOX && b->building.type != BUILDINGTYPE_TREE) {
            auto Actual_building = virtual_cast<df::building_actual>(b->building.info->origin);
            if(!Actual_building) {
                break;
            }
            CoreSuspender csusp;
            Actual_building->contained_items.size(); //Item array.
            std::string BuildingName;
            Actual_building->getName(&BuildingName);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "%s",
                              BuildingName.c_str());
            for(int index = 0; index < Actual_building->contained_items.size(); index++) {
                MaterialInfo mat;
                mat.decode(Actual_building->contained_items[index]->item->getMaterial(), Actual_building->contained_items[index]->item->getMaterialIndex());
                char stacknum[8] = {0};
                sprintf(stacknum, " [%d]", Actual_building->contained_items[index]->item->getStackSize());
                draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                                  "%s - %s%s%s%s%s",
                                  mat.getToken().c_str(),
                                  ENUM_KEY_STR(item_type, Actual_building->contained_items[index]->item->getType()).c_str(),
                                  (Actual_building->contained_items[index]->item->getSubtype()>=0)?"/":"",
                                  (Actual_building->contained_items[index]->item->getSubtype()>=0)?get_item_subtype(Actual_building->contained_items[index]->item->getType(),Actual_building->contained_items[index]->item->getSubtype()):"",
                                  Actual_building->contained_items[index]->item->getStackSize()>1?stacknum:"",
                                  (Actual_building->contained_items[index]->use_mode == 2?" [B]":""));
            }
        }
        break;
    case ui_sidebar_mode::ViewUnits:
        //creatures
        if(!b->occ.bits.unit || !b->creature) {
            break;
        }
        draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                          "Creature:%s(%i) Caste:%s(%i) Job:%s",
                          contentLoader->Mats->race.at(b->creature->race).id.c_str(), b->creature->race,
                          contentLoader->Mats->raceEx.at(b->creature->race).castes.at(b->creature->caste).id.c_str(), b->creature->caste,
                          contentLoader->professionStrings.at(b->creature->profession).c_str());

        //Inventories!
        if(b->inv) {
            for(int item_type_idex = 0; item_type_idex < b->inv->item.size(); item_type_idex++) {
                if(b->inv->item[item_type_idex].empty()) {
                    continue;
                }
                draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                                  "%s:", ENUM_KEY_STR(item_type, (item_type::item_type)item_type_idex).c_str());
                for(int ind = 0; ind < b->inv->item[item_type_idex].size(); ind++) {
                    if(b->inv->item[item_type_idex][ind].empty()) {
                        continue;
                    }
                    if(b->inv->item[item_type_idex][ind][0].matt.type < 0) {
                        continue;
                    }
                    MaterialInfo mat;
                    mat.decode(b->inv->item[item_type_idex][ind][0].matt.type,b->inv->item[item_type_idex][ind][0].matt.index);
                    draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                                      "    %s - %s",
                                      mat.getToken().c_str(),
                                      get_item_subtype((item_type::item_type)item_type_idex,ind));
                }
            }
        }
        //just so it has it's own scope
        {
            char strCreature[150] = {0};
            generateCreatureDebugString( b->creature, strCreature );
            //memset(strCreature, -1, 50);
            /*
            // FIXME:: getJob is no more.
            try{
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
            "flag1: %s Sex: %d  Mood: %d Job: %s", strCreature, b->creature->sex + 1, b->creature->mood, (b->creature->current_job.active?contentLoader->MemInfo->getJob(b->creature->current_job.jobType).c_str():""));
            }
            catch(exception &e)
            {
            WriteErr("DFhack exeption: %s\n", e.what());
            }
            */

            int yy = (i++*al_get_font_line_height(font));
            int xx = 2;
            for(unsigned int j = 0; j<b->creature->nbcolors ; j++) {
                if(contentLoader->Mats->raceEx.at(b->creature->race).castes.at(b->creature->caste).ColorModifier.at(j).colorlist.size() > b->creature->color[j]) {
                    uint32_t cr_color = contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].colorlist[b->creature->color[j]];
                    if(cr_color < contentLoader->Mats->color.size()) {
                        draw_textf_border(font,
                                          al_map_rgb_f(
                                              contentLoader->Mats->color[cr_color].red,
                                              contentLoader->Mats->color[cr_color].green,
                                              contentLoader->Mats->color[cr_color].blue), xx, yy, 0,
                                          "%s ", contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].part.c_str());
                        xx += get_textf_width(font, "%s ", contentLoader->Mats->raceEx[b->creature->race].castes[b->creature->caste].ColorModifier[j].part.c_str());
                    }
                }
            }
        }
        break;
    default:
        draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                          "Game Mode:%i, Control Mode:%i", contentLoader->gameMode.g_mode, contentLoader->gameMode.g_type);
        if (tform != NULL && b->material.type != INVALID_INDEX) {
            const char* formName = lookupFormName(b->consForm);
            const char* matName = lookupMaterialTypeName(b->material.type);
            const char* subMatName = lookupMaterialName(b->material.type,b->material.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "%s %s:%i Material:%s%s%s (%d,%d)", formName, tform, ttype,
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"", b->material.type,b->material.index);
        }
        if (tform != NULL && b->material.type != INVALID_INDEX) {
            MaterialInfo mat;
            mat.decode(b->material.type, b->material.index);
            ALLEGRO_COLOR color = al_map_rgb_f(contentLoader->Mats->color[mat.material->state_color[0]].red, contentLoader->Mats->color[mat.material->state_color[0]].green, contentLoader->Mats->color[mat.material->state_color[0]].blue);
            draw_textf_border(font, color, 2, (i++*al_get_font_line_height(font)), 0,
                              "%s", mat.material->state_name[0].c_str());
        }	//if (tform != NULL)
        //{
        //	draw_textf_border(font, 2, (i++*al_get_font_line_height(font)), 0,
        //		"MaterialType: %d, MaterialIndex: %d", b->material.type, b->material.index);
        //}
        if (tform != NULL) {
            const char* matName = lookupMaterialTypeName(b->layerMaterial.type);
            const char* subMatName = lookupMaterialName(b->layerMaterial.type,b->layerMaterial.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Layer Material:%s%s%s",
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if ((tform != NULL) && b->hasVein == 1) {
            const char* matName = lookupMaterialTypeName(b->veinMaterial.type);
            const char* subMatName = lookupMaterialName(b->veinMaterial.type,b->veinMaterial.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Vein Material:%s%s%s",
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if (tform != NULL) { //(b->grasslevel > 0)
            const char* subMatName = lookupMaterialName(WOOD,b->grassmat);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Grass length:%d, Material: %s",
                              b->grasslevel, subMatName?subMatName:"");
        }
        //for(int j = 0; j < b->grasslevels.size(); j++)
        //{
        //	const char* subMatName = lookupMaterialName(WOOD,b->grassmats.at(j));
        //	draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
        //		"Grass length:%d, Material: %s",
        //		b->grasslevels.at(j), subMatName?subMatName:"");
        //}

        if(b->designation.bits.flow_size > 0 || b->tree.index != 0)
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "tree:%i water:%i,%i", b->tree.index, b->designation.bits.liquid_type, b->designation.bits.flow_size);
        if(b->tree.index != 0)
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "tree name:%s type:%i", lookupTreeName(b->tree.index), b->tree.type);
        if(b->building.sprites.size() != 0)
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "%i extra sprites.", b->building.sprites.size());

        // FIXME: classidstrings is no more
        //building
        if(b->building.info && b->building.type != BUILDINGTYPE_NA && b->building.type != BUILDINGTYPE_BLACKBOX && b->building.type != BUILDINGTYPE_TREE) {
            const char* matName = lookupMaterialTypeName(b->building.info->material.type);
            const char* subMatName = lookupMaterialName(b->building.info->material.type,b->building.info->material.index);
            const char* subTypeName = lookupBuildingSubtype(b->building.type, b->building.info->subtype);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Building: game_type = %s(%i) game_subtype = %s(%i) Material: %s%s%s (%d,%d)",
                              ENUM_KEY_STR(building_type, (building_type::building_type)b->building.type).c_str(),
                              b->building.type,
                              subTypeName,
                              b->building.info->subtype,
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"",
                              b->building.info->material.type,b->building.info->material.index);

            //if(b->building.custom_building_type != -1)
            //{
            //	draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
            //		"Custom workshop type %s (%d)", contentLoader->custom_workshop_types[b->building.custom_building_type].c_str(),b->building.custom_building_type);
            //}
        }

        if(b->designation.bits.traffic) {
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Traffic: %d", b->designation.bits.traffic);
        }
        if(b->designation.bits.pile) {
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Stockpile?");
        }
        if(b->designation.bits.water_table) {
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,"Water table");
        }
        if(b->designation.bits.rained) {
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,"Rained");
        }
        //if(b->building.type != BUILDINGTYPE_BLACKBOX) {
        //    draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
        //                      "Temp1: %dU, %.2f'C, %d'F", b->temp1, (float)(b->temp1-10000)*5.0f/9.0f, b->temp1-9968);
        //}
        if(b->snowlevel || b->mudlevel || b->bloodlevel) {
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Snow: %d, Mud: %d, Blood: %d", b->snowlevel, b->mudlevel, b->bloodlevel);
        }
        if(b->Item.item.type >= 0) {
            MaterialInfo mat;
            mat.decode(b->Item.matt.type, b->Item.matt.index);
            ItemTypeInfo itemdef;
            bool subtype = itemdef.decode((item_type::item_type)b->Item.item.type, b->Item.item.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Item: %s - %s",
                              mat.getToken().c_str(),
                              subtype?itemdef.getToken().c_str():"");
        }
        //borders
        /*
        	int dray = (i++*al_get_font_line_height(font));
        draw_textf_border(font, al_map_rgb(255,255,255), 16, dray, 0,
        	"Open: %d, floor: %d, Wall: %d, Ramp: %d Light: %d", b->openborders, b->floorborders, b->wallborders, b->rampborders, b->lightborders);
        draw_borders(8, dray, b->lightborders);
        */

        if(b->Eff_Miasma.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Miasma.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Miasma.matt.type,b->Eff_Miasma.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Miasma: %d, Material:%s%s%s",
                              b->Eff_Miasma.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Steam.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Steam.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Steam.matt.type,b->Eff_Steam.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Steam: %d, Material:%s%s%s",
                              b->Eff_Steam.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Mist.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Mist.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Mist.matt.type,b->Eff_Mist.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Mist: %d, Material:%s%s%s",
                              b->Eff_Mist.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_MaterialDust.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_MaterialDust.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_MaterialDust.matt.type,b->Eff_MaterialDust.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "MaterialDust: %d, Material:%s%s%s",
                              b->Eff_MaterialDust.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_MagmaMist.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_MagmaMist.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_MagmaMist.matt.type,b->Eff_MagmaMist.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "MagmaMist: %d, Material:%s%s%s",
                              b->Eff_MagmaMist.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Smoke.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Smoke.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Smoke.matt.type,b->Eff_Smoke.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Smoke: %d, Material:%s%s%s",
                              b->Eff_Smoke.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Dragonfire.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Dragonfire.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Dragonfire.matt.type,b->Eff_Dragonfire.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Dragonfire: %d, Material:%s%s%s",
                              b->Eff_Dragonfire.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Fire.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Fire.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Fire.matt.type,b->Eff_Fire.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Fire: %d, Material:%s%s%s",
                              b->Eff_Fire.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_Web.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_Web.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_Web.matt.type,b->Eff_Web.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "Web: %d, Material:%s%s%s",
                              b->Eff_Web.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_MaterialGas.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_MaterialGas.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_MaterialGas.matt.type,b->Eff_MaterialGas.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "MaterialGas: %d, Material:%s%s%s",
                              b->Eff_MaterialGas.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_MaterialVapor.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_MaterialVapor.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_MaterialVapor.matt.type,b->Eff_MaterialVapor.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "MaterialVapor: %d, Material:%s%s%s",
                              b->Eff_MaterialVapor.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_OceanWave.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_OceanWave.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_OceanWave.matt.type,b->Eff_OceanWave.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "OceanWave: %d, Material:%s%s%s",
                              b->Eff_OceanWave.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if(b->Eff_SeaFoam.density > 0) {
            const char* matName = lookupMaterialTypeName(b->Eff_SeaFoam.matt.type);
            const char* subMatName = lookupMaterialName(b->Eff_SeaFoam.matt.type,b->Eff_SeaFoam.matt.index);
            draw_textf_border(font, al_map_rgb(255,255,255), 2, (i++*al_get_font_line_height(font)), 0,
                              "SeaFoam: %d, Material:%s%s%s",
                              b->Eff_SeaFoam.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        break;
    }

    //basecon
    //textprintf(target, font, 2, ssState.ScreenH-20-(i--*10), 0xFFFFFF,
    //   "base: %d %d %d ", b->baseplate, b->basecon.type, b->basecon.index );
}

void DrawMinimap(WorldSegment * segment)
{
    int size = 100;
    //double oneTileInPixels;
    int posx = ssState.ScreenW-size-10;
    int posy = 10;

    if(!segment || segment->regionSize.x == 0 || segment->regionSize.y == 0) {
        draw_textf_border(font, al_map_rgb(255,255,255), posx, posy, 0, "No map loaded");
        return;
    }

    oneTileInPixels = (double) size / segment->regionSize.x;
    //map outine
    int mapheight = (int)(segment->regionSize.y * oneTileInPixels);
    al_draw_rectangle(posx, posy, posx+size, posy+mapheight, al_map_rgb(0,0,0),0);
    //current segment outline
    int x = (size * (segment->pos.x+1)) / segment->regionSize.x;
    int y = (mapheight * (segment->pos.y+1)) / segment->regionSize.y;
    MiniMapSegmentWidth = (segment->size.x-2) * oneTileInPixels;
    MiniMapSegmentHeight = (segment->size.y-2) * oneTileInPixels;
    al_draw_rectangle(posx+x, posy+y, posx+x+MiniMapSegmentWidth, posy+y+MiniMapSegmentHeight,al_map_rgb(0,0,0),0);
    MiniMapTopLeftX = posx;
    MiniMapTopLeftY = posy;
    MiniMapBottomRightX = posx+size;
    MiniMapBottomRightY = posy+mapheight;
}

ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet)
{
    int sheetx = spriteNum % SHEET_OBJECTSWIDE;
    int sheety = spriteNum / SHEET_OBJECTSWIDE;
    return al_create_sub_bitmap(spriteSheet, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT);
}

void DrawSpriteIndexOverlay(int imageIndex)
{
    ALLEGRO_BITMAP* currentImage;
    if (imageIndex == -1) {
        currentImage=IMGObjectSheet;
    } else {
        if( imageIndex >= (int)IMGFilelist.size()) {
            return;
        }
        currentImage=IMGFilelist[imageIndex];
    }
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_draw_bitmap(currentImage,0,0,0);
    for(int i =0; i<= 20*SPRITEWIDTH; i+=SPRITEWIDTH) {
        al_draw_line(i,0,i, ssState.ScreenH, al_map_rgb(0,0,0), 0);
    }
    for(int i =0; i< ssState.ScreenH; i+=SPRITEHEIGHT) {
        al_draw_line(0,i, 20*SPRITEWIDTH,i,al_map_rgb(0,0,0), 0);
    }

    for(int y = 0; y<20; y++) {
        for(int x = 0; x<20; x+=5) {
            int index = y * 20 + x;
            draw_textf_border(font, al_map_rgb(255,255,255),  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+(al_get_font_line_height(font)/2), 0, "%i", index);
        }
    }
    draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW-10, ssState.ScreenH -al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT,
                      "%s (%d) (Press SPACE to return)",
                      (imageIndex==-1?"objects.png":IMGFilenames[imageIndex]->c_str()), imageIndex);
    al_flip_display();
}


void DoSpriteIndexOverlay()
{
    DrawSpriteIndexOverlay(-1);
    int index = 0;
    int max = (int)IMGFilenames.size();
    while(true) {
        while(!al_key_down(&keyboard,ALLEGRO_KEY_SPACE) && !al_key_down(&keyboard,ALLEGRO_KEY_F10)) {
            al_get_keyboard_state(&keyboard);
            al_rest(ALLEGRO_MSECS_TO_SECS(50));
        }
        al_get_keyboard_state(&keyboard);
        if (al_key_down(&keyboard,ALLEGRO_KEY_SPACE)) {
            break;
        }
        DrawSpriteIndexOverlay(index);
        index++;
        if (index >= max) {
            index = -1;
        }
        //debounce f10
        al_get_keyboard_state(&keyboard);
        while(al_key_down(&keyboard,ALLEGRO_KEY_F10)) {
            al_get_keyboard_state(&keyboard);
            al_rest(ALLEGRO_MSECS_TO_SECS(50));
        }
    }
    //redraw screen again
    al_clear_to_color(ssConfig.backcol);
    paintboard();
}

void paintboard()
{
    //do the starting timer stuff
    clock_t starttime = clock();

    int op, src, dst, alpha_op, alpha_src, alpha_dst;
    al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
    al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO,ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    if(ssConfig.transparentScreenshots) {
        al_clear_to_color(al_map_rgba(0,0,0,0));
    } else {
        al_clear_to_color(ssConfig.backcol);
    }
    al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);

    // lock segment for painting and retrieve it.
    map_segment.lockDraw();
    WorldSegment * segment = map_segment.getDraw();
    if( segment == NULL ) {
        draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "Could not find DF process");
        map_segment.unlockDraw();
        return;
    }
    segment->DrawAllTiles();
    if (ssConfig.show_osd) {
        DrawCurrentLevelOutline(false);
    }
    
    //do the closing timer stuff
    clock_t donetime = clock();
    ssTimers.draw_time = (donetime - starttime)*0.1 + ssTimers.draw_time*0.9;
    ssTimers.frame_total = (donetime - ssTimers.prev_frame_time)*0.1 + ssTimers.frame_total*0.9;
    ssTimers.prev_frame_time = donetime;

    if (ssConfig.show_osd) {
        al_hold_bitmap_drawing(true);
        draw_textf_border(font, al_map_rgb(255,255,255), 10,al_get_font_line_height(font), 0, "%i,%i,%i, r%i, z%i", ssState.DisplayedSegmentX,ssState.DisplayedSegmentY,ssState.DisplayedSegmentZ, ssState.DisplayedRotation, ssConfig.zoom);

        if(ssConfig.debug_mode) {
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 3*al_get_font_line_height(font), 0, "Map Read Time: %.2fms", ssTimers.read_time);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 4*al_get_font_line_height(font), 0, "Map Beautification Time: %.2fms", ssTimers.beautify_time);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 5*al_get_font_line_height(font), 0, "Tile Sprite Assembly Time: %.2fms", ssTimers.assembly_time);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 2*al_get_font_line_height(font), 0, "FPS: %.2f", 1000.0/ssTimers.frame_total);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 6*al_get_font_line_height(font), 0, "Draw: %.2fms", ssTimers.draw_time);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 7*al_get_font_line_height(font), 0, "D1: %i", DebugInt1);
            draw_textf_border(font, al_map_rgb(255,255,255), 10, 8*al_get_font_line_height(font), 0, "%i/%i/%i, %i:%i", contentLoader->currentDay+1, contentLoader->currentMonth+1, contentLoader->currentYear, contentLoader->currentHour, (contentLoader->currentTickRel*60)/50);
            
            drawDebugCursorAndInfo(segment);
        }
        ssConfig.platecount = 0;
        int top = 0;
        if(ssConfig.follow_DFscreen) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Locked on DF screen + (%d,%d,%d)",ssConfig.viewXoffset,ssConfig.viewYoffset,ssConfig.viewZoffset);
        }
        if(ssConfig.follow_DFcursor && ssConfig.debug_mode) {
            top += al_get_font_line_height(font);
            if(ssConfig.dfCursorX != -30000) {
                draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Following DF Cursor at: %d,%d,%d", ssConfig.dfCursorX,ssConfig.dfCursorY,ssConfig.dfCursorZ);
            }
        }
        if(ssConfig.single_layer_view) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Single layer view");
        }
        if(ssConfig.automatic_reload_time) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Reloading every %0.1fs", (float)ssConfig.automatic_reload_time/1000);
        }
        al_hold_bitmap_drawing(false);
        DrawMinimap(segment);
    }
    map_segment.unlockDraw();
    al_flip_display();
}


bool load_from_path (ALLEGRO_PATH * p, const char * filename, ALLEGRO_BITMAP *& imgd)
{
    int index;
    al_set_path_filename(p,filename);
    index = loadImgFile(al_path_cstr(p,ALLEGRO_NATIVE_PATH_SEP));
    if(index == -1) {
        return false;
    }
    imgd = al_create_sub_bitmap(IMGFilelist[index], 0, 0, al_get_bitmap_width(IMGFilelist[index]), al_get_bitmap_height(IMGFilelist[index]));
    return true;
}

void loadGraphicsFromDisk()
{
    ALLEGRO_PATH * p = al_create_path_for_directory("stonesense");
    if(!load_from_path(p, "objects.png", IMGObjectSheet)) {
        return;
    }
    if(!load_from_path(p, "creatures.png", IMGCreatureSheet)) {
        return;
    }
    if(!load_from_path(p, "ramps.png", IMGRampSheet)) {
        return;
    }
    if(!load_from_path(p, "SSStatusIcons.png", IMGStatusSheet)) {
        return;
    }
    if(!load_from_path(p, "SSProfIcons.png", IMGProfSheet)) {
        return;
    }
    if(!load_from_path(p, "SSJobIcons.png", IMGJobSheet)) {
        return;
    }
    if(!load_from_path(p, "gibs.png", IMGBloodSheet)) {
        return;
    }
    if(!load_from_path(p, "engravings_floor.png", IMGEngFloorSheet)) {
        return;
    }
    if(!load_from_path(p, "engravings_left.png", IMGEngLeftSheet)) {
        return;
    }
    if(!load_from_path(p, "engravings_right.png", IMGEngRightSheet)) {
        return;
    }
    if(!load_from_path(p, "Sir_Henry's_32x32.png", IMGLetterSheet)) {
        return;
    }
    al_destroy_path(p);
    createEffectSprites();
}

//delete and clean out the image files
void flushImgFiles()
{
    LogVerbose("flushing images...\n");
    destroyEffectSprites();
    //should be OK because we keep others from directly acccessing this stuff
    if(IMGObjectSheet) {
        al_destroy_bitmap(IMGObjectSheet);
        IMGObjectSheet = 0;
    }
    if(IMGCreatureSheet) {
        al_destroy_bitmap(IMGCreatureSheet);
        IMGCreatureSheet = 0;
    }
    if(IMGRampSheet) {
        al_destroy_bitmap(IMGRampSheet);
        IMGRampSheet = 0;
    }
    if(IMGStatusSheet) {
        al_destroy_bitmap(IMGStatusSheet);
        IMGStatusSheet = 0;
    }
    if(IMGProfSheet) {
        al_destroy_bitmap(IMGProfSheet);
        IMGProfSheet = 0;
    }
    if(IMGJobSheet) {
        al_destroy_bitmap(IMGJobSheet);
        IMGJobSheet = 0;
    }
    if(IMGEngFloorSheet) {
        al_destroy_bitmap(IMGEngFloorSheet);
        IMGEngFloorSheet = 0;
    }
    if(IMGEngLeftSheet) {
        al_destroy_bitmap(IMGEngLeftSheet);
        IMGEngLeftSheet = 0;
    }
    if(IMGEngRightSheet) {
        al_destroy_bitmap(IMGEngRightSheet);
        IMGEngRightSheet = 0;
    }
    if(IMGLetterSheet) {
        al_destroy_bitmap(IMGLetterSheet);
        IMGLetterSheet = 0;
    }
    uint32_t numFiles = (uint32_t)IMGFilelist.size();
    assert( numFiles == IMGFilenames.size());
    for(uint32_t i = 0; i < numFiles; i++) {
        al_destroy_bitmap(IMGFilelist[i]);
        //should be same length, I hope
        delete(IMGFilenames[i]);
    }
    uint32_t caches = (uint32_t)IMGCache.size();
    for(uint32_t i = 0; i < caches; i++) {
        al_destroy_bitmap(IMGCache[i]);
    }
    IMGFilelist.clear();
    IMGFilenames.clear();
    IMGCache.clear();

}

ALLEGRO_BITMAP* getImgFile(int index)
{
    return IMGFilelist[index];
}

inline int returnGreater(int a, int b)
{
    if(a>b) {
        return a;
    } else {
        return b;
    }
}

int loadImgFile(const char* filename)
{
    if(ssConfig.cache_images) {
        static bool foundSize = false;
        if(!foundSize) {
            ALLEGRO_BITMAP* test = 0;
            while(true) {
                test = al_create_bitmap(ssConfig.imageCacheSize,ssConfig.imageCacheSize);
                if(test) {
                    LogError("%i works.\n", ssConfig.imageCacheSize);
                    break;
                }
                LogError("%i is too large. chopping it.\n", ssConfig.imageCacheSize);
                ssConfig.imageCacheSize = ssConfig.imageCacheSize / 2;
            }
            foundSize = true;
            al_destroy_bitmap(test);
        }
        int op, src, dst, alpha_op, alpha_src, alpha_dst;
        al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
        ALLEGRO_BITMAP* currentTarget = al_get_target_bitmap();
        uint32_t numFiles = (uint32_t)IMGFilelist.size();
        for(uint32_t i = 0; i < numFiles; i++) {
            if (strcmp(filename, IMGFilenames[i]->c_str()) == 0) {
                return i;
            }
        }
        /*
        al_clear_to_color(al_map_rgb(0,0,0));
        draw_textf_border(font, ssState.ScreenW/2,
        ssState.ScreenH/2,
        ALLEGRO_ALIGN_CENTRE, "Loading %s...", filename);
        al_flip_display();
        */
        static int xOffset = 0;
        static int yOffset = 0;
        int currentCache = IMGCache.size() -1;
        static int columnWidth = 0;
        ALLEGRO_BITMAP* tempfile = load_bitmap_withWarning(filename);
        if(!tempfile) {
            return -1;
        }
        LogVerbose("New image: %s\n",filename);
        if(currentCache < 0) {
            // FIXME: this is some really weird logic.
            IMGCache.push_back(al_create_bitmap(ssConfig.imageCacheSize, ssConfig.imageCacheSize));
            if(!IMGCache[0]) {
                // FIXME: so, what happens when al_create_bitmap fails? rainbows and unicorns?
                LogVerbose("Cannot create bitmap sized %ix%i, please chose a smaller size",ssConfig.imageCacheSize,ssConfig.imageCacheSize);
            }
            currentCache = IMGCache.size() -1;
            LogVerbose("Creating image cache #%d\n",currentCache);
        }
        if((yOffset + al_get_bitmap_height(tempfile)) <= ssConfig.imageCacheSize) {
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
            al_set_target_bitmap(IMGCache[currentCache]);
            al_draw_bitmap(tempfile, xOffset, yOffset, 0);
            IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
            yOffset += al_get_bitmap_height(tempfile);
            columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
        } else if ((xOffset + al_get_bitmap_width(tempfile) + columnWidth) <= ssConfig.imageCacheSize) {
            yOffset = 0;
            xOffset += columnWidth;
            columnWidth = 0;
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
            al_set_target_bitmap(IMGCache[currentCache]);
            al_draw_bitmap(tempfile, xOffset, yOffset, 0);
            IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
            yOffset += al_get_bitmap_height(tempfile);
            columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
        } else {
            yOffset = 0;
            xOffset = 0;
            IMGCache.push_back(al_create_bitmap(ssConfig.imageCacheSize, ssConfig.imageCacheSize));
            currentCache = IMGCache.size() -1;
            LogVerbose("Creating image cache #%d\n",currentCache);
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
            al_set_target_bitmap(IMGCache[currentCache]);
            al_draw_bitmap(tempfile, xOffset, yOffset, 0);
            IMGFilelist.push_back(al_create_sub_bitmap(IMGCache[currentCache], xOffset, yOffset, al_get_bitmap_width(tempfile), al_get_bitmap_height(tempfile)));
            yOffset += al_get_bitmap_height(tempfile);
            columnWidth = returnGreater(columnWidth, al_get_bitmap_width(tempfile));
        }
        if(ssConfig.saveImageCache) {
            saveImage(tempfile);
        }
        al_destroy_bitmap(tempfile);
        al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
        IMGFilenames.push_back(new string(filename));
        al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
        if(ssConfig.saveImageCache) {
            saveImage(IMGCache[currentCache]);
        }
        al_clear_to_color(al_map_rgb(0,0,0));
        al_flip_display();
        return (int)IMGFilelist.size() - 1;
    } else {
        uint32_t numFiles = (uint32_t)IMGFilelist.size();
        for(uint32_t i = 0; i < numFiles; i++) {
            if (strcmp(filename, IMGFilenames[i]->c_str()) == 0) {
                return i;
            }
        }
        ALLEGRO_BITMAP * temp = load_bitmap_withWarning(filename);
        if(!temp) {
            return -1;
        }
        IMGFilelist.push_back(temp);
        IMGFilenames.push_back(new string(filename));
        LogVerbose("New image: %s\n",filename);
        return (int)IMGFilelist.size() - 1;
    }
}

void saveScreenshot()
{
    al_clear_to_color(ssConfig.backcol);
    paintboard();
    //get filename
    char filename[20] = {0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true) {
        sprintf(filename, "screenshot%i.png", index);

        fp = fopen(filename, "r");
        if( fp != 0) {
            fclose(fp);
        } else
            //file does not exist, so exit loop
        {
            break;
        }
        index++;
    };
    //move image to 16 bits
    //al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA);
    ALLEGRO_BITMAP* temp = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
    al_set_target_bitmap(temp);
    PrintMessage("saving screenshot to %s\n", filename);
    if(!ssConfig.transparentScreenshots) {
        al_clear_to_color(ssConfig.backcol);
    }
    paintboard();
    al_save_bitmap(filename, temp);
    al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
    al_destroy_bitmap(temp);
    //al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY);
}

void saveImage(ALLEGRO_BITMAP* image)
{
    //get filename
    char filename[20] = {0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true) {
        sprintf(filename, "Image%i.png", index);

        fp = fopen(filename, "r");
        if( fp != 0) {
            fclose(fp);
        } else
            //file does not exist, so exit loop
        {
            break;
        }
        index++;
    };
    al_save_bitmap(filename, image);
}

void saveMegashot(bool tall)
{
    map_segment.lockRead();

    draw_textf_border(font, al_map_rgb(255,255,255), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "saving large screenshot...");
    al_flip_display();
    char filename[20] = {0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true) {
        sprintf(filename, "screenshot%i.png", index);
        fp = fopen(filename, "r");
        if( fp != 0) {
            fclose(fp);
        } else
            //file does not exist, so exit loop
        {
            break;
        }
        index++;
    };
    int timer = clock();
    //back up all the relevant values
    Crd3D backupsize = ssConfig.segmentSize;
    int tempViewx = ssState.DisplayedSegmentX;
    int tempViewy = ssState.DisplayedSegmentY;
    int tempViewz = ssState.DisplayedSegmentZ;
    bool tempFollow = ssConfig.follow_DFscreen;
    bool tempfog = ssConfig.fogenable;
    bool temposd = ssConfig.show_osd;
    int tempLift = ssConfig.lift_segment_offscreen;
    int tempW = ssState.ScreenW;
    int tempH = ssState.ScreenH;
    //now make them real big.
    ssConfig.show_osd = false;
    ssConfig.follow_DFscreen = false;
    ssConfig.fogenable = false;
    ssConfig.lift_segment_offscreen = 0;

    if(tall) {
        ssConfig.segmentSize.z = ssState.DisplayedSegmentZ + 1;
    }
    parms.sizez = ssConfig.segmentSize.z;
    parms.z = ssState.DisplayedSegmentZ;

    //make the image
    ssState.ScreenW = (ssConfig.blockDimX * PLATEWIDTH)*ssConfig.scale;
    ssState.ScreenH = ( ((ssConfig.blockDimX + ssConfig.blockDimY) * PLATEHEIGHT / 2) + ((ssConfig.segmentSize.z - 1) * TILEHEIGHT) )*ssConfig.scale;
    
    bigFile = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);

    //draw and save the image
    if(bigFile) {
        PrintMessage("saving large screenshot to %s\n", filename);
        al_set_target_bitmap(bigFile);
        if(!ssConfig.transparentScreenshots) {
            al_clear_to_color(ssConfig.backcol);
        }

        //here we deal with the rotations
        int startx, incrx, numx;
        int starty, incry, numy;

        startx = -1;
        starty = -1;
        incrx = parms.sizex-2;
        incry = parms.sizey-2;
        numx = (int)(ssConfig.blockDimX+3);
        numx = numx/incrx + (numx%incrx==0 ? 0 : 1);
        numy = (int)(ssConfig.blockDimY+3);
        numy = numy/incry + (numx%incry==0 ? 0 : 1);

        if(ssState.DisplayedRotation == 1 || ssState.DisplayedRotation == 2) {
            starty = (int)ssConfig.blockDimY + 2 - incry;
            ssState.DisplayedSegmentY = (int)ssConfig.blockDimY - incry - 1;
            incry = -incry;
        } else {
            ssState.DisplayedSegmentY = -1;
        }

        if(ssState.DisplayedRotation == 3 || ssState.DisplayedRotation == 2) {
            startx = (int)ssConfig.blockDimX + 2 - incrx;
            ssState.DisplayedSegmentX = (int)ssConfig.blockDimX - incrx - 1;
            incrx = -incrx;
        } else {
            ssState.DisplayedSegmentX = -1;
        }

        parms.x = startx;
        parms.y = starty;

        //now actually loop through and draw the subsegments
        for(int i=0; i<numy; i++) {
            for(int j=0; j<numx; j++) {
                //read and draw each individual segment
                read_segment(NULL);
                map_segment.lockDraw();
                WorldSegment * segment = map_segment.getDraw();
                segment->DrawAllTiles();
                map_segment.unlockDraw();

                parms.x += incrx;
            }
            parms.x = startx;
            parms.y += incry;
        }


        al_save_bitmap(filename, bigFile);
        al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
        timer = clock() - timer;
        PrintMessage("\ttime for screenshot %ims\n", timer);
    } else {
        LogError("failed to take large screenshot; try zooming out or using software mode\n");
    }
    al_destroy_bitmap(bigFile);
    //restore everything that we changed.
    ssConfig.segmentSize = backupsize;
    parms.sizex = ssConfig.segmentSize.x;
    parms.sizey = ssConfig.segmentSize.y;
    parms.sizez = ssConfig.segmentSize.z;
    ssState.DisplayedSegmentX = tempViewx;
    ssState.DisplayedSegmentY = tempViewy;
    ssState.DisplayedSegmentZ = tempViewz;
    parms.x = ssState.DisplayedSegmentX;
    parms.y = ssState.DisplayedSegmentY;
    parms.z = ssState.DisplayedSegmentZ;
    ssConfig.fogenable = tempfog;
    ssConfig.follow_DFscreen = tempFollow;
    ssConfig.lift_segment_offscreen = tempLift;
    ssConfig.show_osd = temposd;
    ssState.ScreenW = tempW;
    ssState.ScreenH = tempH;

    map_segment.unlockRead();
}
