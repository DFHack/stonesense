#include "common.h"
#include "TrackingModes.h"
#include "MapLoading.h"
#include "GameConfiguration.h"
#include "GameState.h"
#include "StonesenseState.h"

void followCurrentDFCenter()
{
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    int32_t newviewx;
    int32_t newviewy;
    int32_t newviewz;
    auto view_dims = DFHack::Gui::getDwarfmodeViewDims();
    DFHack::Gui::getViewCoords(newviewx,newviewy,newviewz);
    int32_t viewsizex = view_dims.map_x2 - view_dims.map_x1 + 1;
    int32_t viewsizey = view_dims.map_y2 - view_dims.map_y1 + 1;
    ssState.Position.x = newviewx + (viewsizex/2) - (ssState.Size.x / 2) + ssConfig.config.viewOffset.x;
    ssState.Position.y = newviewy + (viewsizey/2) - (ssState.Size.y / 2) + ssConfig.config.viewOffset.y;
    ssState.Position.z = newviewz + ssConfig.config.viewOffset.z + 1;
}

//eventually, this should be a sort of "smart-follow" which switches modes intelligently
void followCurrentDFFocus()
{
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    if(ssState.dfCursor) {
        ssState.Position.x = ssState.dfCursor->x - (ssState.Size.x / 2) + ssConfig.config.viewOffset.x;
        ssState.Position.y = ssState.dfCursor->y - (ssState.Size.y / 2) + ssConfig.config.viewOffset.y;
        ssState.Position.z = ssState.dfCursor->z + ssConfig.config.viewOffset.z + 1;
    } else {
        followCurrentDFCenter();
    }
}
