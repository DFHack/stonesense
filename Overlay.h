#pragma once

#include "Core.h"
#include <VTableInterpose.h>
#include "df/renderer.h"
#include "df/enabler.h"
#include "df/graphic.h"

#include "common.h"

class Overlay : public df::renderer
{
private:
    //size of the current df display's font
    uint8_t fontx, fonty;
    //the pixel-offset of the first DF tile in the top-left corner of the screen
    int32_t offsetx, offsety;
    //the width and height in tiles of the current df display's view
    int32_t width, height;

    //used by the renderer to skip drawing from the overlay buffer if it wasn't updated
    bool front_updated;
    //keeps track of if the most recently visited viewscreen is one it makes sense to overlay
    bool good_viewscreen;

    //need to lock front buffer because SDL and Allegro can't talk to negotiate ownership
    ALLEGRO_MUTEX * front_mutex;

    //our front and back buffers
    ALLEGRO_BITMAP * front;
    ALLEGRO_BITMAP * back;

    ALLEGRO_LOCKED_REGION * front_data;

    renderer * parent;

    //reads tile information, used for resizing and positioning of the overlay
    void ReadTileLocations();
    //updates good_viewscreen
    void CheckViewscreen();
    //determines if the given tile location is within the area we're painting over
    bool PaintingOverTileAt(int32_t, int32_t);

    //df::renderer overrides
    void set_to_null();
    void copy_from_inner();
    void copy_to_inner();

public:
    Overlay(renderer* parent);
    ~Overlay();

    void Flip();

    bool GoodViewscreen();

    //df::renderer overrides
    void update_tile(int32_t x, int32_t y);
    void update_all();
    void render();
    void set_fullscreen();
    void zoom(df::zoom_commands z);
    void resize(int32_t w, int32_t h);
    void grid_resize(int32_t w, int32_t h);
    bool get_mouse_coords(int32_t* x, int32_t* y);
    bool uses_opengl();
};