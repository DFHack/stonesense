#include "common.h"
#include "TrackingModes.h"
#include "MapLoading.h"

void actualWindowSize(int32_t & width, int32_t & height)
{
    uint8_t mnu, map;
    Gui::getMenuWidth(mnu, map);
    Gui::getWindowSize(width, height);
    if (!df::global::gamemode || *df::global::gamemode == game_mode::ADVENTURE)
    {
        height = height - 2; //Adventure mode has a 2 tile high status screen on the bottom.
        //width = width; //But it uses the full screen width.
    }
    else
    {
        height = height - 2; //account for vertical borders

        if (mnu == 1){
            width -= 57; //Menu is open doubly wide
        }
        else if (mnu == 2 && map == 3) {
            width -= 33; //Just the menu is open
        }
        else if (mnu == 2 && map == 2) {
            width -= 26; //Just the area map is open
        }
        else {
            width = width - 2; //No menu or area map, just account for borders
        }
    }
}

void followCurrentDFWindow()
{
    int32_t newviewx;
    int32_t newviewy;
    int32_t viewsizex;
    int32_t viewsizey;
    int32_t newviewz;
    int32_t mapx, mapy, mapz;
    // we take the rectangle you'd get if you scrolled the DF view closely around
    // map edges with a pen pierced through the center,
    // compute the scaling factor between this rectangle and the map bounds and then scale
    // the coords with this scaling factor
    /**
    +---+
    |W+-++----------+
    +-+-+---------+ |
    | |         | |
    | | inner   | |
    | |   rect. | |
    | |         | |
    | |         | |--- map boundary
    | +---------+ |
    +-------------+  W - corrected view
    */
    Maps::getSize((uint32_t &)mapx, (uint32_t &)mapy, (uint32_t &)mapz);
    mapx *= 16;
    mapy *= 16;

    actualWindowSize(viewsizex,viewsizey);
    float scalex = float (mapx) / float (mapx - viewsizex);
    float scaley = float (mapy) / float (mapy - viewsizey);

    Gui::getViewCoords(newviewx,newviewy,newviewz);
    newviewx = newviewx + (viewsizex / 2) - mapx / 2;
    newviewy = newviewy + (viewsizey / 2) - mapy / 2;

    ssState.Position.x = float (newviewx) * scalex - (ssState.Size.x / 2) + ssConfig.viewXoffset + mapx / 2;
    ssState.Position.y = float (newviewy) * scaley - (ssState.Size.y / 2) + ssConfig.viewYoffset + mapy / 2;
    ssState.Position.z = newviewz + ssConfig.viewZoffset + 1;
}

void followCurrentDFCenter()
{
    int32_t newviewx;
    int32_t newviewy;
    int32_t viewsizex;
    int32_t viewsizey;
    int32_t newviewz;
    actualWindowSize(viewsizex,viewsizey);
    Gui::getViewCoords(newviewx,newviewy,newviewz);
    ssState.Position.x = newviewx + (viewsizex/2) - (ssState.Size.x / 2) + ssConfig.viewXoffset;
    ssState.Position.y = newviewy + (viewsizey/2) - (ssState.Size.y / 2) + ssConfig.viewYoffset;
    ssState.Position.z = newviewz + ssConfig.viewZoffset + 1;
}

//eventually, this should be a sort of "smart-follow" which switches modes intelligently
void followCurrentDFFocus()
{
    if(ssState.dfCursor.x != -30000) {
        ssState.Position.x = ssState.dfCursor.x - (ssState.Size.x / 2) + ssConfig.viewXoffset;
        ssState.Position.y = ssState.dfCursor.y - (ssState.Size.y / 2) + ssConfig.viewYoffset;
        ssState.Position.z = ssState.dfCursor.z + ssConfig.viewZoffset + 1;
    } else {
        followCurrentDFCenter();
    }
}

