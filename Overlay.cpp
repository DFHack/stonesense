#include "Overlay.h"
#include "TrackingModes.h"
#include "Hooks.h"

#include "df/init.h"

#include "df/viewscreen_dwarfmodest.h"
#include "df/viewscreen_dungeonmodest.h"

//no idea what I'm doing!
DFhackCExport void * SDL_GetVideoSurface(void);
DFhackCExport vPtr SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
										uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
DFhackCExport vPtr SDL_CreateRGBSurfaceFrom(vPtr pixels, int width, int height, int depth, int pitch,
                                         uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
DFhackCExport void SDL_FreeSurface(vPtr surface);
DFhackCExport int SDL_UpperBlit(DFHack::DFSDL_Surface* src, DFHack::DFSDL_Rect* srcrect, 
								DFHack::DFSDL_Surface* dst, DFHack::DFSDL_Rect* dstrect);

void Overlay::ReadTileLocations()
{
	fontx = df::global::init->font.small_font_dispx;
	fonty = df::global::init->font.small_font_dispy;

	actualWindowSize(width, height);
	
	DFHack::DFSDL_Surface * dfsurf = (DFHack::DFSDL_Surface *) SDL_GetVideoSurface();
	offsetx = ((dfsurf->w) % fontx)/2;
	offsety = ((dfsurf->h) % fonty)/2;
}

void Overlay::CheckViewscreen()
{
	df::viewscreen * vs = Gui::getCurViewscreen();
	virtual_identity * id = virtual_identity::get(vs);
	if(id == &df::viewscreen_dwarfmodest::_identity
		|| id == &df::viewscreen_dungeonmodest::_identity){
			good_viewscreen = true;
	} else {
		good_viewscreen = false;
	}
}

void Overlay::set_to_null() 
{
	screen = NULL;
	screentexpos = NULL;
	screentexpos_addcolor = NULL;
	screentexpos_grayscale = NULL;
	screentexpos_cf = NULL;
	screentexpos_cbr = NULL;
	screen_old = NULL;
	screentexpos_old = NULL;
	screentexpos_addcolor_old = NULL;
	screentexpos_grayscale_old = NULL;
	screentexpos_cf_old = NULL;
	screentexpos_cbr_old = NULL;
}

void Overlay::copy_from_inner() 
{
	screen = parent->screen;
	screentexpos = parent->screentexpos;
	screentexpos_addcolor = parent->screentexpos_addcolor;
	screentexpos_grayscale = parent->screentexpos_grayscale;
	screentexpos_cf = parent->screentexpos_cf;
	screentexpos_cbr = parent->screentexpos_cbr;
	screen_old = parent->screen_old;
	screentexpos_old = parent->screentexpos_old;
	screentexpos_addcolor_old = parent->screentexpos_addcolor_old;
	screentexpos_grayscale_old = parent->screentexpos_grayscale_old;
	screentexpos_cf_old = parent->screentexpos_cf_old;
	screentexpos_cbr_old = parent->screentexpos_cbr_old;
}

void Overlay::copy_to_inner() 
{
	parent->screen = screen;
	parent->screentexpos = screentexpos;
	parent->screentexpos_addcolor = screentexpos_addcolor;
	parent->screentexpos_grayscale = screentexpos_grayscale;
	parent->screentexpos_cf = screentexpos_cf;
	parent->screentexpos_cbr = screentexpos_cbr;
	parent->screen_old = screen_old;
	parent->screentexpos_old = screentexpos_old;
	parent->screentexpos_addcolor_old = screentexpos_addcolor_old;
	parent->screentexpos_grayscale_old = screentexpos_grayscale_old;
	parent->screentexpos_cf_old = screentexpos_cf_old;
	parent->screentexpos_cbr_old = screentexpos_cbr_old;
}

Overlay::Overlay(renderer* parent) : parent(parent)
{
	{
		CoreSuspender suspend;
		CheckViewscreen();
		ReadTileLocations();
		copy_from_inner(); 
	}

	front_mutex = al_create_mutex();

	front = al_create_bitmap(0, 0);
	back = al_create_bitmap(0, 0);

	Flip();
};

Overlay::~Overlay()
{
	{
		CoreSuspender suspend;
		df::global::enabler->renderer = parent;
	}

	al_destroy_mutex(front_mutex);

	al_destroy_bitmap(front);
	al_destroy_bitmap(back);
	front = back = NULL;

	if(al_get_current_display() == NULL){
		al_set_target_bitmap(NULL);
	} else {
		al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
	}
}

void Overlay::Flip()
{
    //do the starting timer stuff
    clock_t starttime = clock();

	al_lock_mutex(front_mutex);
	{
		al_unlock_bitmap(front);

		if(al_get_bitmap_width(front) != ssState.ScreenW
			|| al_get_bitmap_height(front) != ssState.ScreenH){
				al_destroy_bitmap(front);
				int32_t flags = al_get_new_bitmap_flags();
				if(al_get_current_display() != NULL){
					al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_ALPHA_TEST 
						| al_get_bitmap_flags(al_get_backbuffer(al_get_current_display())));
				}
				front = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
				al_set_new_bitmap_flags(flags);
		}
		
		al_set_target_bitmap(front);

		al_draw_bitmap(back, 0, 0, 0);

		front_data = al_lock_bitmap(front, 
			al_get_bitmap_format(front), ALLEGRO_LOCK_READONLY);
	}
	ssState.ScreenW = fontx*width; 
	ssState.ScreenH = fonty*height; 
	front_updated = true;
	al_unlock_mutex(front_mutex);

	if(al_get_bitmap_width(back) != ssState.ScreenW
		|| al_get_bitmap_height(back) != ssState.ScreenH){
			al_destroy_bitmap(back);
			int32_t flags = al_get_new_bitmap_flags();
			if(al_get_current_display() != NULL){
				al_set_new_bitmap_flags(al_get_bitmap_flags(al_get_backbuffer(al_get_current_display())));
			}
			back = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
			al_set_new_bitmap_flags(flags);
	}

	al_set_target_bitmap(back);

    //do the ending timer stuff
    clock_t donetime = clock();
	ssTimers.overlay_time = (donetime - starttime)*0.1 + ssTimers.overlay_time*0.9;
}

