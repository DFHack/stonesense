#include <assert.h>
#include <vector>

#include "common.h"
#include "Tile.h"
#include "GUI.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "MapLoading.h"
#include "MiscUtils.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include "Tile.h"
#include "UserInput.h"

#include "modules/Units.h"

#include "df/plotinfost.h"
#include "df/building_actual.h"
#include "df/world.h"

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
#include "df/descriptor_pattern.h"

#include "df/creature_raw.h"
#include "df/caste_raw.h"
#include "df/tissue_style_raw.h"

/*FIXME when adventure mode returns.
#include "df/viewscreen_dungeonmodest.h"
#include "df/viewscreen_dungeon_wrestlest.h"
*/
#include "df/adventurest.h"
#include "df/report.h"

#include "allegro5/allegro_color.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

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
ALLEGRO_BITMAP* IMGBloodSheet;
ALLEGRO_BITMAP* IMGEngFloorSheet;
ALLEGRO_BITMAP* IMGEngLeftSheet;
ALLEGRO_BITMAP* IMGEngRightSheet;
ALLEGRO_BITMAP* IMGLetterSheet;

ALLEGRO_BITMAP* buffer = 0;
ALLEGRO_BITMAP* bigFile = 0;
vector<ALLEGRO_BITMAP*> IMGCache;
vector<ALLEGRO_BITMAP*> IMGFilelist;
vector<std::unique_ptr<string>> IMGFilenames;
GLhandleARB tinter;
GLhandleARB tinter_shader;

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
        draw_diamond(x, y, uiColor(1));
    } else {
        draw_diamond(x, y, uiColor(0));
    }

    if(borders & 2) {
        draw_diamond(x+4, y+4, uiColor(1));
    } else {
        draw_diamond(x+4, y+4, uiColor(0));
    }

    if(borders & 4) {
        draw_diamond(x+8, y+8, uiColor(1));
    } else {
        draw_diamond(x+8, y+8, uiColor(0));
    }

    if(borders & 8) {
        draw_diamond(x+4, y+12, uiColor(1));
    } else {
        draw_diamond(x+4, y+12, uiColor(0));
    }

    if(borders & 16) {
        draw_diamond(x, y+16, uiColor(1));
    } else {
        draw_diamond(x, y+16, uiColor(0));
    }

    if(borders & 32) {
        draw_diamond(x-4, y+12, uiColor(1));
    } else {
        draw_diamond(x-4, y+12, uiColor(0));
    }

    if(borders & 64) {
        draw_diamond(x-8, y+8, uiColor(1));
    } else {
        draw_diamond(x-8, y+8, uiColor(0));
    }

    if(borders & 128) {
        draw_diamond(x-4, y+4, uiColor(1));
    } else {
        draw_diamond(x-4, y+4, uiColor(0));
    }

}

void ScreenToPoint(int inx,int iny,int &x1, int &y1, int &z1, int segSizeX, int segSizeY, int segSizeZ, int ScreenW, int ScreenH)
{
    float x=inx;
    float y=iny;
    x-=ScreenW / 2.0;
    y-=ScreenH / 2.0;

    y = y/ssConfig.scale;
    y += TILETOPHEIGHT*5.0/4.0;
    y += ssConfig.lift_segment_offscreen_y;
    z1 = segSizeZ-2;
    y += z1*TILEHEIGHT;
    y = 2 * y / TILETOPHEIGHT;
    y += (segSizeX/2) + (segSizeY/2);

    x = x/ssConfig.scale;
    x -= ssConfig.lift_segment_offscreen_x;
    x = 2 * x / TILEWIDTH;
    x += (segSizeX/2) - (segSizeY/2);

    x1 = (x + y)/2;
    y1 = (y - x)/2;

}

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1)
{
    if(ssConfig.track_screen_center){
        ScreenToPoint(x, y, x1, y1, z1, ssState.Size.x, ssState.Size.y, ssState.Size.z, ssState.ScreenW, ssState.ScreenH);
    } else {
        ScreenToPoint(x, y, x1, y1, z1, 0, 0, ssState.Size.z, 0, 0);
    }
}

void pointToScreen(int *inx, int *iny, int inz, int segSizeX, int segSizeY, int ScreenW, int ScreenH){
    int z = inz + 1 - ssState.Size.z;

    int x = *inx-*iny;
    x-=(segSizeX/2) - (segSizeY/2);
    x = x * TILEWIDTH / 2;
    x += ssConfig.lift_segment_offscreen_x;
    x *= ssConfig.scale;

    int y = *inx+*iny;
    y-=(segSizeX/2) + (segSizeY/2);
    y = y*TILETOPHEIGHT / 2;
    y -= z*TILEHEIGHT;
    y -= TILETOPHEIGHT*5/4;
    y -= ssConfig.lift_segment_offscreen_y;
    y *= ssConfig.scale;

    x+=ScreenW / 2;
    y+=ScreenH / 2;

    *inx=x;
    *iny=y;
}

