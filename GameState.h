#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"

struct GameState{
    //properties of the currently viewed portion of the segment
    Crd3D Position;
    int Rotation;

    //the size of the next segment to load, and the map region
    Crd3D Size;
    Crd3D RegionDim;

    //position of the cursor
    OptCrd3D dfCursor;
    //position of the selection cursor
    OptCrd3D dfSelection;

    //the width and height of the stonesense window
    int ScreenW;
    int ScreenH;

    //info panel
    int InfoW;
    int InfoH;
    int selectedTab = -1;

    enum tabs {
        announcements,
        keybinds,
        settings
    };

};
