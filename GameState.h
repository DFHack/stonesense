#pragma once

#include <stdint.h>
#include <string>

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
    Crd3D dfCursor;
    //position of the selection cursor
    Crd3D dfSelection;

    //the width and height of the stonesense window
    int ScreenW;
    int ScreenH;

    bool blueprinting = false;
    bool rectangleSelect = true;

    enum modeTypes {
        DEFAULT,
        DIG,
        CHOP,
        GATHER,
        SMOOTH,
        ERASE,
        BUILDING,
        TRAFFIC,
    };

    //the current mode we're in
    enum modeTypes mode = DEFAULT;
    std::string submode = "None";
};
