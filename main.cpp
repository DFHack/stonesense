#include <assert.h>
#include <vector>
#include <list>

#include "common.h"
#include "commonTypes.h"

#include "Config.h"
#include "Tile.h"
#include "GUI.h"
//#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Constructions.h"
#include "DumpInfo.h"
#include "MapLoading.h"
#include "WorldSegment.h"
#include "Creatures.h"
#include "GroundMaterialConfiguration.h"
#include "ContentLoader.h"
#include "OcclusionTest.h"
#include "GameConfiguration.h"
#include "GameState.h"
#include "StonesenseState.h"

#include "Debug.h"

static constexpr auto WIDTH = 640;
static constexpr auto HEIGHT = 480;
static constexpr auto SIZE_LOG = 50;

//set the plugin name/dfhack version
DFHACK_PLUGIN("stonesense");
DFHACK_PLUGIN_IS_ENABLED(enabled);
REQUIRE_GLOBAL(init);

namespace DFHack {
    DBG_DECLARE(stonesense, main, DebugCategory::LINFO);
}

StonesenseState stonesenseState;

bool stonesense_started = 0;

int keyoffset=0;

std::vector<DFHack::t_matgloss> v_stonetypes;

ALLEGRO_DISPLAY * display;

ALLEGRO_EVENT event;

ALLEGRO_BITMAP* IMGIcon;
ALLEGRO_BITMAP* SplashImage = NULL;

int mouse_x, mouse_y, mouse_z;
unsigned int mouse_b;

/// main thread of stonesense - handles events
ALLEGRO_THREAD *stonesense_event_thread;
// the segment wrapper handles concurrency control
bool redraw = true;

ALLEGRO_BITMAP* load_bitmap_withWarning(std::filesystem::path path)
{
    ALLEGRO_BITMAP* img = 0;
    img = al_load_bitmap(path.string().c_str());
    if(!img) {
        LogError("Cannot load image: %s\n", path.string().c_str());
        al_set_thread_should_stop(stonesense_event_thread);
        return 0;
    }
    al_convert_mask_to_alpha(img, al_map_rgb(255, 0, 255));
    return img;
}

void LogError(const char* msg, ...)
{
    va_list arglist;
    va_start(arglist, msg);
    std::string buf = stl_vsprintf(msg, arglist);
    va_end(arglist);

    WARN(main) << buf;

    std::ofstream fp{ std::filesystem::path { "Stonesense.log" }, std::ios::app };
    fp << buf;
}

void PrintMessage(const char* msg, ...)
{
    va_list arglist;
    va_start(arglist, msg);
    std::string buf = stl_vsprintf(msg, arglist);
    va_end(arglist);

    INFO(main) << buf;
}

void LogVerbose(const char* msg, ...)
{
    va_list arglist;
    va_start(arglist, msg);
    std::string buf = stl_vsprintf(msg, arglist);
    va_end(arglist);

    TRACE(main) << buf;

    if (!stonesenseState.ssConfig.config.verbose_logging)
        return;

    std::ofstream fp{ std::filesystem::path { "Stonesense.log" }, std::ios::app };
    fp << buf;
}

void SetTitle(const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    std::string buf = stl_vsprintf(format, arglist);
    va_end(arglist);

    al_set_window_title(display, buf.c_str());
}

bool loadfont(DFHack::color_ostream & output)
{
    std::filesystem::path p{ "stonesense" };
    p /= stonesenseState.ssConfig.config.font;
    stonesenseState.font = al_load_font(p.string().c_str(), stonesenseState.ssConfig.config.fontsize, 0);
    if (!stonesenseState.font) {
        output.printerr("Cannot load font: %s\n", p.string().c_str());
        return false;
    }
    return true;
}

void benchmark()
{
    stonesenseState.ssState.Position.x = stonesenseState.ssState.Position.y = 0;
    stonesenseState.ssState.Position.x = 110;
    stonesenseState.ssState.Position.y = 110;
    stonesenseState.ssState.Position.z = 18;
    uint32_t startTime = clock();
    int i = 20;
    while(i--) {
        reloadPosition();
    }
    std::ofstream fp{ std::filesystem::path { "benchmark.txt" } };
    fp << clock() - startTime << "ms";
}

void animUpdateProc()
{
    if (stonesenseState.animationFrameShown) {
        // check before setting, or threadsafety will be borked
        if (stonesenseState.currentAnimationFrame >= (MAX_ANIMFRAME-1)) { // ie ends up [0 .. MAX_ANIMFRAME)
            stonesenseState.currentAnimationFrame = 0;
        } else {
            stonesenseState.currentAnimationFrame++;
        }
        stonesenseState.currentFrameLong++;
        stonesenseState.animationFrameShown = false;
    }
}