void pointToScreen(int *inx, int *iny, int inz)
{
    if(ssConfig.track_screen_center){
        pointToScreen(inx, iny, inz, ssState.Size.x, ssState.Size.y, ssState.ScreenW, ssState.ScreenH);
    } else {
        pointToScreen(inx, iny, inz, 0, 0, 0, 0);
    }
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

void draw_report_border(const ALLEGRO_FONT *font, float x, float y, int flags, const df::report * report)
{
    ALLEGRO_COLOR color = ssConfig.colors.getDfColor(report->color, ssConfig.useDfColors);
    draw_text_border(font, color, x, y, flags, DF2UTF(report->text).c_str());
}

void draw_announcements(const ALLEGRO_FONT *font, float x, float y, int flags, std::vector<df::report *> &announcements)
{
    for (int i = announcements.size() - 1; i >= 0 && announcements[i]->duration > 0; i--)
    {
        int offset = ((announcements.size() - 1) - i) * al_get_font_line_height(font);
        draw_report_border(font, x, y - offset, flags, announcements[i]);
    }
}

void draw_loading_message(const char *format, ...)
{
    al_clear_to_color(uiColor(0));
    ALLEGRO_COLOR color = uiColor(1);

    int flags = ALLEGRO_ALIGN_CENTRE;

    int x = al_get_bitmap_width(al_get_target_bitmap()) / 2;
    int y = al_get_bitmap_height(al_get_target_bitmap()) / 2;

    ALLEGRO_USTR *buf;
    va_list arglist;
    const char *s;

    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(arglist, format);
        s = va_arg(arglist, const char *);
        draw_text_border(font, color, x, y, flags, s);
        va_end(arglist);
    }
    else
    {
        va_start(arglist, format);
        buf = al_ustr_new("");
        al_ustr_vappendf(buf, format, arglist);
        va_end(arglist);
        draw_ustr_border(font, color, x, y, flags, buf);
        al_ustr_free(buf);
    }
    al_flip_display();
}

