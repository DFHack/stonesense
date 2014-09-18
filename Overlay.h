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
	//the pixel-offset of the upper left corner of first tile of the main view
	int32_t offsetx_pixel, offsety_pixel;
	//the tile offset of the firtst tile of the main view
	int32_t offsetx_tile, offsety_tile;

	//the width and height in tiles of the current df display's view
	int32_t width, height;

	//Texture id ("name") for OpenGL mode
	GLuint tex_id;

	//Width and height of texure in texels
	int32_t tex_width, tex_height;

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

	//Install and remove hooks for OpenGL mode
	void hook();
	void unhook();

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

	friend void custom_draw();
};

// from g_src/renderer_opengl.hpp
struct renderer_opengl : df::renderer
{
	void *screen;
	int dispx, dispy;

	//bool init_video(int w, int h);

	GLfloat *vertexes, *fg, *bg, *tex;

	//void write_tile_vertexes(GLfloat x, GLfloat y, GLfloat *vertex);
	virtual void allocate(int tiles) {};
	virtual void init_opengl() {};
	virtual void uninit_opengl() {};
	virtual void draw(int vertex_count) {};
	//void write_tile_arrays(int x, int y, GLfloat *fg, GLfloat *bg, GLfloat *tex);

	//void update_tile(int x, int y);
	//void update_all();
	//void render();
	//renderer_opengl();
	virtual ~renderer_opengl() {};

	int zoom_steps, forced_steps;
	int natural_w, natural_h;

	//void zoom(zoom_commands cmd);
	//void compute_forced_zoom();
	//pair<int,int> compute_zoom(bool clamp = false);
	//void reshape(pair<int,int> size);

	int off_x, off_y, size_x, size_y;

	//bool get_mouse_coords(int &x, int &y);
	virtual void reshape_gl() {};
	//void resize(int w, int h);
	//void grid_resize(int w, int h);
	//void set_fullscreen();

	bool needs_reshape;
	int needs_zoom;

};

typedef void (renderer_opengl::*draw_ptr)(int vertex_count);