void drawcredits()
{
    auto color_black = al_map_rgb(0, 0, 0);
    auto color_yellow = al_map_rgb(255, 255, 0);
    auto color_white = al_map_rgb(255, 255, 255);

    auto backbuffer = al_get_backbuffer(al_get_current_display());
    auto centerx = al_get_bitmap_width(backbuffer) / 2;
    auto bottomy = al_get_bitmap_height(backbuffer);
    auto centery = bottomy / 2;
    auto lineheight = al_get_font_line_height(stonesenseState.font);

    al_clear_to_color(color_black);

    if (SplashImage)
    {
        auto splash_width = al_get_bitmap_width(SplashImage);
        auto splash_height = al_get_bitmap_height(SplashImage);
        al_draw_bitmap_region(SplashImage, 0, 0, splash_width, splash_height, centerx - splash_width / 2, centery - splash_height / 2, 0);
    }

    auto font = stonesenseState.font;
    al_draw_text(font, color_yellow, centerx, 5*lineheight, ALLEGRO_ALIGN_CENTRE, "Welcome to Stonesense Felsite!");
    al_draw_text(font, color_white, centerx, 6*lineheight, ALLEGRO_ALIGN_CENTRE, "Stonesense is an isometric viewer for Dwarf Fortress.");

    al_draw_text(font, color_white, centerx, 8*lineheight, ALLEGRO_ALIGN_CENTRE, "Programming: Jonas Ask, Kris Parker, Japa Illo, Tim Aitken, and peterix");
    al_draw_text(font, color_white, centerx, 9*lineheight, ALLEGRO_ALIGN_CENTRE, "Lead graphics designer, Dale Holdampf");

    al_draw_text(font, color_white, centerx, bottomy-13*lineheight, ALLEGRO_ALIGN_CENTRE, "Contributors:");
    al_draw_text(font, color_white, centerx, bottomy-12*lineheight, ALLEGRO_ALIGN_CENTRE, "7c Nickel, BatCountry, Belal, Belannaer, DeKaFu, Dante, Deon, dyze,");
    al_draw_text(font, color_white, centerx, bottomy-11*lineheight, ALLEGRO_ALIGN_CENTRE, "Errol, fifth angel, frumpton, IDreamOfGiniCoeff, Impaler, ");
    al_draw_text(font, color_white, centerx, bottomy-10*lineheight, ALLEGRO_ALIGN_CENTRE, "Japa, jarathor, Jiri Petru, Jordix, Lord Nightmare, McMe, Mike Mayday, Nexii ");
    al_draw_text(font, color_white, centerx, bottomy-9*lineheight, ALLEGRO_ALIGN_CENTRE, "Malthus, peterix, Seuss, soup, SquidCoder, Talvara, winner, Xandrin.");

    al_draw_text(font, color_white, centerx, bottomy-7*lineheight, ALLEGRO_ALIGN_CENTRE, "With special thanks to peterix for making DFHack");

    //"The program is in a very early alpha, we're only showcasing it to get ideas and feedback, so use it at your own risk."
    //al_draw_text(font, color_white, centerx, bottomy-4*lineheight, ALLEGRO_ALIGN_CENTRE, "Press F9 to continue");
    // Make the backbuffer visible
}

