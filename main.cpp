#include <assert.h>
#include <vector>
#include <list>

using namespace std;

#include "common.h"
#include "Block.h"
#include "GUI.h"
//#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"
#include "MapLoading.h"
#include "WorldSegment.h"
#include "Creatures.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"
#include "Occlusion_Test.h"

#define WIDTH        640
#define HEIGHT       480
#define SIZE_LOG     50

#ifdef LINUX_BUILD
#include "stonesense.xpm"
extern void *allegro_icon;
#endif

bool stonesense_started = 0;

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

ALLEGRO_EVENT event;

ALLEGRO_BITMAP* IMGIcon;

int mouse_x, mouse_y, mouse_z;
unsigned int mouse_b;
bool key[ALLEGRO_KEY_MAX];

DFHack::Console * DFConsole;

ALLEGRO_THREAD *thread;

bool redraw = true;

/*int32_t viewx = 0;
int32_t viewy = 0;
int32_t viewz = 0;
bool followmode = true;*/

ALLEGRO_BITMAP* load_bitmap_withWarning(const char* path)
{
    ALLEGRO_BITMAP* img = 0;
    img = al_load_bitmap(path);
    if(!img)
    {
        DFConsole->printerr("Cannot load image: %s\n", path);
        al_set_thread_should_stop(thread);
        return 0;
    }
    al_convert_mask_to_alpha(img, al_map_rgb(255, 0, 255));
    return img;
}


void WriteErr(const char* msg, ...){
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

void LogVerbose(const char* msg, ...){
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

void SetTitle(const char *format, ...)
{
	ALLEGRO_USTR *buf;
	va_list ap;
	const char *s;

	/* Fast path for common case. */
	if (0 == strcmp(format, "%s")) {
		va_start(ap, format);
		s = va_arg(ap, const char *);
		al_set_window_title(display, s);
		va_end(ap);
		return;
	}

	va_start(ap, format);
	buf = al_ustr_new("");
	al_ustr_vappendf(buf, format, ap);
	va_end(ap);

	al_set_window_title(display, al_cstr(buf));

	al_ustr_free(buf);
}

void correctBlockForSegmetOffset(int32_t& x, int32_t& y, int32_t& z){
	x -= DisplayedSegmentX;
	y -= DisplayedSegmentY; //DisplayedSegmentY;
	z -= DisplayedSegmentZ - 1; // + viewedSegment->sizez - 2; // loading one above the top of the displayed segment for block rules
}

bool loadfont()
{
    ALLEGRO_PATH * p = al_create_path_for_directory("stonesense");
    if(!al_join_paths(p, config.font))
    {
        al_destroy_path(p);
        return false;
    }
    font = al_load_font(al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP), config.fontsize, 0);
    if (!font)
    {
        DFConsole->printerr("Cannot load font: %s\n", al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP));
        al_destroy_path(p);
        return false;
    }
    al_destroy_path(p);
    return true;
}

