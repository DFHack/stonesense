#include <assert.h>
#include <vector>


using namespace std;

#include "common.h"
#include "Block.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"
#include "MapLoading.h"
#include "WorldSegment.h"
#include "Creatures.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"

#ifdef LINUX_BUILD
#include "stonesense.xpm"
extern void *allegro_icon;
#endif


uint32_t ClockedTime = 0;
uint32_t ClockedTime2 = 0;
uint32_t DebugInt1;

int keyoffset=0;

GameConfiguration config;
bool timeToReloadSegment;
bool timeToReloadConfig;
char currentAnimationFrame;
bool animationFrameShown;

vector<t_matgloss> v_stonetypes;

ALLEGRO_FONT * font;

ALLEGRO_DISPLAY * display;

ALLEGRO_KEYBOARD_STATE keyboard;

ALLEGRO_TIMER * reloadtimer;
ALLEGRO_TIMER * animationtimer;

ALLEGRO_EVENT_QUEUE *queue;

ALLEGRO_EVENT event;

ALLEGRO_BITMAP* IMGIcon;

int mouse_x, mouse_y, mouse_z;
unsigned int mouse_b;
bool key[ALLEGRO_KEY_MAX];

/*int32_t viewx = 0;
int32_t viewy = 0;
int32_t viewz = 0;
bool followmode = true;*/
volatile int close_button_pressed = false;

void WriteErr(char* msg, ...){
	va_list arglist;
	va_start(arglist, msg);
	//  char buf[200] = {0};
	//  vsprintf(buf, msg, arglist);
	FILE* fp = fopen( "Stonesense.log", "a");
	if(fp)
		vfprintf( fp, msg, arglist );
	va_end(arglist);
	fclose(fp);
}

void LogVerbose(char* msg, ...){
	if (!config.verbose_logging)
		return;
	va_list arglist;
	va_start(arglist, msg);
	//  char buf[200] = {0};
	//  vsprintf(buf, msg, arglist);
	FILE* fp = fopen( "Stonesense.log", "a");
	if(fp)
		vfprintf( fp, msg, arglist );
	va_end(arglist);
	fclose(fp);
}
void DisplayErr(const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list ap;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(ap, format);
		s = va_arg(ap, const char *);
		al_show_native_message_box("Error", "ERROR", s, NULL, ALLEGRO_MESSAGEBOX_ERROR);
		va_end(ap);
		return;
	}

	va_start(ap, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, ap);
	va_end(ap);

	al_show_native_message_box("Error", "ERROR", al_cstr(buf), NULL, ALLEGRO_MESSAGEBOX_ERROR);

	al_ustr_free(buf);
}

void SetTitle(const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list ap;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(ap, format);
		s = va_arg(ap, const char *);
		al_set_window_title(s);
		va_end(ap);
		return;
	}

	va_start(ap, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, ap);
	va_end(ap);

	al_set_window_title(al_cstr(buf));

	al_ustr_free(buf);
}

void correctBlockForSegmetOffset(int32_t& x, int32_t& y, int32_t& z){
	x -= viewedSegment->x;
	y -= viewedSegment->y; //DisplayedSegmentY;
	z -= viewedSegment->z + viewedSegment->sizez - 2; // loading one above the top of the displayed segment for block rules
}

void loadfont()
{
	font = al_load_font(al_path_cstr(config.font, ALLEGRO_NATIVE_PATH_SEP), config.fontsize, 0);
	if (!font) {
		DisplayErr("Cannot load font: %s", al_path_cstr(config.font, ALLEGRO_NATIVE_PATH_SEP));
		exit(1);
	}
}

void benchmark(){
	DisplayedSegmentX = DisplayedSegmentY = 0;
	DisplayedSegmentX = 110; DisplayedSegmentY = 110;DisplayedSegmentZ = 18;
	uint32_t startTime = clock();
	int i = 20;
	while(i--)
		reloadDisplayedSegment();

	FILE* fp = fopen("benchmark.txt", "w" );
	if(!fp) return;
	fprintf( fp, "%lims", clock() - startTime);
	fclose(fp);
}

void animUpdateProc()
{
	if (animationFrameShown)
	{
		// check before setting, or threadsafety will be borked
		if (currentAnimationFrame >= (MAX_ANIMFRAME-1)) // ie ends up [0 .. MAX_ANIMFRAME)
			currentAnimationFrame = 0;
		else
			currentAnimationFrame = currentAnimationFrame + 1;
		animationFrameShown = false;
	}
}

