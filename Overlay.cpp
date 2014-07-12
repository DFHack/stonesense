#include "Overlay.h"
#include "TrackingModes.h"
#include "Hooks.h"
#include "GUI.h"

#include "df/init.h"

#include "df/viewscreen_dwarfmodest.h"
#include "df/viewscreen_dungeonmodest.h"


struct renderer_custom : renderer_opengl
{
	virtual void draw(int vertex_count);
};

//Globals used to support OpenGL rendering mode
extern Overlay* overlay;
static bool is_hooked=false;
static draw_ptr old_draw;

DFhackCExport void * SDL_GetVideoSurface(void);
DFhackCExport vPtr SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
										uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
DFhackCExport vPtr SDL_CreateRGBSurfaceFrom(vPtr pixels, int width, int height, int depth, int pitch,
										 uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
DFhackCExport void SDL_FreeSurface(vPtr surface);
DFhackCExport int SDL_UpperBlit(DFHack::DFSDL_Surface* src, DFHack::DFSDL_Rect* srcrect, 
								DFHack::DFSDL_Surface* dst, DFHack::DFSDL_Rect* dstrect);
DFhackCExport uint8_t SDL_GetMouseState(int *x, int *y);

void Overlay::ReadTileLocations()
{
	fontx = df::global::init->font.small_font_dispx;
	fonty = df::global::init->font.small_font_dispy;

	actualWindowSize(width, height);
	
	DFHack::DFSDL_Surface * dfsurf = (DFHack::DFSDL_Surface *) SDL_GetVideoSurface();

	offsetx_tile=1;
	offsety_tile=1;

	if (!df::global::gamemode || *df::global::gamemode == game_mode::ADVENTURE)
	{
		//Adventure mode doesn't have a single-tile border around it.
		offsetx_tile = 0;
		offsety_tile = 0;
	}

	offsetx_pixel = fontx * offsetx_tile + ((dfsurf->w) % fontx)/2;
	offsety_pixel = fonty * offsety_tile + ((dfsurf->h) % fonty)/2;


	ssState.ScreenW = fontx*width; 
	ssState.ScreenH = fonty*height; 
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

bool Overlay::PaintingOverTileAt(int32_t x, int32_t y)
{
	return x >= offsetx_tile && x < offsetx_tile + width 
		&& y >= offsety_tile && y < offsety_tile + height ;
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

		if (parent->uses_opengl()){
			hook();
			glGenTextures(1, &tex_id);
		}

		//parent->zoom(df::zoom_commands::zoom_reset);
		good_viewscreen = false;
		ReadTileLocations();
		copy_from_inner(); 
	}

	front_mutex = al_create_mutex();

	int32_t flags = al_get_new_bitmap_flags();
	if(al_get_current_display() != NULL){
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_ALPHA_TEST);
	}
	front = al_create_bitmap(0,0);
	if(al_get_current_display() != NULL){
		al_set_new_bitmap_flags(al_get_bitmap_flags(al_get_backbuffer(al_get_current_display())));
	}
	back = al_create_bitmap(0,0);
	al_set_new_bitmap_flags(flags);

	Flip();
};

