#include "common.h"
#include "TrackingModes.h"
#include "MapLoading.h"
#include "GameConfiguration.h"
#include "GameState.h"

void followCurrentDFCenter()
{
    int32_t newviewx;
    int32_t newviewy;
    int32_t newviewz;
    auto view_dims = DFHack::Gui::getDwarfmodeViewDims();
    DFHack::Gui::getViewCoords(newviewx,newviewy,newviewz);
    int32_t viewsizex = view_dims.map_x2 - view_dims.map_x1 + 1;
    int32_t viewsizey = view_dims.map_y2 - view_dims.map_y1 + 1;
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