void correctTileForDisplayedOffset(int32_t& x, int32_t& y, int32_t& z)
{
    x -= ssState.Position.x;
    y -= ssState.Position.y; //Position.y;
    z -= ssState.Position.z - 1; // + viewedSegment->sizez - 2; // loading one above the top of the displayed segment for tile rules
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
    int x = ssState.Position.x+1;
    int y = ssState.Position.y+1;
    int z = ssState.Position.z + ssState.Size.z - 1;
    int sizex = ssState.Size.x-2;
    int sizey = ssState.Size.y-2;

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
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, uiColor(0), 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
        al_draw_line(p2.x, p2.y, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);

        al_draw_line(p1.x, p1.y, p3.x, p3.y, uiColor(0), 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
        al_draw_line(p3.x, p3.y, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
    } else {
        al_draw_line(p4.x, p4.y, p4.x, p4.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
        al_draw_line(p4.x, p4.y, p2.x, p2.y, uiColor(0) ,0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(0) ,0);

        al_draw_line(p4.x, p4.y, p3.x, p3.y, uiColor(0), 0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(0), 0);
    }
}

void drawSelectionCursor(WorldSegment * segment)
{
    Crd3D selection = segment->segState.dfSelection;
    if( (selection.x != -30000 && ssConfig.follow_DFcursor)
        || (ssConfig.track_mode == GameConfiguration::TRACKING_FOCUS) ){
            segment->CorrectTileForSegmentOffset(selection.x, selection.y, selection.z);
            segment->CorrectTileForSegmentRotation(selection.x, selection.y, selection.z);
    } else {
        return;
    }
    Crd2D point = LocalTileToScreen(selection.x, selection.y, selection.z);
    int sheetx = SPRITEOBJECT_CURSOR % SHEET_OBJECTSWIDE;
    int sheety = SPRITEOBJECT_CURSOR / SHEET_OBJECTSWIDE;
    al_draw_tinted_scaled_bitmap(
        IMGObjectSheet,
        uiColor(3),
        sheetx * SPRITEWIDTH,
        sheety * SPRITEHEIGHT,
        SPRITEWIDTH,
        SPRITEHEIGHT,
        point.x-((SPRITEWIDTH/2)*ssConfig.scale),
        point.y - (WALLHEIGHT)*ssConfig.scale,
        SPRITEWIDTH*ssConfig.scale,
        SPRITEHEIGHT*ssConfig.scale,
        0);
}

void drawDebugCursor(WorldSegment * segment)
{
    Crd3D cursor = segment->segState.dfCursor;
    segment->CorrectTileForSegmentOffset(cursor.x, cursor.y, cursor.z);
    segment->CorrectTileForSegmentRotation(cursor.x, cursor.y, cursor.z);

    Crd2D point = LocalTileToScreen(cursor.x, cursor.y, cursor.z);
    int sheetx = SPRITEOBJECT_CURSOR % SHEET_OBJECTSWIDE;
    int sheety = SPRITEOBJECT_CURSOR / SHEET_OBJECTSWIDE;
    al_draw_tinted_scaled_bitmap(
        IMGObjectSheet,
        uiColor(2),
        sheetx * SPRITEWIDTH,
        sheety * SPRITEHEIGHT,
        SPRITEWIDTH,
        SPRITEHEIGHT,
        point.x-((SPRITEWIDTH/2)*ssConfig.scale),
        point.y - (WALLHEIGHT)*ssConfig.scale,
        SPRITEWIDTH*ssConfig.scale,
        SPRITEHEIGHT*ssConfig.scale,
        0);
}

void drawAdvmodeMenuTalk(const ALLEGRO_FONT *font, int x, int y)
{
    //df::adventure * menu = df::global::adventure;
    //if (!menu)
    //    return;
    //if (menu->talk_targets.size() == 0)
    //    return;
    //int line = menu->talk_targets.size() + 3;
    //draw_textf_border(font, ssConfig.colors.getDfColor(dfColors::white, ssConfig.useDfColors), x, (y - (line*al_get_font_line_height(font))), 0,
    //    "Who will you talk to?");
    //line -= 2;
    //for (int i = 0; i < menu->talk_targets.size(); i++)
    //{
    //    ALLEGRO_COLOR color = ssConfig.colors.getDfColor(dfColors::lgray, ssConfig.useDfColors);
    //    if (i == menu->talk_target_selection)
    //        color = ssConfig.colors.getDfColor(dfColors::white, ssConfig.useDfColors);
    //    df::unit * crete = Units::GetCreature(Units::FindIndexById(menu->talk_targets[i]->unit_id));
    //    if (crete)
    //    {
    //        ALLEGRO_USTR * string = al_ustr_newf("%s, ", Units::getProfessionName(crete).c_str());
    //        int8_t gender = df::global::world->raws.creatures.all[crete->race]->caste[crete->caste]->gender;
    //        if (gender == 0)
    //            al_ustr_append_chr(string, 0x2640);
    //        else if (gender == 1)
    //            al_ustr_append_chr(string, 0x2642);
    //        draw_ustr_border(font, color, x + 5, (y - ((line - i)*al_get_font_line_height(font))), 0,
    //            string);
    //    }
    //}
}

void drawDebugInfo(WorldSegment * segment)
{
    using df::global::plotinfo;

    //get tile info
    Tile* b = segment->getTile(
        segment->segState.dfCursor.x,
        segment->segState.dfCursor.y,
        segment->segState.dfCursor.z);
    int i = 10;
    if(b) {
        draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0, "Tile 0x%x (%i,%i,%i)", b, b->x, b->y, b->z);
    }
    df::viewscreen * vs = Gui::getCurViewscreen();
    /*FIXME when adventure mode returns.
    if (strict_virtual_cast<df::viewscreen_dungeonmodest>(vs))
    {
        if (df::global::adventure)
        {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Current menu: %s",
                df::enum_traits<df::ui_advmode_menu>::key_table[df::global::adventure->menu]);
        }
    }
    */
    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
        "Coord:(%i,%i,%i)", segment->segState.dfCursor.x, segment->segState.dfCursor.y, segment->segState.dfCursor.z);

    if(!b) {
        return;
    }
    int ttype;
    const char* tform = NULL;
    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
        "Tile: %s", enum_item_key_str(b->tileType));
    if (b->tileShapeBasic() == tiletype_shape_basic::Floor) {
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
    } else {
        ttype=-1;
    }

    ////FIXME: Figure out a different way to tell what's selected, because we can't depend on the sidebar mode anymore.
    //switch(plotinfo->main.mode) {
    //case ui_sidebar_mode::BuildingItems:
    //    if(b->building.info && b->building.type != BUILDINGTYPE_NA && b->building.type != BUILDINGTYPE_BLACKBOX && b->building.type != BUILDINGTYPE_TREE) {
    //        auto Actual_building = virtual_cast<df::building_actual>(b->building.info->origin);
    //        if(!Actual_building) {
    //            break;
    //        }
    //        CoreSuspender csusp;
    //        const size_t num_items = Actual_building->contained_items.size(); //Item array.
    //        std::string BuildingName;
    //        Actual_building->getName(&BuildingName);
    //        draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                          "%s",
    //                          BuildingName.c_str());
    //        for(size_t index = 0; index < num_items; index++) {
    //            MaterialInfo mat;
    //            mat.decode(Actual_building->contained_items[index]->item->getMaterial(), Actual_building->contained_items[index]->item->getMaterialIndex());
    //            char stacknum[8] = {0};
    //            sprintf(stacknum, " [%d]", Actual_building->contained_items[index]->item->getStackSize());
    //            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                              "%s - %s%s%s%s%s",
    //                              mat.getToken().c_str(),
    //                              ENUM_KEY_STR(item_type, Actual_building->contained_items[index]->item->getType()).c_str(),
    //                              (Actual_building->contained_items[index]->item->getSubtype()>=0)?"/":"",
    //                              (Actual_building->contained_items[index]->item->getSubtype()>=0)?get_item_subtype(Actual_building->contained_items[index]->item->getType(),Actual_building->contained_items[index]->item->getSubtype()):"",
    //                              Actual_building->contained_items[index]->item->getStackSize()>1?stacknum:"",
    //                              (Actual_building->contained_items[index]->use_mode == 2?" [B]":""));
    //        }
    //    }
    //    break;
    //case ui_sidebar_mode::ViewUnits:
    //    //creatures
    //    if(!b->occ.bits.unit || !b->creature) {
    //        break;
    //    }
    //    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                      "Creature:%s(%i) Caste:%s(%i) Job:%s",
    //                      contentLoader->Mats->race.at(b->creature->origin->race).id.c_str(), b->creature->origin->race,
    //                      contentLoader->Mats->raceEx.at(b->creature->origin->race).castes.at(b->creature->origin->caste).id.c_str(), b->creature->origin->caste,
    //                      contentLoader->professionStrings.at(b->creature->profession).c_str());

    //    //Inventories!
    //    if(b->creature && b->creature->inv) {
    //        for(size_t item_type_idex = 0; item_type_idex < b->creature->inv->item.size(); item_type_idex++) {
    //            if(b->creature->inv->item[item_type_idex].empty()) {
    //                continue;
    //            }
    //            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                "%s:", ENUM_KEY_STR(item_type, (item_type::item_type)item_type_idex).c_str());
    //            for(size_t ind = 0; ind < b->creature->inv->item[item_type_idex].size(); ind++) {
    //                if(b->creature->inv->item[item_type_idex][ind].empty()) {
    //                    continue;
    //                }
    //                draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                    "    %s",
    //                    get_item_subtype((item_type::item_type)item_type_idex,ind));
    //                for(size_t layerindex = 0; layerindex < b->creature->inv->item[item_type_idex][ind].size(); layerindex++)
    //                {
    //                    if(b->creature->inv->item[item_type_idex][ind][layerindex].matt.type < 0) {
    //                        continue;
    //                    }
    //                    MaterialInfo mat;
    //                    mat.decode(b->creature->inv->item[item_type_idex][ind][layerindex].matt.type,b->creature->inv->item[item_type_idex][ind][layerindex].matt.index);
    //                    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //                        "        %s",
    //                        mat.getToken().c_str());
    //                }
    //            }
    //        }
    //    }
    //    //just so it has it's own scope
    //    {
    //        char strCreature[150] = {0};
    //        generateCreatureDebugString( b->creature, strCreature );
    //        //memset(strCreature, -1, 50);
    //        /*
    //        // FIXME:: getJob is no more.
    //        try{
    //        draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
    //        "flag1: %s Sex: %d  Mood: %d Job: %s", strCreature, b->creature->sex + 1, b->creature->mood, (b->creature->current_job.active?contentLoader->MemInfo->getJob(b->creature->current_job.jobType).c_str():""));
    //        }
    //        catch(exception &e)
    //        {
    //        WriteErr("DFhack exeption: %s\n", e.what());
    //        }
    //        */

    //        int yy = (i++*al_get_font_line_height(font));
    //        int xx = 2;
    //        for(unsigned int j = 0; j<b->creature->nbcolors ; j++) {
    //            if(b->creature->color[j] < contentLoader->Mats->raceEx.at(b->creature->origin->race).castes.at(b->creature->origin->caste).ColorModifier[j].colorlist.size()) {
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    " %s:", contentLoader->Mats->raceEx[b->creature->origin->race].castes[b->creature->origin->caste].ColorModifier[j].part.c_str());
    //                xx += get_textf_width(font, " %s:", contentLoader->Mats->raceEx[b->creature->origin->race].castes[b->creature->origin->caste].ColorModifier[j].part.c_str());
    //                uint32_t cr_color = contentLoader->Mats->raceEx[b->creature->origin->race].castes[b->creature->origin->caste].ColorModifier[j].colorlist[b->creature->color[j]];
    //                if(cr_color < df::global::world->raws.descriptors.patterns.size()) {
    //                    for(size_t patternin = 0; patternin < df::global::world->raws.descriptors.patterns[cr_color]->colors.size(); patternin++){
    //                        uint16_t actual_color = df::global::world->raws.descriptors.patterns[cr_color]->colors[patternin];
    //                        al_draw_filled_rectangle(xx, yy, xx+al_get_font_line_height(font), yy+al_get_font_line_height(font),
    //                            al_map_rgb_f(
    //                            contentLoader->Mats->color[actual_color].red,
    //                            contentLoader->Mats->color[actual_color].green,
    //                            contentLoader->Mats->color[actual_color].blue));
    //                        xx += al_get_font_line_height(font);
    //                    }
    //                }
    //            }
    //        }
    //        yy = (i++*al_get_font_line_height(font));
    //        xx = 2;
    //        draw_textf_border(font,
    //            uiColor(1), xx, yy, 0,
    //            "hair lengths:");
    //        xx += get_textf_width(font, "hair lengths:");
    //        for(int j = 0; j < hairtypes_end; j++){
    //            draw_textf_border(font,
    //                uiColor(1), xx, yy, 0,
    //                "%d,", b->creature->hairlength[j]);
    //            xx += get_textf_width(font, "%d,", b->creature->hairlength[j]);
    //        }
    //        yy = (i++*al_get_font_line_height(font));
    //        xx = 2;
    //        draw_textf_border(font,
    //            uiColor(1), xx, yy, 0,
    //            "hair styles:");
    //        xx += get_textf_width(font, "hair styles:");
    //        for(int j = 0; j < hairtypes_end; j++){
    //            switch( b->creature->hairstyle[j]){
    //            case NEATLY_COMBED:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "NEATLY_COMBED-");
    //                xx += get_textf_width(font, "NEATLY_COMBED-");
    //                break;
    //            case BRAIDED:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "BRAIDED-");
    //                xx += get_textf_width(font, "BRAIDED-");
    //                break;
    //            case DOUBLE_BRAID:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "DOUBLE_BRAID-");
    //                xx += get_textf_width(font, "DOUBLE_BRAID-");
    //                break;
    //            case PONY_TAILS:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "PONY_TAILS-");
    //                xx += get_textf_width(font, "PONY_TAILS-");
    //                break;
    //            case CLEAN_SHAVEN:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "CLEAN_SHAVEN-");
    //                xx += get_textf_width(font, "CLEAN_SHAVEN-");
    //                break;
    //            default:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "UNKNOWN-");
    //                xx += get_textf_width(font, "UNKNOWN-");
    //            }
    //            switch(j){
    //            case HAIR:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "HAIR, ");
    //                xx += get_textf_width(font, "HAIR, ");
    //                break;
    //            case BEARD:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "BEARD, ");
    //                xx += get_textf_width(font, "BEARD, ");
    //                break;
    //            case MOUSTACHE:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "MOUSTACHE, ");
    //                xx += get_textf_width(font, "MOUSTACHE, ");
    //                break;
    //            case SIDEBURNS:
    //                draw_textf_border(font,
    //                    uiColor(1), xx, yy, 0,
    //                    "SIDEBURNS, ");
    //                xx += get_textf_width(font, "SIDEBURNS, ");
    //                break;
    //            }
    //        }
    //    }
    // break;
    //default:

        draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                          "Game Mode:%i, Control Mode:%i", contentLoader->gameMode.g_mode, contentLoader->gameMode.g_type);
        if (tform != NULL && b->material.type != INVALID_INDEX) {
            const char* formName = lookupFormName(b->consForm);
            const char* matName = lookupMaterialTypeName(b->material.type);
            const char* subMatName = lookupMaterialName(b->material.type,b->material.index);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "%s %s:%i Material:%s%s%s (%d,%d)", formName, tform, ttype,
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"", b->material.type,b->material.index);
        }
        if (tform != NULL && b->material.type != INVALID_INDEX && b->material.index != INVALID_INDEX) {
            MaterialInfo mat;
            mat.decode(b->material.type, b->material.index);
            ALLEGRO_COLOR color = al_map_rgb_f(contentLoader->Mats->color[mat.material->state_color[0]].red, contentLoader->Mats->color[mat.material->state_color[0]].green, contentLoader->Mats->color[mat.material->state_color[0]].blue);
            draw_textf_border(font, color, 2, (i++*al_get_font_line_height(font)), 0,
                              "%s", mat.material->state_name[0].c_str());
        }    //if (tform != NULL)
        //{
        //    draw_textf_border(font, 2, (i++*al_get_font_line_height(font)), 0,
        //        "MaterialType: %d, MaterialIndex: %d", b->material.type, b->material.index);
        //}
        if (tform != NULL) {
            const char* matName = lookupMaterialTypeName(b->layerMaterial.type);
            const char* subMatName = lookupMaterialName(b->layerMaterial.type,b->layerMaterial.index);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Layer Material:%s%s%s",
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if ((tform != NULL) && b->hasVein == 1) {
            const char* matName = lookupMaterialTypeName(b->veinMaterial.type);
            const char* subMatName = lookupMaterialName(b->veinMaterial.type,b->veinMaterial.index);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Vein Material:%s%s%s",
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
        }
        if (tform != NULL) { //(b->grasslevel > 0)
            const char* subMatName = lookupMaterialName(WOOD,b->grassmat);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Grass length:%d, Material: %s",
                              b->grasslevel, subMatName?subMatName:"");
        }
        //for(int j = 0; j < b->grasslevels.size(); j++)
        //{
        //    const char* subMatName = lookupMaterialName(WOOD,b->grassmats.at(j));
        //    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
        //        "Grass length:%d, Material: %s",
        //        b->grasslevels.at(j), subMatName?subMatName:"");
        //}

        if(b->designation.bits.flow_size > 0 || b->tree.index != 0)
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "tree:%i water:%i,%i", b->tree.index, b->designation.bits.liquid_type, b->designation.bits.flow_size);
        if (b->tree.index != 0)
        {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "tree name:%s type:%i", lookupTreeName(b->tree.index), b->tree.type);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "tree tile:%s%s%s%s%s%s%s",
                b->tree_tile.bits.trunk ? " trunk" : "",
                b->tree_tile.bits.connection_east ? " >" : "",
                b->tree_tile.bits.connection_south ? " v" : "",
                b->tree_tile.bits.connection_west ? " <" : "",
                b->tree_tile.bits.connection_north ? " ^" : "",
                b->tree_tile.bits.branches ? " branches" : "",
                b->tree_tile.bits.twigs ? " twigs" : ""
                );
        }
        if(b->building.sprites.size() != 0)
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "%i extra sprites.", b->building.sprites.size());

        // FIXME: classidstrings is no more
        //building
        if(b->building.info && b->building.type != BUILDINGTYPE_NA && b->building.type != BUILDINGTYPE_BLACKBOX && b->building.type != BUILDINGTYPE_TREE) {
            const char* matName = lookupMaterialTypeName(b->building.info->material.type);
            const char* subMatName = lookupMaterialName(b->building.info->material.type,b->building.info->material.index);
            const char* subTypeName = lookupBuildingSubtype(b->building.type, b->building.info->subtype);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Building: game_type = %s(%i) game_subtype = %s(%i) Material: %s%s%s (%d,%d) Occupancy:%i, Special: %i ",
                              ENUM_KEY_STR(building_type, (building_type::building_type)b->building.type).c_str(),
                              b->building.type,
                              subTypeName,
                              b->building.info->subtype,
                              matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"",
                              b->building.info->material.type,b->building.info->material.index,
                              b->occ.bits.building,
                              b->building.special);
            for(size_t index = 0; index < b->building.constructed_mats.size(); index++) {
            const char* partMatName = lookupMaterialTypeName(b->building.constructed_mats[index].matt.type);
            const char* partSubMatName = lookupMaterialName(b->building.constructed_mats[index].matt.type, b->building.constructed_mats[index].matt.index);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Material[%i]: %s%s%s (%d,%d)",
                              index,
                              partMatName?partMatName:"Unknown",partSubMatName?"/":"",partSubMatName?partSubMatName:"",
                              b->building.constructed_mats[index].matt.type, b->building.constructed_mats[index].matt.index);
            }

            //if(b->building.custom_building_type != -1)
            //{
            //    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
            //        "Custom workshop type %s (%d)", contentLoader->custom_workshop_types[b->building.custom_building_type].c_str(),b->building.custom_building_type);
            //}
        }

        if(b->designation.bits.traffic) {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Traffic: %d", b->designation.bits.traffic);
        }
        if(b->designation.bits.pile) {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Stockpile?");
        }
        if(b->designation.bits.water_table) {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,"Water table");
        }
        if(b->designation.bits.rained) {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,"Rained");
        }
        //if(b->building.type != BUILDINGTYPE_BLACKBOX) {
        //    draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
        //                      "Temp1: %dU, %.2f'C, %d'F", b->temp1, (float)(b->temp1-10000)*5.0f/9.0f, b->temp1-9968);
        //}
        if(b->snowlevel || b->mudlevel || b->bloodlevel) {
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Snow: %d, Mud: %d, Blood: %d", b->snowlevel, b->mudlevel, b->bloodlevel);
        }
        if(b->Item.item.type >= 0) {
            MaterialInfo mat;
            mat.decode(b->Item.matt.type, b->Item.matt.index);
            ItemTypeInfo itemdef;
            bool subtype = itemdef.decode((item_type::item_type)b->Item.item.type, b->Item.item.index);
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                              "Item: %s - %s",
                              mat.getToken().c_str(),
                              subtype?itemdef.getToken().c_str():"");
        }
        //borders
        /*
            int dray = (i++*al_get_font_line_height(font));
        draw_textf_border(font, uiColor(1), 16, dray, 0,
            "Open: %d, floor: %d, Wall: %d, Ramp: %d Light: %d", b->openborders, b->floorborders, b->wallborders, b->rampborders, b->lightborders);
        draw_borders(8, dray, b->lightborders);
        */
        const char* matName = lookupMaterialTypeName(b->tileeffect.matt.type);
        const char* subMatName = lookupMaterialName(b->tileeffect.matt.type,b->tileeffect.matt.index);
        switch(b->tileeffect.type){
        case df::flow_type::Miasma:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Miasma: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Steam:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Steam: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Mist:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Mist: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::MaterialDust:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "MaterialDust: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::MagmaMist:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "MagmaMist: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Smoke:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Smoke: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Dragonfire:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Dragonfire: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Fire:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Fire: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::Web:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "Web: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::MaterialGas:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "MaterialGas: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::MaterialVapor:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "MaterialVapor: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::OceanWave:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "OceanWave: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::SeaFoam:
            draw_textf_border(font, uiColor(1), 2, (i++*al_get_font_line_height(font)), 0,
                "SeaFoam: %d, Material:%s%s%s",
                b->tileeffect.density, matName?matName:"Unknown",subMatName?"/":"",subMatName?subMatName:"");
            break;
        case df::flow_type::ItemCloud:
            // TODO
            break;
        }
    //    break;
    //}

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

    if(!segment || segment->segState.RegionDim.x == 0 || segment->segState.RegionDim.y == 0) {
        draw_textf_border(font, uiColor(1), posx, posy, 0, "No map loaded");
        return;
    }

    oneTileInPixels = (double) size / segment->segState.RegionDim.x;
    //map outine
    int mapheight = (int)(segment->segState.RegionDim.y * oneTileInPixels);
    al_draw_rectangle(posx, posy, posx+size, posy+mapheight, uiColor(0),0);
    //current segment outline
    int x = (size * (segment->segState.Position.x+1)) / segment->segState.RegionDim.x;
    int y = (mapheight * (segment->segState.Position.y+1)) / segment->segState.RegionDim.y;
    MiniMapSegmentWidth = (segment->segState.Size.x-2) * oneTileInPixels;
    MiniMapSegmentHeight = (segment->segState.Size.y-2) * oneTileInPixels;
    al_draw_rectangle(posx+x, posy+y, posx+x+MiniMapSegmentWidth, posy+y+MiniMapSegmentHeight,uiColor(0),0);
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
        al_draw_line(i,0,i, ssState.ScreenH, uiColor(0), 0);
    }
    for(int i =0; i< ssState.ScreenH; i+=SPRITEHEIGHT) {
        al_draw_line(0,i, 20*SPRITEWIDTH,i,uiColor(0), 0);
    }

    for(int y = 0; y<20; y++) {
        for(int x = 0; x<20; x+=5) {
            int index = y * 20 + x;
            draw_textf_border(font, uiColor(1),  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+(al_get_font_line_height(font)/2), 0, "%i", index);
        }
    }
    draw_textf_border(font, uiColor(1), ssState.ScreenW-10, ssState.ScreenH -al_get_font_line_height(font), ALLEGRO_ALIGN_RIGHT,
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
    CoreSuspender suspend;

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
        draw_textf_border(font, uiColor(1), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "Could not find DF process");
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

    if(ssConfig.show_keybinds){
        string *keyname, *actionname;
        keyname = actionname = NULL;
        int line = 1;
        al_hold_bitmap_drawing(true);
        for(int32_t i=1; true; i++){
            if(getKeyStrings(i, keyname, actionname)){
                draw_textf_border(font, uiColor(1), 10, line*al_get_font_line_height(font), 0, "%s: %s%s", keyname->c_str(), actionname->c_str(), isRepeatable(i) ? " (repeats)" : "");
                line++;
            }
            if(keyname == NULL) {
                break;
            }
        }
        al_hold_bitmap_drawing(false);
    } else if (ssConfig.show_osd) {
        al_hold_bitmap_drawing(true);
        draw_textf_border(font, uiColor(1), 10,al_get_font_line_height(font), 0, "%i,%i,%i, r%i, z%i", ssState.Position.x,ssState.Position.y,ssState.Position.z, ssState.Rotation, ssConfig.zoom);

        drawSelectionCursor(segment);

        drawDebugCursor(segment);

        draw_announcements(font, ssState.ScreenW / 2, ssState.ScreenH - 20, ALLEGRO_ALIGN_CENTRE, df::global::world->status.announcements);
        drawAdvmodeMenuTalk(font, 5, ssState.ScreenH - 5);

        if(ssConfig.debug_mode) {
            draw_textf_border(font, uiColor(1), 10, 3*al_get_font_line_height(font), 0, "Map Read Time: %.2fms", ssTimers.read_time);
            draw_textf_border(font, uiColor(1), 10, 4*al_get_font_line_height(font), 0, "Map Beautification Time: %.2fms", ssTimers.beautify_time);
            draw_textf_border(font, uiColor(1), 10, 5*al_get_font_line_height(font), 0, "Tile Sprite Assembly Time: %.2fms", ssTimers.assembly_time);
            draw_textf_border(font, uiColor(1), 10, 6*al_get_font_line_height(font), 0, "DF Renderer Overlay Time: %.2fms", ssTimers.overlay_time);
            draw_textf_border(font, uiColor(1), 10, 2*al_get_font_line_height(font), 0, "FPS: %.2f", 1000.0/ssTimers.frame_total);
            draw_textf_border(font, uiColor(1), 10, 7*al_get_font_line_height(font), 0, "Draw: %.2fms", ssTimers.draw_time);
            draw_textf_border(font, uiColor(1), 10, 8*al_get_font_line_height(font), 0, "D1: %i", DebugInt1);
            draw_textf_border(font, uiColor(1), 10, 9*al_get_font_line_height(font), 0, "%i/%i/%i, %i:%i", contentLoader->currentDay+1, contentLoader->currentMonth+1, contentLoader->currentYear, contentLoader->currentHour, (contentLoader->currentTickRel*60)/50);

            drawDebugInfo(segment);
        }
        ssConfig.platecount = 0;
        int top = 0;
        if(ssConfig.track_mode != GameConfiguration::TRACKING_NONE) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, uiColor(1), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Locked on DF screen + (%d,%d,%d)",ssConfig.viewXoffset,ssConfig.viewYoffset,ssConfig.viewZoffset);
        }
        if(ssConfig.follow_DFcursor && ssConfig.debug_mode) {
            top += al_get_font_line_height(font);
            if(segment->segState.dfCursor.x != -30000) {
                draw_textf_border(font, uiColor(1), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Following DF Cursor at: %d,%d,%d", segment->segState.dfCursor.x,segment->segState.dfCursor.y,segment->segState.dfCursor.z);
            }
        }
        if(ssConfig.single_layer_view) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, uiColor(1), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Single layer view");
        }
        if(ssConfig.automatic_reload_time) {
            top += al_get_font_line_height(font);
            draw_textf_border(font, uiColor(1), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Reloading every %0.1fs", (float)ssConfig.automatic_reload_time/1000);
        }
        al_hold_bitmap_drawing(false);
        DrawMinimap(segment);
    }
    map_segment.unlockDraw();
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
    if(!load_from_path(p, "Sir_Henry_s_32x32.png", IMGLetterSheet)) {
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
    if(IMGProfSheet) {
        al_destroy_bitmap(IMGProfSheet);
        IMGProfSheet = 0;
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
        uint32_t numFiles = (uint32_t)IMGFilelist.size();
        for(uint32_t i = 0; i < numFiles; i++) {
            if (strcmp(filename, IMGFilenames[i]->c_str()) == 0) {
                return i;
            }
        }
        /*
        al_clear_to_color(uiColor(0));
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
        IMGFilenames.push_back(dts::make_unique<string>(filename));
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
        IMGFilenames.push_back(dts::make_unique<string>(filename));
        LogVerbose("New image: %s\n",filename);
        return (int)IMGFilelist.size() - 1;
    }
}

void saveScreenshot()
{
    al_clear_to_color(ssConfig.backcol);
    paintboard();
    //get filename
    char filename[32] = {0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true) {
        snprintf(filename, sizeof(filename), "screenshot%i.png", index);

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
    char filename[25] = {0};
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

    draw_textf_border(font, uiColor(1), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "saving large screenshot...");
    al_flip_display();
    char filename[32] = {0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true) {
        snprintf(filename, sizeof(filename), "screenshot%i.png", index);
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
    GameConfiguration tempConfig = ssConfig;
    GameState tempState = ssState;
    int tempflags = al_get_new_bitmap_flags();

    //now make them real big.
    ssConfig.show_osd = false;
    ssConfig.track_mode = GameConfiguration::TRACKING_NONE;
    ssConfig.fogenable = false;
    ssConfig.track_screen_center = false;

    //make the image
    ssState.ScreenW = ((ssState.RegionDim.x + ssState.RegionDim.y) * TILEWIDTH / 2)*ssConfig.scale;
    if(tall) {
        ssState.ScreenH = ( ((ssState.RegionDim.x + ssState.RegionDim.y) * TILETOPHEIGHT / 2) + (ssState.RegionDim.z * TILEHEIGHT) )*ssConfig.scale;
    } else {
        ssState.ScreenH = ( ((ssState.RegionDim.x + ssState.RegionDim.y) * TILETOPHEIGHT / 2) + ((ssState.Size.z - 1) * TILEHEIGHT) )*ssConfig.scale;
    }

    bigFile = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);

    //draw and save the image
    if(bigFile) {
        PrintMessage("saving large screenshot to %s\n", filename);
        al_set_target_bitmap(bigFile);
        if(!ssConfig.transparentScreenshots) {
            al_clear_to_color(ssConfig.backcol);
        }

        //zero out the segment lift
        int startlifty, startliftx;
        startlifty = 0;
        //realign the image if the region is rectangular
        if ((ssState.Rotation & 1) == 0)
        {
            startliftx = (TILEWIDTH/2)*ssState.RegionDim.y;
        }
        else
        {
            startliftx = (TILEWIDTH/2)*ssState.RegionDim.x;
        }
        ssConfig.lift_segment_offscreen_y = startlifty;
        ssConfig.lift_segment_offscreen_x = startliftx;

        //here we deal with the rotations
        int startx, incrx, numx;
        int starty, incry, numy;
        int sizex, sizey;
        int numz;

        startx = -1;
        starty = -1;
        sizex = ssState.Size.x-2;
        sizey = ssState.Size.y-2;
        if ((ssState.Rotation & 1) == 0)
        {
            incrx = sizex;
            incry = sizey;
            numx = (int)(ssState.RegionDim.x+3);
            numy = (int)(ssState.RegionDim.y+3);
        }
        else
        {
            incrx = sizey;
            incry = sizex;
            numx = (int)(ssState.RegionDim.y+3);
            numy = (int)(ssState.RegionDim.x+3);
        }
        numx = numx/incrx + (numx%incrx==0 ? 0 : 1);
        numy = numy/incry + (numx%incry==0 ? 0 : 1);
        numz = tall ? ((ssState.RegionDim.z/(ssState.Size.z-1)) + 1) : 1;


        if(ssState.Rotation == 1 || ssState.Rotation == 2) {
            starty = (int)ssState.RegionDim.y - incry - 1;
            ssState.Position.y = (int)ssState.RegionDim.y - incry + 1;
            incry = -incry;
        } else {
            ssState.Position.y = -1;
        }

        if(ssState.Rotation == 3 || ssState.Rotation == 2) {
            startx = (int)ssState.RegionDim.x - incrx - 1;
            ssState.Position.x = (int)ssState.RegionDim.x - incrx + 1;
            incrx = -incrx;
        } else {
            ssState.Position.x = -1;
        }
        if(tall) {
            ssState.Position.z = ssState.RegionDim.z;
        }
        ssState.Position.x = startx;
        ssState.Position.y = starty;
        ssState.Position.z = tall ? 0 : ssState.Position.z;

        //set up the pixel-shifts
        int32_t movexx, moveyx, movexy, moveyy;
        if(ssState.Rotation == 1 || ssState.Rotation == 3) {
            movexx = -sizey;
            moveyx = sizey;

            movexy = -sizex;
            moveyy = sizex;
        } else {
            movexx = sizex;
            moveyx = sizex;

            movexy = sizey;
            moveyy = sizey;
        }

        //now actually loop through and draw the subsegments
        int32_t startstartlifty = startlifty;
        for(int k=0; k<numz; k++) {
            startlifty = startstartlifty - TILEHEIGHT*(numz-k-1)*(ssState.Size.z - 1);
            for(int i=0; i<numy; i++) {
                ssConfig.lift_segment_offscreen_x = startliftx - (TILEWIDTH/2)*i*movexy;
                ssConfig.lift_segment_offscreen_y = startlifty - (TILETOPHEIGHT/2)*i*moveyy;
                for(int j=0; j<numx; j++) {
                    //read and draw each individual segment
                    read_segment(NULL);
                    map_segment.lockDraw();
                    WorldSegment * segment = map_segment.getDraw();
                    segment->DrawAllTiles();
                    map_segment.unlockDraw();

                    ssState.Position.x += incrx;
                    ssConfig.lift_segment_offscreen_x += (TILEWIDTH/2)*movexx;
                    ssConfig.lift_segment_offscreen_y -= (TILETOPHEIGHT/2)*moveyx;
                }
                ssState.Position.x = startx;
                ssState.Position.y += incry;
            }
            ssState.Position.x=startx;
            ssState.Position.y=starty;
            ssState.Position.z += ssState.Size.z - 1;
        }


        al_save_bitmap(filename, bigFile);
        al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
        timer = clock() - timer;
        PrintMessage("\ttime for screenshot %ims\n", timer);
    } else {
        LogError("failed to take large screenshot; try zooming out\n");
    }
    al_destroy_bitmap(bigFile);
    //restore everything that we changed.
    ssConfig = tempConfig;
    ssState = tempState;
    al_set_new_bitmap_flags(tempflags);

    map_segment.unlockRead();
}

ALLEGRO_COLOR morph_color(ALLEGRO_COLOR source, ALLEGRO_COLOR reference, ALLEGRO_COLOR target)
{
    float sH, sS, sL, rH, rS, rL, tH, tS, tL;
    al_color_rgb_to_hsv(source.r, source.g, source.b, &sH, &sS, &sL);
    al_color_rgb_to_hsv(reference.r, reference.g, reference.b, &rH, &rS, &rL);
    al_color_rgb_to_hsv(target.r, target.g, target.b, &tH, &tS, &tL);

    sH += tH - rH;
    sS += tS - rS;
    sL += tL - rL;
    if (sH > 360.0f)
        sH -= 360.0f;
    if (sH < 0.0f)
        sH += 360.0f;
    if (sS > 1.0f)
        sS = 1.0f;
    if (sS < 0.0f)
        sS = 0.0f;
    if (sL > 1.0f)
        sL = 1.0f;
    if (sL < 0.0f)
        sL = 0.0f;

    return al_color_hsv(sH, sS, sL);
}
