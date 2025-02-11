#pragma once

#include <stdint.h>

#include "common.h"
#include "commonTypes.h"

struct GameState{
    //properties of the currently viewed portion of the segment
    df::coord Position;
    int Rotation;

    //the size of the next segment to load, and the map region
    df::coord Size;
    df::coord RegionDim;

    //position of the cursor
    df::coord dfCursor;
    //position of the selection cursor
    df::coord dfSelection;

    //the width and height of the stonesense window
    int ScreenW;
    int ScreenH;
};