void drawcredits()
{
	al_clear_to_color(al_map_rgb(0,0,0));
	//centred splash image
	{
		ALLEGRO_BITMAP* SplashImage = load_bitmap_withWarning("splash.png");
		al_draw_bitmap_region(SplashImage, 0, 0,
			al_get_bitmap_width(SplashImage), al_get_bitmap_height(SplashImage),
			(al_get_bitmap_width(al_get_backbuffer()) - al_get_bitmap_width(SplashImage))/2,
			(al_get_bitmap_height(al_get_backbuffer()) - al_get_bitmap_height(SplashImage))/2, 0);
		al_destroy_bitmap(SplashImage);
	}
	int op, src, dst, alpha_op, alpha_src, alpha_dst;
	ALLEGRO_COLOR color;
	al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst, &color);
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, al_map_rgb(255, 255, 0));
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, 5*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Welcome to Stonesense Slate!");
	al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst, color);
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, 6*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Stonesense is an isometric viewer for Dwarf Fortress.");

	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, 8*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Programming, Jonas Ask, Kris Parker and Japa Illo");
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, 9*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Lead graphics designer, Dale Holdampf");

	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-13*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Contributors:");
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-12*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "7c Nickel, Belal, DeKaFu, Dante, Deon, dyze, Errol, fifth angel,");
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-11*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "frumpton, IDreamOfGiniCoeff, Impaler, jarathor, ");
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-10*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Jiri Petru, Lord Nightmare, McMe, Mike Mayday, Nexii ");
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-9*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Malthus, peterix, Seuss, Talvara, winner, and Xandrin.");

	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-7*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "With special thanks to peterix for making dfHack");

	//"The program is in a very early alpha, we're only showcasing it to get ideas and feedback, so use it at your own risk."
	al_draw_text(font, al_get_bitmap_width(al_get_backbuffer())/2, al_get_bitmap_height(al_get_backbuffer())-4*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Press F9 to continue");
	// Make the backbuffer visible
	al_flip_display();
}