void benchmark(DFHack::Core * c){
	DisplayedSegmentX = DisplayedSegmentY = 0;
	DisplayedSegmentX = 110; DisplayedSegmentY = 110;DisplayedSegmentZ = 18;
	uint32_t startTime = clock();
	int i = 20;
	while(i--)
		reloadDisplayedSegment(c);

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
    static ALLEGRO_BITMAP* SplashImage = NULL; // BUG: leaks the image
	al_clear_to_color(al_map_rgb(0,0,0));
	//centred splash image
	{
        if(!SplashImage)
        {
            ALLEGRO_PATH * p = al_create_path("stonesense/splash.png");
            SplashImage = load_bitmap_withWarning(al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP));
            al_destroy_path(p);
        }
		if(!SplashImage)
			return;
		al_draw_bitmap_region(SplashImage, 0, 0,
			al_get_bitmap_width(SplashImage), al_get_bitmap_height(SplashImage),
			(al_get_bitmap_width(al_get_backbuffer(al_get_current_display())) - al_get_bitmap_width(SplashImage))/2,
			(al_get_bitmap_height(al_get_backbuffer(al_get_current_display())) - al_get_bitmap_height(SplashImage))/2, 0);
	}
	al_draw_text(font, al_map_rgb(255, 255, 0), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, 5*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Welcome to Stonesense Felsite!");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, 6*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Stonesense is an isometric viewer for Dwarf Fortress.");

	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, 8*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Programming, Jonas Ask, Kris Parker, Japa Illo, and peterix");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, 9*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Lead graphics designer, Dale Holdampf");

	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-13*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Contributors:");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-12*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "7c Nickel, BatCountry, Belal, Belannaer, DeKaFu, Dante, Deon, dyze,");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-11*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Errol, fifth angel, frumpton, IDreamOfGiniCoeff, Impaler, ");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-10*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Japa, jarathor, Jiri Petru, Jordix, Lord Nightmare, McMe, Mike Mayday, Nexii ");
	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-9*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Malthus, peterix, Seuss, soup, Talvara, winner, Xandrin.");

	al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-7*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "With special thanks to peterix for making dfHack");

	//"The program is in a very early alpha, we're only showcasing it to get ideas and feedback, so use it at your own risk."
	//al_draw_text(font, al_map_rgb(255, 255, 255), al_get_bitmap_width(al_get_backbuffer(al_get_current_display()))/2, al_get_bitmap_height(al_get_backbuffer(al_get_current_display()))-4*al_get_font_line_height(font), ALLEGRO_ALIGN_CENTRE, "Press F9 to continue");
	// Make the backbuffer visible
	al_flip_display();
}
//
/*
int main(void)
{




//upper left corners
DisplayedSegmentX = DisplayedSegmentY = DisplayedSegmentZ = 0;

//ramps
//DisplayedSegmentX = 238; DisplayedSegmentY = 220;DisplayedSegmentZ = 23;

//ford. Main hall
//DisplayedSegmentX = 172; DisplayedSegmentY = 195;DisplayedSegmentZ = 15;

//ford. desert map
//sDisplayedSegmentX = 78; DisplayedSegmentY = 123;DisplayedSegmentZ = 15;

//DisplayedSegmentX = 125; DisplayedSegmentY = 125;DisplayedSegmentZ = 18;

//DisplayedSegmentX = 242; DisplayedSegmentY = 345;DisplayedSegmentZ = 15;


DFHack::CoreManager DFMgr("Memory.xml");

while (true) {


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
if(config.threadmade)
{
al_broadcast_cond(config.readCond);
al_destroy_thread(config.readThread);
config.spriteIndexOverlay = 0;
}
flushImgFiles();
DisconnectFromDF();

//dispose old segments
if(altSegment){
altSegment->Dispose();
delete(altSegment);
}
if(viewedSegment){
viewedSegment->Dispose();
delete(viewedSegment);
}

return 0;
}
*/