Overlay::~Overlay()
{
	{
		CoreSuspender suspend;

		if (parent->uses_opengl()){
			glDeleteTextures(1,&tex_id);
			unhook();
		}

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
					al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_ALPHA_TEST);
				}
				front = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
				al_set_new_bitmap_flags(flags);
		}
		
		al_set_target_bitmap(front);

		al_draw_bitmap(back, 0, 0, 0);

		front_data = al_lock_bitmap(front, 
			al_get_bitmap_format(front), ALLEGRO_LOCK_READONLY);
	}
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
	//except in GL mode to provide for better twbt compat
	if(!PaintingOverTileAt(x,y)||parent->uses_opengl()){
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
	CheckViewscreen();
	ReadTileLocations();
	if(good_viewscreen){
		tex_width=al_get_bitmap_width(front);
		tex_height=al_get_bitmap_height(front);
		if(front_data != NULL && front_updated){
			//allegro sometimes gives a negative pitch, which SDL doesn't understand, so take care of that case
			int neg = 1;
			int dataoffset = 0;
			if(front_data->pitch < 0){
				neg = -1;
				dataoffset = (tex_height - 1)*front_data->pitch;
			}

			if(uses_opengl())
			{
				glPushAttrib(GL_TEXTURE_BIT);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_TEXTURE_RECTANGLE);
				//glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE, tex_id);
				//glPixelStorei(GL_UNPACK_ROW_LENGTH,neg*front_data->pitch);
				glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, tex_width, tex_height, 0, GL_BGRA,GL_UNSIGNED_BYTE,
					((char*) front_data->data) + dataoffset);
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);
				glDisable(GL_TEXTURE_RECTANGLE);
				glPopAttrib();
			}
			else
			{
				//get the SDL surface information so we can do a blit
				DFHack::DFSDL_Surface * dfsurf = (DFHack::DFSDL_Surface *) SDL_GetVideoSurface();
				DFHack::DFSDL_Surface * sssurf = (DFHack::DFSDL_Surface *) SDL_CreateRGBSurfaceFrom( ((char*) front_data->data) + dataoffset, 
					tex_width, tex_height, 8*front_data->pixel_size, neg*front_data->pitch, 0, 0, 0, 0);

				DFSDL_Rect src;
				src.x = 0;
				src.y = 0;
				src.w = ssState.ScreenW;
				src.h = ssState.ScreenH;

				DFSDL_Rect pos;
				pos.x = offsetx_pixel;
				pos.y = offsety_pixel;
				pos.w = 0;
				pos.h = 0;

				//do the blit
				SDL_UpperBlit(sssurf, &src, dfsurf, &pos);

				SDL_FreeSurface(sssurf);
			}
		}
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
	copy_from_inner();
}

void Overlay::zoom(df::zoom_commands z) 
{ 
	copy_to_inner();
	parent->zoom(df::zoom_commands::zoom_reset);
	//parent->zoom(z);
	copy_from_inner();

	//switch (z) {
	//case df::zoom_commands::zoom_fullscreen:
	//	//???
	//	break;
	//case df::zoom_commands::zoom_in:
	//	ssConfig.zoom++;
	//	ssConfig.scale = pow(2.0f, ssConfig.zoom);
	//	break;
	//case df::zoom_commands::zoom_out:
	//	ssConfig.zoom--;
	//	ssConfig.scale = pow(2.0f, ssConfig.zoom);
	//	break;
	//case df::zoom_commands::zoom_reset:
	//	ssConfig.zoom = 5;
	//	ssConfig.scale = pow(2.0f, ssConfig.zoom);
	//	break;
	//case df::zoom_commands::zoom_resetgrid:
	//	//???
	//	break;
	//}
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
	copy_from_inner();
}

bool Overlay::get_mouse_coords(int32_t* x, int32_t* y) 
{ 
	//PrintMessage("in:(%i,%i)\n",*x,*y);
	bool ret = parent->get_mouse_coords(x,y);
	//PrintMessage("out:(%i,%i)\n",*x,*y); 

	//if we are painting over the tile, then we need to reroute through stonesense
	if(ret && PaintingOverTileAt(*x,*y)){
		int xpx, ypx, xpos, ypos, zpos;
		SDL_GetMouseState(&xpx, &ypx);
		xpx = xpx - offsetx_pixel;
		ypx = ypx - offsety_pixel;
		
		//first figure out which tile in the segment it came from
        ScreenToPoint(xpx,ypx,xpos,ypos,zpos);

		//then remove the segment rotation
		correctForRotation(
			xpos, ypos, 
			(4 - ssState.Rotation) % 4,
			ssState.Size.x, ssState.Size.y);

		//add on the segment offset
		xpos = xpos + ssState.Position.x;
		ypos = ypos + ssState.Position.y;
		//zpos = zpos + ssState.Position.z - 1;

		//remove the offset of the df window
		int dfviewx, dfviewy, dfviewz;
		Gui::getViewCoords(dfviewx, dfviewy, dfviewz);
		xpos = xpos - dfviewx + offsetx_tile;
		ypos = ypos - dfviewy + offsety_tile;
		//zpos = zpos - dfviewz;

		//check to see if this new loaction is within the area we are painting over
		//since we don't want to accidentally click somewhere in the interface
		if(PaintingOverTileAt(xpos,ypos)) {
			*x = xpos;
			*y = ypos;
			return true;
		} else {
			*x = -1;
			*y = -1;
			return false;
		}
	}

	return ret;
}