bool Overlay::GoodViewscreen()
{
	return good_viewscreen;
}

void Overlay::update_tile(int32_t x, int32_t y) 
{ 
	//don't update tiles we are painting overtop of
	if(x == 0 || x > width || y == 0 || y > height){
		copy_to_inner();
		parent->update_tile(x,y);
	}
}

void Overlay::update_all() 
{ 
	copy_to_inner();
	parent->update_all();
	front_updated = true;
}

void Overlay::render() 
{ 
	copy_to_inner();

	al_lock_mutex(front_mutex);
	if(good_viewscreen){
		if(front_data != NULL && front_updated){
			//allegro sometimes gives a negative pitch, which SDL doesn't understand, so take care of that case
			int neg = 1;
			int dataoffset = 0;
			if(front_data->pitch < 0){
				neg = -1;
				dataoffset = (al_get_bitmap_height(front) - 1)*front_data->pitch;
			}

			//get the SDL surface information so we can do a blit
			DFHack::DFSDL_Surface * dfsurf = (DFHack::DFSDL_Surface *) SDL_GetVideoSurface();
			DFHack::DFSDL_Surface * sssurf = (DFHack::DFSDL_Surface *) SDL_CreateRGBSurfaceFrom( ((char*) front_data->data) + dataoffset, 
				al_get_bitmap_width(front), al_get_bitmap_height(front), 8*front_data->pixel_size, neg*front_data->pitch, 0, 0, 0, 0);

			DFSDL_Rect pos;
			pos.x = fontx + offsetx;
			pos.y = fonty + offsety;
			pos.w = dfsurf->w;
			pos.h = dfsurf->h;

			//do the blit
			SDL_UpperBlit(sssurf, NULL, dfsurf, &pos);

			SDL_FreeSurface(sssurf);
		}
		CheckViewscreen();
		ReadTileLocations();
		front_updated = false;
	} else {
		width = 0;
		height = 0;
	}
	al_unlock_mutex(front_mutex);

	parent->render();
}

void Overlay::set_fullscreen() 
{ 
	copy_to_inner();
	parent->set_fullscreen();
}

void Overlay::zoom(df::zoom_commands z) 
{ 
	copy_to_inner();
	parent->zoom(z);
}

void Overlay::resize(int32_t w, int32_t h) 
{ 
	copy_to_inner();
	parent->resize(w,h);
	copy_from_inner();
}

void Overlay::grid_resize(int32_t w, int32_t h) 
{ 
	copy_to_inner();
	parent->grid_resize(w,h);
	parent->update_all();
	copy_from_inner();
	front_updated = true;
}

bool Overlay::get_mouse_coords(int32_t* x, int32_t* y) 
{ 
	return parent->get_mouse_coords(x,y);
}

bool Overlay::uses_opengl() 
{ 
	return parent->uses_opengl();
}

void Overlay::invalidateRect(int32_t x,int32_t y,int32_t w,int32_t h)
{
	for(int i=x;i<x+w;i++)
		for(int j=y;j<y+h;j++)
		{
			int index=i*df::global::gps->dimy + j;
			screen_old[index*4]=screen[index*4]+1;
		}
}

void Overlay::invalidate()
{
	invalidateRect(0,0,df::global::gps->dimx,df::global::gps->dimy);
}
