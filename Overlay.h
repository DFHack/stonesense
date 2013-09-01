#pragma once

#include "Core.h"
#include <VTableInterpose.h>
#include "df/renderer.h"
#include "df/enabler.h"
#include "df/graphic.h"

//#include "df/viewscreen_dwarfmodest.h"
//#include "df/viewscreen_dungeonmodest.h"

#include "common.h"

class Overlay : public df::renderer
{
private:


	//not really needed, but good for sanity
	uint8_t fontx, fonty;

	//need to lock front buffer because SDL and Allegro can't talk to negotiate ownership
    ALLEGRO_MUTEX * front_mutex;

	//our front and back buffers
	ALLEGRO_BITMAP * front;
	ALLEGRO_BITMAP * back;

	ALLEGRO_LOCKED_REGION * front_data;

	renderer * parent;

	//df::renderer overrides
	void set_to_null();
	void copy_from_inner();
	void copy_to_inner();

public:
	Overlay(renderer* parent);
	~Overlay();

	void Flip();

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
	void invalidateRect(int32_t x,int32_t y,int32_t w,int32_t h);
	void invalidate();
};

//struct dwarmode_render_hook : df::viewscreen_dwarfmodest{
//	typedef df::viewscreen_dwarfmodest interpose_base;
//	DEFINE_VMETHOD_INTERPOSE(void,render,())
//	{
//		CoreSuspendClaimer suspend;
//		INTERPOSE_NEXT(render)();
//		//???
//	}
//};
//IMPLEMENT_VMETHOD_INTERPOSE(dwarmode_render_hook, render);
//
//struct dungeon_render_hook : df::viewscreen_dungeonmodest{
//	typedef df::viewscreen_dungeonmodest interpose_base;
//	DEFINE_VMETHOD_INTERPOSE(void,render,())
//	{
//		CoreSuspendClaimer suspend;
//		INTERPOSE_NEXT(render)();
//		//???
//	}
//};
//IMPLEMENT_VMETHOD_INTERPOSE(dungeon_render_hook, render);