/* main_loop:
*  The main loop of the program.  Here we wait for events to come in from
*  any one of the event sources and react to each one accordingly.  While
*  there are no events to react to the program sleeps and consumes very
*  little CPU time.  See main() to see how the event sources and event queue
*  are set up.
*/
static void main_loop(ALLEGRO_DISPLAY * display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_THREAD * thred, DFHack::Console & con, DFHack::Core * core)
{
	ALLEGRO_EVENT event;
	DFHack::t_gamemodes game_mode;
	while (!al_get_thread_should_stop(thred))
	{
		if (redraw && al_event_queue_is_empty(queue))
		{
			al_rest(0);
			if(config.spriteIndexOverlay)
			{
				DrawSpriteIndexOverlay(config.currentSpriteOverlay);
			}
			else if(!Maps::IsValid())
			{
				drawcredits();
			}
			else if( timeToReloadSegment ){
				reloadDisplayedSegment(core);
				al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
				paintboard();
				timeToReloadSegment = false;
				animationFrameShown = true;
			}
			else if (animationFrameShown == false)
			{
				al_clear_to_color(al_map_rgb(config.backr,config.backg,config.backb));
				paintboard();
				animationFrameShown = true;
			}
			doKeys();
			redraw = false;
		}
		/* Take the next event out of the event queue, and store it in `event'. */
		bool in_time = 0;
		in_time = al_wait_for_event_timed(queue, &event, 1.0f);

		/* Check what type of event we got and act accordingly.  ALLEGRO_EVENT
		* is a union type and interpretation of its contents is dependent on
		* the event type, which is given by the 'type' field.
		*
		* Each event also comes from an event source and has a timestamp.
		* These are accessible through the 'any.source' and 'any.timestamp'
		* fields respectively, e.g. 'event.any.timestamp'
		*/

		if(in_time)
		{
			switch (event.type)
			{
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				if(!al_acknowledge_resize(event.display.source))
				{
					DFConsole->printerr("Failed to resize diplay");
					return;
				}
				timeToReloadSegment = true;
				redraw = true;
#if 1
				{
					//XXX the opengl drivers currently don't resize the backbuffer
					ALLEGRO_BITMAP *bb = al_get_backbuffer(al_get_current_display());
					int w = al_get_bitmap_width(bb);
					int h = al_get_bitmap_height(bb);
					WriteErr("backbuffer w, h: %d, %d\n", w, h);
				}
#endif
				/* ALLEGRO_EVENT_KEY_DOWN - a keyboard key was pressed.
				*/
			case ALLEGRO_EVENT_KEY_DOWN:
				if(event.keyboard.display != display)
					break;
				else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					return;
				}
				else
				{
					doKeys(event.keyboard.keycode);
					redraw = true;
				}
				break;

				/* ALLEGRO_EVENT_DISPLAY_CLOSE - the window close button was pressed.
				*/
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				return;

			case ALLEGRO_EVENT_TIMER:
				if(event.timer.source == reloadtimer)
				{
					timeToReloadSegment = true;
					redraw = true;
				}
				else if (event.timer.source == animationtimer)
				{
					animUpdateProc();
					redraw = true;
				}
				/* We received an event of some type we don't know about.
				* Just ignore it.
				*/
			default:
				break;
			}
		}
	}
}

//replacement for main()
static void * stonesense_thread(ALLEGRO_THREAD * thred, void * parms)
{
	DFHack::Core * c = (DFHack::Core * )parms;
	DFConsole->print("Stonesense launched\n");

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
	config.font = al_create_path("data/art/font.ttf");
	config.creditScreen = true;
	config.bloodcutoff = 100;
	config.poolcutoff = 100;
	config.threadmade = 0;
	config.threading_enable = 1;
	config.fog_of_war = 1;
	config.occlusion = 1;
    contentLoader = new ContentLoader();
	config.zoom = 1;
	config.scale = 1.0f;
	initRandomCube();
	loadConfigFile();
	init_masks();
	if(!loadfont())
	{
		stonesense_started = 0;
		return NULL;
	}
	//set debug cursor
	debugCursor.x = config.segmentSize.x / 2;
	debugCursor.y = config.segmentSize.y / 2;

	uint32_t version = al_get_allegro_version();
	int major = version >> 24;
	int minor = (version >> 16) & 255;
	int revision = (version >> 8) & 255;
	int release = version & 255;

	DFConsole->print("Using allegro version %d.%d.%d r%d\n", major, minor, revision, release);

	int gfxMode = config.Fullscreen ? ALLEGRO_FULLSCREEN : ALLEGRO_WINDOWED;
	al_set_new_display_flags(gfxMode|ALLEGRO_RESIZABLE|(config.opengl ? ALLEGRO_OPENGL : 0)|(config.directX ? ALLEGRO_DIRECT3D_INTERNAL : 0));
	display = al_create_display(config.screenWidth, config.screenHeight);
	if (!display) {
		c->con.printerr("al_create_display failed\n");
		stonesense_started = 0;
		return NULL;
	}
    if(!al_is_keyboard_installed())
    {
        if (!al_install_keyboard())
        {
            DFConsole->printerr("Stonesense: al_install_keyboard failed\n");
        }
    }
    if(!al_is_mouse_installed())
    {
        if (!al_install_mouse())
        {
            DFConsole->printerr("Stonesense: al_install_mouse failed\n");
        }
    }
	SetTitle("Stonesense");

	if(config.software)
		al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP|ALLEGRO_ALPHA_TEST|ALLEGRO_MIN_LINEAR|ALLEGRO_MAG_LINEAR);
	else al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR|ALLEGRO_MAG_LINEAR);

    ALLEGRO_PATH * p = al_create_path("stonesense/stonesense.png");
    IMGIcon = load_bitmap_withWarning(al_path_cstr(p, ALLEGRO_NATIVE_PATH_SEP));
    al_destroy_path(p);
	if(!IMGIcon)
	{
        al_destroy_display(display);
		stonesense_started = 0;
		return NULL;
	}

	al_set_display_icon(display, IMGIcon);

	ALLEGRO_EVENT_QUEUE *queue;
	queue = al_create_event_queue();
	if (!queue) {
		c->con.printerr("al_create_event_queue failed\n");
		stonesense_started = 0;
		return NULL;
	}

	loadGraphicsFromDisk();
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_textf(font, al_map_rgb(255,255,255), al_get_bitmap_width(al_get_target_bitmap())/2, al_get_bitmap_height(al_get_target_bitmap())/2, ALLEGRO_ALIGN_CENTRE, "Starting up...");
	al_flip_display();

	reloadtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(config.automatic_reload_time));
	animationtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(config.animation_step));

	if(config.animation_step)
	{
		al_start_timer(animationtimer);
	}


	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_timer_event_source(reloadtimer));
	al_register_event_source(queue, al_get_timer_event_source(animationtimer));

	config.readMutex = al_create_mutex();
	config.readCond = al_create_cond();