bool Overlay::uses_opengl() 
{ 
	return parent->uses_opengl();
}

//Some Typedefs and helper functions to assitst with vtable patching:

typedef void *vtable_pointer;
typedef void *m_pointer;

vtable_pointer get_vtable(void* instance_ptr) { return *(void**)instance_ptr; }

m_pointer get_vtable_entry(vtable_pointer vtable_ptr,int idx)
{
	void **vtable = (void**)vtable_ptr;
	if (!vtable) return NULL;
	return vtable[idx];
}

bool set_vtable_entry(MemoryPatcher &patcher,vtable_pointer vtable_ptr, int idx, m_pointer ptr)
{
	assert(idx >= 0);
	void **vtable = (void**)vtable_ptr;
	if (!vtable) return NULL;
	return patcher.write(&vtable[idx], &ptr, sizeof(void*));
}

// Note that the unusual interposing method used here is very deliberate.
// We need to interpose whichever class's draw method is actually used.
// In practice that will alays be some subclass of renderer_opengl,
// but if we just interpose renderers known in DFHack's data files then
// renderers defined in other plugins like the TextWillBeText plugin
// will be missed. We want to support them to, so we patch the correct
// slot of the vtable of whatever the exting renderer really is.

void Overlay::hook()
{
	if(is_hooked) return;

	int draw_index=vmethod_pointer_to_idx(&renderer_opengl::draw);

	//Get and save off a non-virtual pointer to the original draw method
	vtable_pointer oldr_vtable=get_vtable(parent);
	old_draw = addr_to_method_pointer<draw_ptr>(get_vtable_entry(oldr_vtable,draw_index));


	//Get the address of the new draw method
	renderer_custom newr2;
	vtable_pointer newr_vtable=get_vtable(&newr2);
	m_pointer newdraw =get_vtable_entry(newr_vtable,draw_index);

	//Patch existing renderer to use new draw method
	MemoryPatcher patcher;
	set_vtable_entry(patcher,oldr_vtable,draw_index,newdraw);

	is_hooked=true;
}

void Overlay::unhook()
{
	if(!is_hooked) return;

	int draw_index=vmethod_pointer_to_idx(&renderer_opengl::draw);

	//Get and save off a non-virtual pointer to the original draw method
	vtable_pointer oldr_vtable=get_vtable(parent);

	//Patch existing renderer to use new draw method
	MemoryPatcher patcher;
	set_vtable_entry(patcher,oldr_vtable,draw_index,method_pointer_to_addr(old_draw));

	is_hooked=false;
}


void renderer_custom::draw(int vertex_count)
{
	(this->*old_draw)(vertex_count);
	custom_draw();
}


void custom_draw()
{
	if(!overlay) return;
	if(!overlay->good_viewscreen) return;

	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glPushAttrib(GL_TEXTURE_BIT);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE);

	glBindTexture(GL_TEXTURE_RECTANGLE, overlay->tex_id);
	glColor3f(1,1,1);

	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(overlay->offsetx_tile, overlay->offsety_tile);
		glTexCoord2f(0, overlay->tex_height);
		glVertex2f(overlay->offsetx_tile, overlay->offsety_tile+overlay->height);
		glTexCoord2f(overlay->tex_width, overlay->tex_height);
		glVertex2f(overlay->offsetx_tile+overlay->width, overlay->offsety_tile+overlay->height);
		glTexCoord2f(overlay->tex_width,0);
		glVertex2f(overlay->offsetx_tile+overlay->width, overlay->offsety_tile);
	glEnd();

	glDisable(GL_TEXTURE_RECTANGLE);
	glPopAttrib();
	glPopClientAttrib();
}