int main(void)
{
	/*
	#ifdef LINUX_BUILD
	allegro_icon = stonesense_xpm;
	#endif
	*/
	al_init();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_mouse();
	al_show_mouse_cursor();
	al_set_system_mouse_cursor(ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
	WriteErr("\nStonesense launched\n");

	config.debug_mode = false;
	config.hide_outer_blocks = false;
	config.shade_hidden_blocks = true;
	config.load_ground_materials = true;
	config.automatic_reload_time = 0;
	config.automatic_reload_step = 500;
	config.lift_segment_offscreen = 0;
	config.Fullscreen = FULLSCREEN;
	config.screenHeight = RESOLUTION_HEIGHT;
	config.screenWidth = RESOLUTION_WIDTH;
	config.segmentSize.x = DEFAULT_SEGMENTSIZE;
	config.segmentSize.y = DEFAULT_SEGMENTSIZE;
	config.segmentSize.z = DEFAULT_SEGMENTSIZE_Z;
	config.show_creature_names = true;
	config.show_osd = true;
	config.show_intro = true;
	config.track_center = false;
	config.animation_step = 300;
	config.follow_DFscreen = false;
	timeToReloadConfig = true;
	config.fogr = 255;
	config.fogg = 255;
	config.fogb = 255;
	config.foga = 255;
	config.backr = 95;
	config.backg = 95;
	config.backb = 160;
	config.fogenable = true;
	config.imageCacheSize = 4096;
	config.fontsize = 10;
	config.font = al_create_path("DejaVuSans.ttf");
	config.creditScreen = true;
	initRandomCube();
	loadConfigFile();
	loadfont();
	//set debug cursor
	debugCursor.x = config.segmentSize.x / 2;
	debugCursor.y = config.segmentSize.y / 2;


	int gfxMode = config.Fullscreen ? ALLEGRO_FULLSCREEN : ALLEGRO_WINDOWED;
	al_set_new_display_flags(gfxMode|ALLEGRO_RESIZABLE|(config.opengl ? ALLEGRO_OPENGL : 0)|(config.directX ? ALLEGRO_DIRECT3D_INTERNAL : 0));
	display = al_create_display(config.screenWidth, config.screenHeight);
	if(!display)
	{
		WriteErr("Could not init display\n");
		exit(1);
		return 1;
	}
	if (!al_install_keyboard()) {
		al_show_native_message_box("Error", "Error", "al_install_keyboard failed.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		exit(1);
		return 1;
	}
	SetTitle("Stonesense");

	if(config.software)
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP|ALLEGRO_ALPHA_TEST);

	IMGIcon = load_bitmap_withWarning("stonesense.png");
	al_set_display_icon(IMGIcon);

	//al_set_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, al_map_rgba_f(1.0, 1.0, 1.0, 1.0));
	al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE, al_map_rgba_f(1, 1, 1, 1));
	loadGraphicsFromDisk();
	al_clear_to_color(al_map_rgb(0,0,0));
	draw_textf_border(font, al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Starting up...");
	al_flip_display();
	reloadtimer = al_install_timer(ALLEGRO_MSECS_TO_SECS(config.automatic_reload_time));
	animationtimer = al_install_timer(ALLEGRO_MSECS_TO_SECS(config.animation_step));
	if(config.animation_step)
	{
	al_start_timer(animationtimer);
	}
	// Start the event queue to handle keyboard input
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_timer_event_source(reloadtimer));
	al_register_event_source(queue, al_get_timer_event_source(animationtimer));
	bool redraw = true;

	//upper left corners
	DisplayedSegmentX = DisplayedSegmentY = DisplayedSegmentZ = 0;

	//ramps
	//DisplayedSegmentX = 238; DisplayedSegmentY = 220;DisplayedSegmentZ = 23;

	//ford. Main hall
	DisplayedSegmentX = 172; DisplayedSegmentY = 195;DisplayedSegmentZ = 15;

	//ford. desert map
	//sDisplayedSegmentX = 78; DisplayedSegmentY = 123;DisplayedSegmentZ = 15;

	DisplayedSegmentX = 125; DisplayedSegmentY = 125;DisplayedSegmentZ = 18;

	//DisplayedSegmentX = 242; DisplayedSegmentY = 345;DisplayedSegmentZ = 15;


#ifdef BENCHMARK
	benchmark();
#endif
	//install_int( animUpdateProc, config.animation_step );
	initAutoReload();

	timeToReloadSegment = true;

	while (true) {
		if (redraw && al_event_queue_is_empty(queue))
		{
			al_rest(ALLEGRO_MSECS_TO_SECS(30));
			if(config.spriteIndexOverlay)
			{
				DrawSpriteIndexOverlay(config.currentSpriteOverlay);
			}
			else if( config.show_intro && config.creditScreen )
			{
				drawcredits();
			}
			else if( timeToReloadSegment ){
				reloadDisplayedSegment();
				paintboard();
				timeToReloadSegment = false;
				animationFrameShown = true;
			}
			else if (animationFrameShown == false)
			{
				paintboard();
				animationFrameShown = true;
			}
			doKeys();
			redraw = false;
		}

		al_wait_for_event(queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			if(!al_acknowledge_resize(event.display.source))
			{
				DisplayErr("Failed to resize diplay");
				exit(0);
			}
			timeToReloadSegment = true;
			redraw = true;
#if 1
			{
				/* XXX the opengl drivers currently don't resize the backbuffer */
				ALLEGRO_BITMAP *bb = al_get_backbuffer();
				int w = al_get_bitmap_width(bb);
				int h = al_get_bitmap_height(bb);
				WriteErr("backbuffer w, h: %d, %d\n", w, h);
			}
#endif
		}
		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				break;
			else 
			{
				doKeys(event.keyboard.keycode);
				redraw = true;
			}
		}
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		if (event.type == ALLEGRO_EVENT_TIMER &&
			event.timer.source == reloadtimer){
				timeToReloadSegment = true;
				redraw = true;
		}
		if (event.type == ALLEGRO_EVENT_TIMER &&
			event.timer.source == animationtimer){
				animUpdateProc();
				redraw = true;
		}
	}
	flushImgFiles();
	DisconnectFromDF();

	//dispose old segment
	if(viewedSegment){
		viewedSegment->Dispose();
		delete(viewedSegment);
	}

	return 0;
}
//END_OF_MAIN()