#ifdef BENCHMARK
	benchmark();
#endif
	//install_int( animUpdateProc, config.animation_step );
	initAutoReload();

	timeToReloadSegment = true;

	main_loop(display, queue, thred, c->con, c);
	al_destroy_display(display);

	if(config.threadmade)
	{
		al_broadcast_cond(config.readCond);
		al_destroy_thread(config.readThread);
		config.spriteIndexOverlay = 0;
	}
	flushImgFiles();

	if(altSegment){
		altSegment->Dispose();
		delete altSegment;
        altSegment = 0;
	}
	if(viewedSegment){
		viewedSegment->Dispose();
		delete viewedSegment;
        viewedSegment = 0;
	}
	al_destroy_bitmap(IMGIcon);
    IMGIcon = 0;
    delete contentLoader;
    contentLoader = 0;
	DFConsole->print("Stonesense shutdown.\n");
	stonesense_started = 0;
	return NULL;
}

//All this fun DFhack stuff I gotta do now.
DFhackCExport command_result stonesense_command(DFHack::Core * c, std::vector<std::string> & params);

//set the plugin name
DFhackCExport const char * plugin_name ( void )
{
	return "stonesense";
}

//This is the init command. it includes input options.
DFhackCExport command_result plugin_init ( Core * c, std::vector <PluginCommand> &commands)
{
	commands.clear();
	commands.push_back(PluginCommand("stonesense","Start up the stonesense visualiser.",stonesense_command));
    commands.push_back(PluginCommand("ssense","Start up the stonesense visualiser.",stonesense_command));
	return CR_OK;
}

//this command is called every frame DF.
DFhackCExport command_result plugin_onupdate ( Core * c )
{
	return CR_OK;
}

//And the shutdown command.
DFhackCExport command_result plugin_shutdown ( Core * c )
{
	if(thread)
		al_join_thread(thread, NULL);
	al_uninstall_system();
	return CR_OK;
}

//and the actual stonesense command. Maybe.
DFhackCExport command_result stonesense_command(DFHack::Core * c, std::vector<std::string> & params)
{
	if(stonesense_started)
	{
		c->con.print("Stonesense already running.\n");
		return CR_OK;
	}
	stonesense_started = true;
	DFConsole = &(c->con);
	if(!al_is_system_installed())
	{
		if (!al_init()) {
			DFConsole->printerr("Could not init Allegro.\n");
			return CR_FAILURE;
		}

		if (!al_init_image_addon())
		{
			DFConsole->printerr("al_init_image_addon failed. \n");
			return CR_FAILURE;
		}
		if (!al_init_primitives_addon()) {
			DFConsole->printerr("al_init_primitives_addon failed. \n");
			return CR_FAILURE;
		}
		al_init_font_addon();
		if (!al_init_ttf_addon())
		{
			DFConsole->printerr("al_init_ttf_addon failed. \n");
			return CR_FAILURE;
		}
	}

	thread = al_create_thread(stonesense_thread, (void * )c);
	al_start_thread(thread);
	return CR_OK;
}