/* main_loop:
*  The main loop of the program.  Here we wait for events to come in from
*  any one of the event sources and react to each one accordingly.  While
*  there are no events to react to the program sleeps and consumes very
*  little CPU time.  See main() to see how the event sources and event queue
*  are set up.
*/
static void main_loop(ALLEGRO_DISPLAY * display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_THREAD * main_thread, DFHack::color_ostream & con)
{
    auto& ssConfig = stonesenseState.ssConfig;

    ALLEGRO_EVENT event;
    while (!al_get_thread_should_stop(main_thread)) {
        if (redraw && al_event_queue_is_empty(queue)) {

            al_rest(0);

            {
                if (ssConfig.spriteIndexOverlay) {
                    DrawSpriteIndexOverlay(ssConfig.currentSpriteOverlay);
                    al_flip_display();
                }
                else if (!DFHack::Maps::IsValid()) {
                    drawcredits();
                    al_flip_display();
                }
                else if (stonesenseState.timeToReloadSegment) {
                    reloadPosition();
                    al_clear_to_color(ssConfig.config.backcol);
                    paintboard();
                    al_flip_display();
                    stonesenseState.timeToReloadSegment = false;
                    stonesenseState.animationFrameShown = true;
                }
                else if (stonesenseState.animationFrameShown == false) {
                    al_clear_to_color(ssConfig.config.backcol);
                    paintboard();
                    al_flip_display();
                    stonesenseState.animationFrameShown = true;
                }
            }

                doMouse();
                doRepeatActions();
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

        if(in_time) {
            switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                stonesenseState.timeToReloadSegment = true;
                redraw = true;
                stonesenseState.ssState.ScreenH = event.display.height;
                stonesenseState.ssState.ScreenW = event.display.width;
                if (!al_acknowledge_resize(event.display.source)) {
                    con.printerr("Failed to resize diplay");
                    return;
                }
                break;
                /* ALLEGRO_EVENT_KEY_DOWN - a keyboard key was pressed.
                */
            case ALLEGRO_EVENT_KEY_CHAR:
                if(event.keyboard.display != display) {
                    break;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && ssConfig.config.closeOnEsc) {
                    return;
                } else {
                    doKeys(event.keyboard.keycode, event.keyboard.modifiers);
                    redraw = true;
                }
                break;

                /* ALLEGRO_EVENT_DISPLAY_CLOSE - the window close button was pressed.
                */
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                return;

            case ALLEGRO_EVENT_TIMER:
                if(event.timer.source == stonesenseState.reloadtimer) {
                    stonesenseState.timeToReloadSegment = true;
                    redraw = true;
                } else if (event.timer.source == stonesenseState.animationtimer) {
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
static void* stonesense_thread(ALLEGRO_THREAD* main_thread, void* parms)
{
    auto& out{ DFHack::Core::getInstance().getConsole() };
    out.print("Stonesense launched\n");

    stonesenseState.ssConfig.reset();

    stonesenseState.lift_segment_offscreen_x = 0;
    stonesenseState.lift_segment_offscreen_y = 0;

    stonesenseState.ssState.ScreenH = stonesenseState.ssConfig.config.defaultScreenHeight;
    stonesenseState.ssState.ScreenW = stonesenseState.ssConfig.config.defaultScreenWidth;
    stonesenseState.ssState.Size = { DEFAULT_SIZE, DEFAULT_SIZE, DEFAULT_SIZE_Z };
    stonesenseState.timeToReloadConfig = true;
    stonesenseState.contentLoader = std::make_unique<ContentLoader>();

    stonesenseState.stoneSenseTimers = FrameTimers{};

    initRandomCube();
    if (!loadConfigFile() || !loadKeymapFile()) {
        stonesense_started = 0;
        return NULL;
    }
    init_masks();
    if(!loadfont(out)) {
        stonesense_started = 0;
        return NULL;
    }

    uint32_t version = al_get_allegro_version();
    int major = version >> 24;
    int minor = (version >> 16) & 255;
    int revision = (version >> 8) & 255;
    int release = version & 255;

    out.print("Using allegro version %d.%d.%d r%d\n", major, minor, revision, release);

    auto& ssConfig = stonesenseState.ssConfig;
    al_set_new_display_flags(
        (ssConfig.config.Fullscreen ? ALLEGRO_FULLSCREEN : ALLEGRO_WINDOWED)
        |(ALLEGRO_RESIZABLE)
        |(0)
        |(ssConfig.config.opengl ? ALLEGRO_OPENGL : 0)
        |(ssConfig.config.directX ? ALLEGRO_DIRECT3D_INTERNAL : 0));

    if(ssConfig.config.software) {
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP|_ALLEGRO_ALPHA_TEST|ALLEGRO_MIN_LINEAR|ALLEGRO_MIPMAP);
    } else {
        // FIXME: When we have ability to set a maximum mipmap lod,
        // do so when cache_images is enabled to prevent sprites going transparent.
        // Until then, disable mipmapping when using an image cache.
        al_set_new_bitmap_flags(
                ALLEGRO_MIN_LINEAR
                |(ssConfig.config.cache_images ? 0 : ALLEGRO_MIPMAP));
    }

    display = al_create_display(stonesenseState.ssState.ScreenW, stonesenseState.ssState.ScreenH);
    if (!display) {
        out.printerr("al_create_display failed\n");
        stonesense_started = 0;
        return NULL;
    }

    // Overwrite our screen size with the actual size.
    // Window managers may overwrite our specified size without emiting
    // a resize event for us.
    stonesenseState.ssState.ScreenW = al_get_display_width(display);
    stonesenseState.ssState.ScreenH = al_get_display_height(display);

    if(!al_is_keyboard_installed()) {
        if (!al_install_keyboard()) {
            out.printerr("Stonesense: al_install_keyboard failed\n");
        }
    }
    if(!al_is_mouse_installed()) {
        if (!al_install_mouse()) {
            out.printerr("Stonesense: al_install_mouse failed\n");
        }
    }
    SetTitle("Stonesense");
    drawcredits();


    std::filesystem::path p = std::filesystem::path{} / "stonesense" / "stonesense.png";
    IMGIcon = load_bitmap_withWarning(p);
    if(!IMGIcon) {
        al_destroy_display(display);
        display = 0;
        stonesense_started = 0;
        return NULL;
    }

    al_set_display_icon(display, IMGIcon);

    ALLEGRO_EVENT_QUEUE *queue;
    queue = al_create_event_queue();
    if (!queue) {
        out.printerr("al_create_event_queue failed\n");
        stonesense_started = 0;
        return NULL;
    }

    {
        std::filesystem::path p = std::filesystem::path{} / "stonesense" / "splash.png";
        SplashImage = load_bitmap_withWarning(p);
    }

    loadGraphicsFromDisk();
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_textf(stonesenseState.font, al_map_rgb(255,255,255), stonesenseState.ssState.ScreenW/2, stonesenseState.ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "Starting up...");
    al_flip_display();

    stonesenseState.reloadtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(ssConfig.config.automatic_reload_time));
    stonesenseState.animationtimer = al_create_timer(ALLEGRO_MSECS_TO_SECS(ssConfig.config.animation_step));

    if(ssConfig.config.animation_step) {
        al_start_timer(stonesenseState.animationtimer);
    }


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(stonesenseState.reloadtimer));
    al_register_event_source(queue, al_get_timer_event_source(stonesenseState.animationtimer));

    ssConfig.readCond = al_create_cond();

#ifdef BENCHMARK
    benchmark();
#endif
    // init map segment wrapper and its lock, start the reload thread.
    initAutoReload();

    stonesenseState.timeToReloadSegment = false;
    // enter event loop here:
    main_loop(display, queue, main_thread, out);

    // window is destroyed.
    al_destroy_display(display);
    display = 0;

    if(ssConfig.threadmade) {
        al_broadcast_cond(ssConfig.readCond);
        al_destroy_thread(ssConfig.readThread);
        ssConfig.spriteIndexOverlay = 0;
    }
    flushImgFiles();

    // remove the uranium fuel from the reactor... or map segment from the clutches of other threads.
    stonesenseState.map_segment.shutdown();
    al_destroy_bitmap(IMGIcon);
    IMGIcon = 0;
    stonesenseState.contentLoader.reset();
    out.print("Stonesense shutdown.\n");
    stonesense_started = 0;
    return NULL;
}

using namespace DFHack;

//All this fun DFhack stuff I gotta do now.
DFhackCExport command_result stonesense_command(color_ostream &out, std::vector<std::string> & params);

//This is the init command. it includes input options.
DFhackCExport command_result plugin_init ( color_ostream &out, std::vector <PluginCommand> &commands)
{
    enabled = true;
    commands.push_back(PluginCommand("stonesense","Start up the stonesense visualiser.",stonesense_command));
    commands.push_back(PluginCommand("ssense","Start up the stonesense visualiser.",stonesense_command));
    return CR_OK;
}

//this command is called every frame DF.
DFhackCExport command_result plugin_onupdate ( color_ostream &out )
{
    return CR_OK;
}

//And the shutdown command.
DFhackCExport command_result plugin_shutdown ( color_ostream &out )
{
    if(stonesense_event_thread) {
        al_join_thread(stonesense_event_thread, NULL);
    }
    al_uninstall_system();
    return CR_OK;
}

//and the actual stonesense command. Maybe.
DFhackCExport command_result stonesense_command(color_ostream &out, std::vector<std::string> & params)
{
    if(stonesense_started) {
        out.print("Stonesense already running.\n");
        return CR_OK;
    }
    stonesenseState.ssConfig.immersive_mode = false;
    if(params.size() > 0 ) {
        if(params[0] == "immersive"){
            auto focusStr = DFHack::Gui::getCurFocus().front();
            if (!(focusStr.starts_with("title") ||
                focusStr.starts_with("loadgame"))) {
                out.print(
                    "You need to start this mode from the titlescreen and enable keyboard cursor (in settings) to ensure a proper state."
                );
                return CR_OK;
            }
            stonesenseState.ssConfig.immersive_mode = true;
        } else {
            DumpInfo(out, params);
            return CR_OK;
        }
    }

    if(!al_is_system_installed()) {
        if (!al_init()) {
            out.printerr("Could not init Allegro.\n");
            return CR_FAILURE;
        }

        if (!al_init_image_addon()) {
            out.printerr("al_init_image_addon failed. \n");
            return CR_FAILURE;
        }
        if (!al_init_primitives_addon()) {
            out.printerr("al_init_primitives_addon failed. \n");
            return CR_FAILURE;
        }
        al_init_font_addon();
        if (!al_init_ttf_addon()) {
            out.printerr("al_init_ttf_addon failed. \n");
            return CR_FAILURE;
        }
    }

    stonesense_started = true;
    stonesense_event_thread = al_create_thread(stonesense_thread, (void*)&out);
    al_start_thread(stonesense_event_thread);
    return CR_OK;
}
