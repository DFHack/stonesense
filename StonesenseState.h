#pragma once

#include "commonTypes.h"
#include "GameConfiguration.h"
#include "GameState.h"
#include "WorldSegment.h"

#include "allegro5/allegro_font.h"
#include "allegro5/mouse.h"
#include "allegro5/keyboard.h"
#include "allegro5/timer.h"


class ContentLoader;

constexpr auto RANDOM_CUBE = 16;

class MapState
{
public:

};

class StonesenseState
{
public:
    FrameTimers stoneSenseTimers;
    bool timeToReloadSegment;
    bool timeToReloadConfig;
    char currentAnimationFrame;
    uint32_t currentFrameLong;
    bool animationFrameShown;
    ALLEGRO_FONT* font;
    ALLEGRO_KEYBOARD_STATE keyboard;
    ALLEGRO_TIMER* reloadtimer;
    ALLEGRO_TIMER* animationtimer;
    ALLEGRO_MOUSE_STATE mouse;
    int randomCube[RANDOM_CUBE][RANDOM_CUBE][RANDOM_CUBE];

    enum UIState {
        DEFAULT,
        OSD,
        DEBUG,
        INFO_PANEL,
        INFO_PANEL_ANNOUNCEMENTS,
        INFO_PANEL_KEYBINDS,
        INFO_PANEL_SETTING,
        COUNT
    };

    UIState currentUIState;


    std::unique_ptr<MapState> currentMap;
    std::unique_ptr<ContentLoader> contentLoader;

    GameConfiguration ssConfig;
    GameState ssState;

    int MiniMapTopLeftX;
    int MiniMapTopLeftY;
    int MiniMapBottomRightX;
    int MiniMapBottomRightY;
    int MiniMapSegmentWidth;
    int MiniMapSegmentHeight;
    double oneTileInPixels;

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

    int mouse_x;
    int mouse_y;
    int mouse_z;
    unsigned int mouse_b;
    bool mouseHeld;

    SegmentWrap map_segment;

    int lift_segment_offscreen_x;
    int lift_segment_offscreen_y;
};

extern StonesenseState stonesenseState;
