#ifndef LINUX_BUILD
#   define NOMINMAX
#endif

#include <assert.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <array>
#include <unordered_set>

#include "common.h"
#include "Tile.h"
#include "GUI.h"
#include "WorldSegment.h"
#include "SpriteMaps.h"
#include "MapLoading.h"
#include "MiscUtils.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "ContentLoader.h"
#include "UserInput.h"
#include "GameConfiguration.h"
#include "StonesenseState.h"

#include "UserInput.h"

#include "modules/Screen.h"
#include "modules/Units.h"
#include "DataDefs.h"

#include "df/plotinfost.h"
#include "df/building_actual.h"
#include "df/world.h"

#include "df/itemdef.h"
#include "df/itemdef_weaponst.h"
#include "df/itemdef_trapcompst.h"
#include "df/itemdef_toyst.h"
#include "df/itemdef_toolst.h"
#include "df/itemdef_instrumentst.h"
#include "df/itemdef_armorst.h"
#include "df/itemdef_ammost.h"
#include "df/itemdef_siegeammost.h"
#include "df/itemdef_glovesst.h"
#include "df/itemdef_shoesst.h"
#include "df/itemdef_shieldst.h"
#include "df/itemdef_helmst.h"
#include "df/itemdef_pantsst.h"
#include "df/itemdef_foodst.h"
#include "df/descriptor_pattern.h"
#include "df/material.h"

#include "df/creature_raw.h"
#include "df/caste_raw.h"
#include "df/tissue_style_raw.h"

/*FIXME when adventure mode returns.
#include "df/viewscreen_dungeonmodest.h"
#include "df/viewscreen_dungeon_wrestlest.h"
*/
#include "df/adventurest.h"
#include "df/report.h"

#include "allegro5/allegro_color.h"
#include "allegro5/opengl/GLext/gl_ext_defs.h"

ALLEGRO_BITMAP* buffer = 0;
ALLEGRO_BITMAP* bigFile = 0;
GLhandleARB tinter;
GLhandleARB tinter_shader;

class ImageCache
{
private:
    std::vector<ALLEGRO_BITMAP*> cache;
public:
    void flush()
    {
        for (auto& c : cache)
            al_destroy_bitmap(c);
        cache.clear();
    }
    int size() const { return cache.size(); }
    int add(ALLEGRO_BITMAP* bmp) {
        cache.push_back(bmp);
        return cache.size() - 1;
    }
    ALLEGRO_BITMAP* get(int idx) const {
        return cache[idx];
    }
    int extend() {
        auto& ssConfig = stonesenseState.ssConfig;
        cache.push_back(al_create_bitmap(ssConfig.config.imageCacheSize, ssConfig.config.imageCacheSize));
        return cache.size() - 1;
    }

    ~ImageCache() {
        flush();
    };

} IMGCache;

class ImageFileList
{
private:
    struct Img
    {
        std::filesystem::path filename;
        ALLEGRO_BITMAP* bmPtr;
        Img(std::filesystem::path filename, ALLEGRO_BITMAP* bmp) : filename{ filename }, bmPtr{ bmp } {};
        ~Img() {}
    };
    std::vector<Img> list;
public:
    ALLEGRO_BITMAP* lookup(int index) const
    {
        if (index == -1)
            return stonesenseState.IMGObjectSheet;
        else if (index >= int(list.size()))
            return nullptr;
        else
            return list[index].bmPtr;
    }
    int size() const { return int(list.size()); }
    const std::filesystem::path& getFilename(size_t index) const
    {
        return list[index].filename;
    }
    ALLEGRO_BITMAP* getBitmap(size_t index) const
    {
        return list[index].bmPtr;
    }
    void flush()
    {
        for (auto& i : list)
        {
            if (i.bmPtr) al_destroy_bitmap(i.bmPtr);
        }
        list.clear();
    }
    bool contains(std::filesystem::path pathname)
    {
        auto it = std::find_if(list.begin(), list.end(), [&](auto& e) -> bool { return e.filename == pathname; });
        return it != list.end();
    }
    int lookup(std::filesystem::path pathname)
    {
        auto it = std::find_if(list.begin(), list.end(), [&](auto& e) -> bool { return e.filename == pathname; });
        return it != list.end() ? it - list.begin() : -1;
    }
    void add(std::filesystem::path filename, ALLEGRO_BITMAP* cache, int xOffset, int yOffset, ALLEGRO_BITMAP* bmp2)
    {
        ALLEGRO_BITMAP* bmp = al_create_sub_bitmap(cache, xOffset, yOffset, al_get_bitmap_width(bmp2), al_get_bitmap_height(bmp2));
        list.emplace_back(filename, bmp);
    }
    void add(std::filesystem::path filename, ALLEGRO_BITMAP* bmp)
    {
        list.emplace_back(filename, bmp);
    }

    ~ImageFileList()
    {
        flush();
    }

} IMGFileList;

namespace
{
    void ScreenToPoint(int inx, int iny, int& x1, int& y1, int& z1, int segSizeX, int segSizeY, int segSizeZ, int ScreenW, int ScreenH)
    {
        auto& ssConfig = stonesenseState.ssConfig;

        float x = inx;
        float y = iny;
        x -= ScreenW / 2.0;
        y -= ScreenH / 2.0;

        y = y / ssConfig.scale;
        y += TILETOPHEIGHT * 5.0 / 4.0;
        y += stonesenseState.lift_segment_offscreen_y;
        z1 = segSizeZ - 2;
        y += z1 * TILEHEIGHT;
        y = 2 * y / TILETOPHEIGHT;
        y += (segSizeX / 2) + (segSizeY / 2);

        x = x / ssConfig.scale;
        x -= stonesenseState.lift_segment_offscreen_x;
        x = 2 * x / TILEWIDTH;
        x += (segSizeX / 2) - (segSizeY / 2);

        x1 = (x + y) / 2;
        y1 = (y - x) / 2;

    }
}

void ScreenToPoint(int x,int y,int &x1, int &y1, int &z1)
{
    auto& ssState = stonesenseState.ssState;
    if(stonesenseState.ssConfig.track_screen_center){
        ScreenToPoint(x, y, x1, y1, z1, ssState.Size.x, ssState.Size.y, ssState.Size.z, ssState.ScreenW, ssState.ScreenH);
    } else {
        ScreenToPoint(x, y, x1, y1, z1, 0, 0, ssState.Size.z, 0, 0);
    }
}

namespace
{
    void pointToScreen(int* inx, int* iny, int inz, int segSizeX, int segSizeY, int ScreenW, int ScreenH) {
        auto& ssConfig = stonesenseState.ssConfig;
        auto& ssState = stonesenseState.ssState;

        int z = inz + 1 - ssState.Size.z;

        int x = *inx - *iny;
        x -= (segSizeX / 2) - (segSizeY / 2);
        x = x * TILEWIDTH / 2;
        x += stonesenseState.lift_segment_offscreen_x;
        x *= ssConfig.scale;

        int y = *inx + *iny;
        y -= (segSizeX / 2) + (segSizeY / 2);
        y = y * TILETOPHEIGHT / 2;
        y -= z * TILEHEIGHT;
        y -= TILETOPHEIGHT * 5 / 4;
        y -= stonesenseState.lift_segment_offscreen_y;
        y *= ssConfig.scale;

        x += ScreenW / 2;
        y += ScreenH / 2;

        *inx = x;
        *iny = y;
    }
}

void pointToScreen(int *inx, int *iny, int inz)
{
    auto& ssState = stonesenseState.ssState;

    if(stonesenseState.ssConfig.track_screen_center){
        pointToScreen(inx, iny, inz, ssState.Size.x, ssState.Size.y, ssState.ScreenW, ssState.ScreenH);
    } else {
        pointToScreen(inx, iny, inz, 0, 0, 0, 0);
    }
}

int get_textf_width(const ALLEGRO_FONT *font, const char *format, ...)
{
    ALLEGRO_USTR *buf;
    va_list ap;
    const char *s;

    int width;
    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(ap, format);
        s = va_arg(ap, const char *);
        width = al_get_text_width(font, s);
        va_end(ap);
        return width;
    }

    va_start(ap, format);
    buf = al_ustr_new("");
    al_ustr_vappendf(buf, format, ap);
    va_end(ap);

    width = al_get_text_width(font, al_cstr(buf));

    al_ustr_free(buf);
    return width;
}

void draw_textf_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const char *format, ...)
{
    ALLEGRO_USTR *buf;
    va_list arglist;
    const char *s;

    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(arglist, format);
        s = va_arg(arglist, const char *);
        draw_text_border(font, color, x, y, flags, s);
        va_end(arglist);
        return;
    }


    va_start(arglist, format);
    buf = al_ustr_new("");
    al_ustr_vappendf(buf, format, arglist);
    va_end(arglist);
    draw_ustr_border(font, color, x, y, flags, buf);
    al_ustr_free(buf);
}

namespace
{
    template<typename T>
    constexpr auto get_width = nullptr;
    template<> constexpr auto get_width<const char*> = &al_get_text_width;
    template<> constexpr auto get_width<const ALLEGRO_USTR*> = &al_get_ustr_width;

    template<typename T>
    constexpr auto draw_text = nullptr;
    template<> constexpr auto draw_text<const char*> = &al_draw_text;
    template<> constexpr auto draw_text<const ALLEGRO_USTR*> = &al_draw_ustr;

    void draw_color_border(const ALLEGRO_FONT* font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR border_color, float x, float y, int flags, auto ustr)
    {
        using T = decltype(ustr);
        int xx = 0;
        int ww = get_width<T>(font, ustr);
        // Prefer the overall font height for consistency,
        // even though it may vary depending on ascending/decending characters.
        int hh = al_get_font_line_height(font);

        if (flags & ALLEGRO_ALIGN_CENTRE) {
            xx -= ww / 2;
        }
        else if (flags & ALLEGRO_ALIGN_RIGHT) {
            xx -= ww;
        }
        al_draw_filled_rectangle(x + xx, y, x + xx + ww, y + hh, border_color);
        draw_text<T>(font, text_color, x, y, flags, ustr);
    }

    void draw_border(const ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y, int flags, auto ustr)
    {
        draw_color_border(font, color, al_map_rgba_f(0.0, 0.0, 0.0, 0.75), x, y, flags, ustr);
    }
}

void draw_text_border(const ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y, int flags, const char * ustr)
{
    draw_border(font, color, x, y, flags, ustr);
}

void draw_ustr_border(const ALLEGRO_FONT *font, ALLEGRO_COLOR color, float x, float y, int flags, const ALLEGRO_USTR *ustr)
{
    draw_border(font, color, x, y, flags, ustr);
}

void draw_color_text_border(const ALLEGRO_FONT* font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR border_color, float x, float y, int flags, const char* ustr)
{
    draw_color_border(font, text_color, border_color, x, y, flags, ustr);
}

void draw_color_ustr_border(const ALLEGRO_FONT* font, ALLEGRO_COLOR text_color, ALLEGRO_COLOR border_color, float x, float y, int flags, const ALLEGRO_USTR* ustr)
{
    draw_color_border(font, text_color, border_color, x, y, flags, ustr);
}

std::string fitTextToWidth(const std::string& input, ALLEGRO_FONT* font, int width) {
    std::string allCurrentChars;  // The part of the string that fits so far

    for (char nextChar : input) {
        std::string testString = allCurrentChars + nextChar + ".";  // Test adding the next char plus a dot

        if (al_get_text_width(font, testString.c_str()) > width) {
            return allCurrentChars + ".";  // If too wide, return what we have with a dot
        }

        allCurrentChars += nextChar;  // Append the character if it fits
    }

    return allCurrentChars;  // If we reached the end, return the full string
}


std::vector<std::string> splitLinesToWidth(const std::string& input,const ALLEGRO_FONT* font, int width) {
    std::vector<std::string> splits;  // To store each part of the split string
    std::string allCurrentChars;  // The part of the string that fits so far

    // Split input into words
    std::istringstream stream(input);
    std::string word;

    while (stream >> word) {
        // Test if adding this word to the current string would exceed the width
        std::string testString = allCurrentChars + (allCurrentChars.empty() ? "" : " ") + word;

        if (al_get_text_width(font, testString.c_str()) > width) {
            // If it's too wide, stop and push the current string into the vector
            if (!allCurrentChars.empty()) {
                splits.push_back(allCurrentChars);
            }
            allCurrentChars = word;  // Start a new line with the current word
        }
        else {
            // If it fits, append the word
            if (!allCurrentChars.empty()) {
                allCurrentChars += " ";
            }
            allCurrentChars += word;
        }
    }

    // Add any remaining text to the vector
    if (!allCurrentChars.empty()) {
        splits.push_back(allCurrentChars);
    }

    return splits;
}

// Base GUI Element Class
class GUIElement {
public:
    int x, y, w, h;  // Position and size of the element
    bool hovered;    // Tracks if the mouse is over the element
    std::unordered_set<std::string> visibleStates;

    GUIElement(int x, int y, int w, int h, std::unordered_set<std::string> visibleStates)
        : x(x), y(y), w(w), h(h), hovered(false), visibleStates(std::move(visibleStates)) {
    }

    virtual ~GUIElement() {}

    virtual void onClick() {}

    virtual void onRelease() {}

    virtual void onHover() {}

    virtual void onHoverExit() { onRelease(); }

    virtual void onScroll(int deltaY) {}

    virtual void draw() {}

    void update() {
        al_draw_text(stonesenseState.font, uiColor(dfColors::white), 0, 40, 0, stonesenseState.UIState.c_str());
        if (visibleStates.find(stonesenseState.UIState) != visibleStates.end()) {
            draw();
        }
    }

    bool isMouseOver(int mouseX, int mouseY) {
        return mouseX >= x && mouseX <= x + w &&
            mouseY >= y && mouseY <= y + h;
    }

    void updateHoverState(int mouseX, int mouseY) {
        bool mouseOver = isMouseOver(mouseX, mouseY);

        if (mouseOver && !hovered) {
            onHover();
        }
        else if (!mouseOver && hovered) {
            onHoverExit();
        }
        hovered = mouseOver;
    }

};

// Function pointer type
typedef void (*OnClickCallback)(uint32_t);

class Button : public GUIElement {
public:
    int32_t borderColor;
    int32_t bgColor;
    OnClickCallback onClickCallback;  // Function pointer for the callback
    bool held = false;

    Button(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor, OnClickCallback clickFn, std::unordered_set<std::string> visibleStates)
        : GUIElement(x, y, w, h, visibleStates), borderColor(borderColor), bgColor(bgColor), onClickCallback(clickFn) {
    }

    void onClick() override {
        if (!held) {
            held = true;
            if (visibleStates.find(stonesenseState.UIState) != visibleStates.end()) {
                if (onClickCallback) {
                    onClickCallback(getKeyMods(&stonesenseState.keyboard));  // Pass a uint32_t argument when the button is clicked
                }
            }
        }
    }

    void onRelease() override {
        held = false;
    }

    void draw() override {
        al_draw_filled_rectangle(x, y, x + w, y + h, uiColor(bgColor, hovered));
        al_draw_rectangle(x, y, x + w, y + h, uiColor(borderColor, hovered), 2);
    }
};

#include <string>

class LabeledButton : public Button {
public:
    std::string label;
    ALLEGRO_FONT* font;

    LabeledButton(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor,
        const std::string& label, ALLEGRO_FONT* font,
        OnClickCallback clickFn, std::unordered_set<std::string> visibleStates)
        : Button(x, y, w, h, borderColor, bgColor, clickFn, visibleStates),
        label(label), font(font) {
    }

    void draw() override {
        Button::draw(); // Draw base button

        // Draw label text centered
        if (font) {
            int textHeight = al_get_font_line_height(font);
            al_draw_text(font, uiColor(dfColors::white), x + (w / 2), y + (h - textHeight) / 2,
                ALLEGRO_ALIGN_CENTER, label.c_str());
        }
    }
};



class Tab : public Button {
public:
    int tabIndex;
    std::string label;
    ALLEGRO_FONT* font;

    Tab(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor, int tabIndex, const std::string& label, ALLEGRO_FONT* font, OnClickCallback onClickCallback, std::unordered_set<std::string> visibleStates)
        : Button(x, y, w, h, borderColor, bgColor, onClickCallback, visibleStates), tabIndex(tabIndex), label(label), font(font) {
    }

    void draw() override {
        bool isSelected = (stonesenseState.ssState.selectedTab == tabIndex);
        ALLEGRO_COLOR bg = uiColor(bgColor, isSelected);
        ALLEGRO_COLOR textColor = uiColor(dfColors::lgray, isSelected);

        al_draw_filled_rounded_rectangle(x, y, x + w, y + h, 10, 10, bg);
        al_draw_rounded_rectangle(x, y, x + w, y + h, 10, 10, uiColor(borderColor), 2);

        std::string truncatedLabel = fitTextToWidth(label, font, w - 20);
        al_draw_text(font, textColor, x + w / 2, y + (h - al_get_font_line_height(font)) / 2, ALLEGRO_ALIGN_CENTER, truncatedLabel.c_str());
    }

    void onClick() override {
        stonesenseState.ssState.selectedTab = tabIndex;  // Update selected tab
        Button::onClick();  // Call the base class onClick() to execute any extra behavior
    }
};


// Global list of GUI elements
std::vector<GUIElement*> elements;

void updateAll() {
    for (auto* elem : elements) {
        elem->update();
    }
}

// Handle mouse click events
void handleMouseClick(int mouseX, int mouseY) {
    for (auto* elem : elements) {
        if (elem->isMouseOver(mouseX, mouseY)) {
            elem->onClick();
            break;
        }
    }
}

// Handle mouse release events
void handleMouseRelease() {
    for (auto* elem : elements) {
        elem->onRelease();
    }
    stonesenseState.mouseHeld = false;
}

// Handle mouse movement for hover state
void handleMouseMove(int mouseX, int mouseY) {
    for (auto* elem : elements) {
        elem->updateHoverState(mouseX, mouseY);
    }
}

// Handle mouse wheel scrolling
void handleMouseWheel(int mouseX, int mouseY, int deltaY) {
    for (auto* elem : elements) {
        if (elem->isMouseOver(mouseX, mouseY)) {
            elem->onScroll(deltaY);
        }
    }
}

bool elementExists(int x, int y, int w, int h) {
    for (GUIElement* elem : elements) {
        if (elem->x == x && elem->y == y && elem->w == w && elem->h == h) {
            return true;  // Found an element with the same position and size
        }
    }
    return false;
}

void addButton(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor, OnClickCallback onClickCallback, std::unordered_set<std::string> visibleStates) {
    if (!elementExists(x, y, w, h)) {
        elements.push_back(new Button(x, y, w, h, borderColor, bgColor, onClickCallback, visibleStates));
    }
}

void addLabeledButton(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor,
    const std::string& label, ALLEGRO_FONT* font,
    OnClickCallback onClickCallback, std::unordered_set<std::string> visibleStates) {
    if (!elementExists(x, y, w, h)) {
        elements.push_back(new LabeledButton(x, y, w, h, borderColor, bgColor, label, font, onClickCallback, visibleStates));
    }
}


void addTab(int x, int y, int w, int h, int32_t borderColor, int32_t bgColor, int tabIndex, const std::string& label, ALLEGRO_FONT* font, OnClickCallback onClickCallback, std::unordered_set<std::string> visibleStates) {
    if (!elementExists(x, y, w, h)) {
        elements.push_back(new Tab(x, y, w, h, borderColor, bgColor, tabIndex, label, font, onClickCallback, visibleStates));
    }
}

void clearElements() {
    for (auto* element : elements) {
        delete element;  // Free allocated memory
    }
    elements.clear();  // Remove all pointers from the vector
}

namespace
{
    void draw_announcements(const ALLEGRO_FONT* font, float x, float y, int flags, std::vector<df::report*>& announcements)
    {
        auto& ssConfig = stonesenseState.ssConfig;

        const int numAnnouncements = (int)announcements.size();
        //const int maxAnnouncements = std::min(10, numAnnouncements);
        int line = 0;
        int offset = 0;

        int fontHeight = al_get_font_line_height(font);
        //int tabHeight = fontHeight + 10;  // Padding for visuals

        for (int i = numAnnouncements - 1; /*i >= (numAnnouncements - maxAnnouncements) && */announcements[i]->duration > 0; i--)
        {
            ALLEGRO_COLOR color = ssConfig.config.colors.getDfColor(announcements[i]->color, ssConfig.config.useDfColors);
            auto reportStr = DF2UTF(announcements[i]->text).c_str();
            auto splits = splitLinesToWidth(reportStr, font, stonesenseState.ssState.InfoW - 20);
            for (auto split : splits) {
                offset = (line * fontHeight);
                draw_text_border(font, color, x, y + offset, flags, split.c_str());
                line++;
            }
        }
    }
}

void draw_loading_message(const char *format, ...)
{
    al_clear_to_color(uiColor(dfColors::black));
    ALLEGRO_COLOR color = uiColor(dfColors::white);

    int flags = ALLEGRO_ALIGN_CENTRE;

    int x = al_get_bitmap_width(al_get_target_bitmap()) / 2;
    int y = al_get_bitmap_height(al_get_target_bitmap()) / 2;

    ALLEGRO_USTR *buf;
    va_list arglist;
    const char *s;

    /* Fast path for common case. */
    if (0 == strcmp(format, "%s")) {
        va_start(arglist, format);
        s = va_arg(arglist, const char *);
        draw_text_border(stonesenseState.font, color, x, y, flags, s);
        va_end(arglist);
    }
    else
    {
        va_start(arglist, format);
        buf = al_ustr_new("");
        al_ustr_vappendf(buf, format, arglist);
        va_end(arglist);
        draw_ustr_border(stonesenseState.font, color, x, y, flags, buf);
        al_ustr_free(buf);
    }
    al_flip_display();
}

void correctTileForDisplayedOffset(int32_t& x, int32_t& y, int32_t& z)
{
    auto& ssState = stonesenseState.ssState;

    x -= ssState.Position.x;
    y -= ssState.Position.y; //Position.y;
    z -= ssState.Position.z - 1; // + viewedSegment->sizez - 2; // loading one above the top of the displayed segment for tile rules
}

/**
 * Corrects the coordinate (x,y) for rotation in a region of size (szx, szy).
 */
void correctForRotation(int32_t& x, int32_t& y, unsigned char rot, int32_t szx, int32_t szy){
    int32_t oldx = x;
    int32_t oldy = y;

    if(rot == 1) {
        x = szy - oldy -1;
        y = oldx;
    }
    if(rot == 2) {
        x = szx - oldx -1;
        y = szy - oldy -1;
    }
    if(rot == 3) {
        x = oldy;
        y = szx - oldx -1;
    }
}

Crd2D WorldTileToScreen(int32_t x, int32_t y, int32_t z)
{
    correctTileForDisplayedOffset( x, y, z);
    return LocalTileToScreen(x, y, z-1);
}

Crd2D LocalTileToScreen(int32_t x, int32_t y, int32_t z)
{
    pointToScreen((int*)&x, (int*)&y, z);
    Crd2D result;
    result.x = x;
    result.y = y;
    return result;
}

void DrawCurrentLevelOutline(bool backPart)
{
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    int x = ssState.Position.x+1;
    int y = ssState.Position.y+1;
    int z = ssState.Position.z + ssState.Size.z - 1;
    int sizex = ssState.Size.x-2;
    int sizey = ssState.Size.y-2;

    if(ssConfig.hide_outer_tiles) {
        x++;
        y++;
        sizex -= 2;
        sizey -= 2;
    }

    Crd2D p1 = WorldTileToScreen(x, y, z);
    Crd2D p2 = WorldTileToScreen(x, y + sizey , z);
    Crd2D p3 = WorldTileToScreen(x + sizex , y, z);
    Crd2D p4 = WorldTileToScreen(x + sizex , y + sizey , z);
    p1.y += FLOORHEIGHT*ssConfig.scale;
    p2.y += FLOORHEIGHT*ssConfig.scale;
    p3.y += FLOORHEIGHT*ssConfig.scale;
    p4.y += FLOORHEIGHT*ssConfig.scale;
    if(backPart) {
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
        al_draw_line(p1.x, p1.y, p1.x, p1.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
        al_draw_line(p1.x, p1.y, p2.x, p2.y, uiColor(dfColors::black), 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
        al_draw_line(p2.x, p2.y, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);

        al_draw_line(p1.x, p1.y, p3.x, p3.y, uiColor(dfColors::black), 0);
        al_draw_line(p1.x, p1.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
        al_draw_line(p3.x, p3.y, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
    } else {
        al_draw_line(p4.x, p4.y, p4.x, p4.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
        al_draw_line(p4.x, p4.y, p2.x, p2.y, uiColor(dfColors::black) ,0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p2.x, p2.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black) ,0);

        al_draw_line(p4.x, p4.y, p3.x, p3.y, uiColor(dfColors::black), 0);
        al_draw_line(p4.x, p4.y-TILEHEIGHT*ssConfig.scale, p3.x, p3.y-TILEHEIGHT*ssConfig.scale, uiColor(dfColors::black), 0);
    }
}

namespace
{
    void drawCursorAt(WorldSegment* segment, Crd3D cursor, const ALLEGRO_COLOR& color)
    {
        auto& ssConfig = stonesenseState.ssConfig;
        segment->CorrectTileForSegmentOffset(cursor.x, cursor.y, cursor.z);
        segment->CorrectTileForSegmentRotation(cursor.x, cursor.y, cursor.z);

        Crd2D point = LocalTileToScreen(cursor.x, cursor.y, cursor.z);
        int sheetx = SPRITEOBJECT_CURSOR % SHEET_OBJECTSWIDE;
        int sheety = SPRITEOBJECT_CURSOR / SHEET_OBJECTSWIDE;
        al_draw_tinted_scaled_bitmap(
            stonesenseState.IMGObjectSheet,
            color,
            sheetx * SPRITEWIDTH,
            sheety * SPRITEHEIGHT,
            SPRITEWIDTH,
            SPRITEHEIGHT,
            point.x - ((SPRITEWIDTH / 2) * ssConfig.scale),
            point.y - (WALLHEIGHT)*ssConfig.scale,
            SPRITEWIDTH * ssConfig.scale,
            SPRITEHEIGHT * ssConfig.scale,
            0);
    }

    void drawSelectionCursor(WorldSegment* segment)
    {
        auto& ssConfig = stonesenseState.ssConfig;
        auto selection = segment->segState.dfSelection;
        if (selection && ssConfig.config.follow_DFcursor) {
            drawCursorAt(segment, *selection, uiColor(dfColors::lgreen));
        }
        else {
            return;
        }
    }

    void drawDebugCursor(WorldSegment* segment)
    {
        auto& cursor = segment->segState.dfCursor;
        if (cursor)
            drawCursorAt(segment, *cursor, uiColor(dfColors::yellow));
    }

    void drawAdvmodeMenuTalk(const ALLEGRO_FONT* font, int x, int y)
    {
        //df::adventure * menu = df::global::adventure;
        //if (!menu)
        //    return;
        //if (menu->talk_targets.size() == 0)
        //    return;
        //int line = menu->talk_targets.size() + 3;
        //draw_textf_border(font, ssConfig.config.colors.getDfColor(dfColors::white, ssConfig.config.useDfColors), x, (y - (line*al_get_font_line_height(font))), 0,
        //    "Who will you talk to?");
        //line -= 2;
        //for (int i = 0; i < menu->talk_targets.size(); i++)
        //{
        //    ALLEGRO_COLOR color = ssConfig.config.colors.getDfColor(dfColors::lgray, ssConfig.config.useDfColors);
        //    if (i == menu->talk_target_selection)
        //        color = ssConfig.config.colors.getDfColor(dfColors::white, ssConfig.config.useDfColors);
        //    df::unit * crete = Units::GetCreature(Units::FindIndexById(menu->talk_targets[i]->unit_id));
        //    if (crete)
        //    {
        //        ALLEGRO_USTR * string = al_ustr_newf("%s, ", Units::getProfessionName(crete).c_str());
        //        int8_t gender = df::global::world->raws.creatures.all[crete->race]->caste[crete->caste]->gender;
        //        if (gender == 0)
        //            al_ustr_append_chr(string, 0x2640);
        //        else if (gender == 1)
        //            al_ustr_append_chr(string, 0x2642);
        //        draw_ustr_border(font, color, x + 5, (y - ((line - i)*al_get_font_line_height(font))), 0,
        //            string);
        //    }
        //}
    }
    void drawDebugInfo(WorldSegment* segment)
    {
        using df::global::plotinfo;
        auto font = stonesenseState.font;
        auto fontHeight = al_get_font_line_height(font);
        auto& contentLoader = stonesenseState.contentLoader;

        if (!segment->segState.dfCursor)
        {
            draw_textf_border(font, uiColor(dfColors::white), 2, (10 * fontHeight), 0,
                "Coord: invalid");
            return;
        }

        //get tile info
        Tile* b = segment->getTile(
            segment->segState.dfCursor->x,
            segment->segState.dfCursor->y,
            segment->segState.dfCursor->z);
        int i = 10;
        if (b) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0, "Tile 0x%x (%i,%i,%i)", b, b->x, b->y, b->z);
        }

        draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
            "Coord:(%i,%i,%i)", segment->segState.dfCursor->x, segment->segState.dfCursor->y, segment->segState.dfCursor->z);

        if (!b) {
            return;
        }
        int ttype;
        const char* tform = NULL;
        using df::tiletype_shape_basic;

        draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
            "Tile: %s", DFHack::enum_item_key_str(b->tileType));
        if (b->tileShapeBasic() == tiletype_shape_basic::Floor) {
            ttype = b->tileType;
            tform = "floor";
        }
        else if (b->tileShapeBasic() == tiletype_shape_basic::Wall) {
            ttype = b->tileType;
            tform = "wall";
        }
        else if (b->tileShapeBasic() == tiletype_shape_basic::Ramp ||
            b->tileType == df::tiletype::RampTop) {
            ttype = b->tileType;
            tform = "ramp";
        }
        else if (b->tileShapeBasic() == tiletype_shape_basic::Stair) {
            ttype = b->tileType;
            tform = "stair";
        }
        else {
            ttype = -1;
        }

        draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
            "Game Mode:%i, Control Mode:%i", contentLoader->gameMode.g_mode, contentLoader->gameMode.g_type);
        if (tform != NULL && b->material.type != INVALID_INDEX) {
            const char* formName = lookupFormName(b->consForm);
            const char* matName = lookupMaterialTypeName(b->material.type);
            const char* subMatName = lookupMaterialName(b->material.type, b->material.index);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "%s %s:%i Material:%s%s%s (%d,%d)", formName, tform, ttype,
                matName ? matName : "Unknown", subMatName ? "/" : "", subMatName ? subMatName : "", b->material.type, b->material.index);
        }
        if (tform != NULL && b->material.type != INVALID_INDEX && b->material.index != INVALID_INDEX) {
            DFHack::MaterialInfo mat;
            mat.decode(b->material.type, b->material.index);
            if (mat.isValid())
            {
                ALLEGRO_COLOR color = al_map_rgb_f(contentLoader->Mats->color[mat.material->state_color[0]].red, contentLoader->Mats->color[mat.material->state_color[0]].green, contentLoader->Mats->color[mat.material->state_color[0]].blue);
                draw_textf_border(font, color, 2, (i++ * fontHeight), 0,
                    "%s", mat.material->state_name[0].c_str());
            }
        }
        if (tform != NULL) {
            const char* matName = lookupMaterialTypeName(b->layerMaterial.type);
            const char* subMatName = lookupMaterialName(b->layerMaterial.type, b->layerMaterial.index);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Layer Material:%s%s%s",
                matName ? matName : "Unknown", subMatName ? "/" : "", subMatName ? subMatName : "");
        }
        if ((tform != NULL) && b->hasVein == 1) {
            const char* matName = lookupMaterialTypeName(b->veinMaterial.type);
            const char* subMatName = lookupMaterialName(b->veinMaterial.type, b->veinMaterial.index);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Vein Material:%s%s%s",
                matName ? matName : "Unknown", subMatName ? "/" : "", subMatName ? subMatName : "");
        }
        if (tform != NULL) { //(b->grasslevel > 0)
            const char* subMatName = lookupMaterialName(WOOD, b->grassmat);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Grass length:%d, Material: %s",
                b->grasslevel, subMatName ? subMatName : "");
        }

        if (b->designation.bits.flow_size > 0 || b->tree.index != 0)
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "tree:%i water:%i,%i", b->tree.index, b->designation.bits.liquid_type, b->designation.bits.flow_size);
        if (b->tree.index != 0)
        {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "tree name:%s type:%i", lookupTreeName(b->tree.index), b->tree.type);
            auto & tree_tile = b->tree_tile;
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "tree tile:%s%s%s%s%s%s%s",
                b->tree_tile.bits.trunk ? " trunk" : "",
                tree_tile.bits.branch_w ? " >" : "",
                tree_tile.bits.branch_n ? " v" : "",
                tree_tile.bits.branch_e ? " <" : "",
                tree_tile.bits.branch_s ? " ^" : "",
                b->tree_tile.bits.branches ? " branches" : "",
                b->tree_tile.bits.leaves ? " leaves" : ""
            );
        }
        if (b->building.sprites.size() != 0)
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "%i extra sprites.", b->building.sprites.size());

        if (b->building.info && b->building.type != BUILDINGTYPE_NA && b->building.type != BUILDINGTYPE_BLACKBOX && b->building.type != BUILDINGTYPE_TREE) {
            const char* matName = lookupMaterialTypeName(b->building.info->material.type);
            const char* subMatName = lookupMaterialName(b->building.info->material.type, b->building.info->material.index);
            const char* subTypeName = lookupBuildingSubtype(b->building.type, b->building.info->subtype);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Building: game_type = %s(%i) game_subtype = %s(%i) Material: %s%s%s (%d,%d) Occupancy:%i, Special: %i ",
                ENUM_KEY_STR(building_type, (df::building_type)b->building.type).c_str(),
                b->building.type,
                subTypeName,
                b->building.info->subtype,
                matName ? matName : "Unknown", subMatName ? "/" : "", subMatName ? subMatName : "",
                b->building.info->material.type, b->building.info->material.index,
                b->occ.bits.building,
                b->building.special);
            for (size_t index = 0; index < b->building.constructed_mats.size(); index++) {
                const char* partMatName = lookupMaterialTypeName(b->building.constructed_mats[index].matt.type);
                const char* partSubMatName = lookupMaterialName(b->building.constructed_mats[index].matt.type, b->building.constructed_mats[index].matt.index);
                draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                    "Material[%i]: %s%s%s (%d,%d)",
                    index,
                    partMatName ? partMatName : "Unknown", partSubMatName ? "/" : "", partSubMatName ? partSubMatName : "",
                    b->building.constructed_mats[index].matt.type, b->building.constructed_mats[index].matt.index);
            }
        }

        if (b->designation.bits.traffic) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Traffic: %d", b->designation.bits.traffic);
        }
        if (b->designation.bits.pile) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Stockpile?");
        }
        if (b->designation.bits.water_table) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0, "Water table");
        }
        if (b->designation.bits.rained) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0, "Rained");
        }
        if (b->snowlevel || b->mudlevel || b->bloodlevel) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Snow: %d, Mud: %d, Blood: %d", b->snowlevel, b->mudlevel, b->bloodlevel);
        }
        if (b->Item.item.type >= 0) {
            DFHack::MaterialInfo mat;
            mat.decode(b->Item.matt.type, b->Item.matt.index);
            DFHack::ItemTypeInfo itemdef;
            bool subtype = itemdef.decode((df::item_type)b->Item.item.type, b->Item.item.index);
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "Item: %s - %s",
                mat.getToken().c_str(),
                subtype ? itemdef.getToken().c_str() : "");
        }

        const char* matName = lookupMaterialTypeName(b->tileeffect.matt.type);
        const char* subMatName = lookupMaterialName(b->tileeffect.matt.type, b->tileeffect.matt.index);
        std::string text{};
        switch (b->tileeffect.type) {
        case df::flow_type::Miasma: text = "Miasma"; break;
        case df::flow_type::Steam:  text = "Steam";  break;
        case df::flow_type::Mist:   text = "Mist";   break;
        case df::flow_type::MaterialDust: text = "MaterialDust"; break;
        case df::flow_type::MagmaMist: text = "MagmaMist"; break;
        case df::flow_type::Smoke: text = "Smoke"; break;
        case df::flow_type::Dragonfire: text = "Dragonfire"; break;
        case df::flow_type::Fire: text = "Fire"; break;
        case df::flow_type::Web: text = "Web"; break;
        case df::flow_type::MaterialGas: text = "MaterialGas"; break;
        case df::flow_type::MaterialVapor: text = "MaterialVapor"; break;
        case df::flow_type::OceanWave: text = "OceanWave"; break;
        case df::flow_type::SeaFoam: text = "SeaFoam"; break;
        case df::flow_type::ItemCloud:
            // TODO
            break;
        }

        if (!text.empty()) {
            draw_textf_border(font, uiColor(dfColors::white), 2, (i++ * fontHeight), 0,
                "%s: %d, Material:%s%s%s", text.c_str(),
                b->tileeffect.density, matName ? matName : "Unknown", subMatName ? "/" : "", subMatName ? subMatName : "");
        }
    }
}

void DrawMinimap(WorldSegment * segment)
{
    auto& ssState = stonesenseState.ssState;

    int size = 100;
    //double oneTileInPixels;
    int posx = ssState.ScreenW-size-10;
    int posy = 10;

    if(!segment || segment->segState.RegionDim.x == 0 || segment->segState.RegionDim.y == 0) {
        draw_textf_border(stonesenseState.font, uiColor(dfColors::white), posx, posy, 0, "No map loaded");
        return;
    }

    stonesenseState.oneTileInPixels = (double) size / segment->segState.RegionDim.x;
    //map outine
    int mapheight = (int)(segment->segState.RegionDim.y * stonesenseState.oneTileInPixels);
    al_draw_rectangle(posx, posy, posx+size, posy+mapheight, uiColor(dfColors::black),0);
    //current segment outline
    int x = (size * (segment->segState.Position.x+1)) / segment->segState.RegionDim.x;
    int y = (mapheight * (segment->segState.Position.y+1)) / segment->segState.RegionDim.y;
    stonesenseState.MiniMapSegmentWidth = (segment->segState.Size.x-2) * stonesenseState.oneTileInPixels;
    stonesenseState.MiniMapSegmentHeight = (segment->segState.Size.y-2) * stonesenseState.oneTileInPixels;
    al_draw_rectangle(posx+x, posy+y, posx+x+ stonesenseState.MiniMapSegmentWidth, posy+y+ stonesenseState.MiniMapSegmentHeight,uiColor(dfColors::black),0);
    stonesenseState.MiniMapTopLeftX = posx;
    stonesenseState.MiniMapTopLeftY = posy;
    stonesenseState.MiniMapBottomRightX = posx+size;
    stonesenseState.MiniMapBottomRightY = posy+mapheight;
}

ALLEGRO_BITMAP * CreateSpriteFromSheet( int spriteNum, ALLEGRO_BITMAP* spriteSheet)
{
    int sheetx = spriteNum % SHEET_OBJECTSWIDE;
    int sheety = spriteNum / SHEET_OBJECTSWIDE;
    return al_create_sub_bitmap(spriteSheet, sheetx * SPRITEWIDTH, sheety * SPRITEHEIGHT, SPRITEWIDTH, SPRITEHEIGHT);
}

void drawTab(ALLEGRO_FONT* font, const std::string& label, int tabIndex, OnClickCallback onClickCallback) {
    int numTabs = 2;
    int tabWidth = stonesenseState.ssState.InfoW / numTabs;
    int fontHeight = al_get_font_line_height(font);
    int tabHeight = fontHeight + 10;  // Padding for visuals
    int panelX = stonesenseState.ssState.ScreenW - stonesenseState.ssState.InfoW;  // Left edge of info panel
    int tabX = panelX + (tabIndex * tabWidth);  // Position inside the panel
    int tabY = stonesenseState.ssState.ScreenH - tabHeight;  // Flush with bottom

    // Truncate label to fit inside tab
    std::string truncatedLabel = fitTextToWidth(label, font, tabWidth - 20);

    addTab(tabX, tabY, tabWidth, tabHeight, dfColors::yellow, dfColors::black, tabIndex, truncatedLabel, font, onClickCallback, { "INFO_PANEL", "INFO_PANEL/ANNOUNCEMENTS","INFO_PANEL/KEYBINDS" });
}


void drawInfoPanel() {
    auto font = stonesenseState.font;
    auto fontHeight = al_get_font_line_height(font);
    int halftabHeight = (fontHeight + 10)/2;

    //draw panel
    al_draw_filled_rectangle(stonesenseState.ssState.ScreenW, 0, stonesenseState.ssState.ScreenW - stonesenseState.ssState.InfoW, stonesenseState.ssState.ScreenH - halftabHeight, uiColor(dfColors::black));
    al_draw_rectangle(stonesenseState.ssState.ScreenW, 2, stonesenseState.ssState.ScreenW - stonesenseState.ssState.InfoW, stonesenseState.ssState.ScreenH - halftabHeight, uiColor(dfColors::yellow), 2);

    //draw tabs
    drawTab(font, "Announcements", GameState::tabs::announcements, action_toggleannouncements);
    drawTab(font, "Keybinds", GameState::tabs::keybinds, action_togglekeybinds);
}


void DrawSpriteIndexOverlay(int imageIndex)
{
    auto currentImage = IMGFileList.lookup(imageIndex);
    auto font = stonesenseState.font;
    auto fontHeight = al_get_font_line_height(font);
    auto& ssState = stonesenseState.ssState;

    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_draw_bitmap(currentImage,0,0,0);
    for(int i =0; i<= 20*SPRITEWIDTH; i+=SPRITEWIDTH) {
        al_draw_line(i,0,i, ssState.ScreenH, uiColor(dfColors::black), 0);
    }
    for(int i =0; i< ssState.ScreenH; i+=SPRITEHEIGHT) {
        al_draw_line(0,i, 20*SPRITEWIDTH,i,uiColor(dfColors::black), 0);
    }

    for(int y = 0; y<20; y++) {
        for(int x = 0; x<20; x+=5) {
            int index = y * 20 + x;
            draw_textf_border(font, uiColor(dfColors::white),  x*SPRITEWIDTH+5, y* SPRITEHEIGHT+(fontHeight/2), 0, "%i", index);
        }
    }
    draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW-10, ssState.ScreenH -fontHeight, ALLEGRO_ALIGN_RIGHT,
                      "%s (%d) (Press SPACE to return)",
        (imageIndex == -1 ? "objects.png" : IMGFileList.getFilename(imageIndex).string().c_str()), imageIndex);
    al_flip_display();
}

void DoSpriteIndexOverlay()
{
    DrawSpriteIndexOverlay(-1);
    int index = 0;
    int max = IMGFileList.size();
    while(true) {
        while(!al_key_down(&stonesenseState.keyboard,ALLEGRO_KEY_SPACE) && !al_key_down(&stonesenseState.keyboard,ALLEGRO_KEY_F10)) {
            al_get_keyboard_state(&stonesenseState.keyboard);
            al_rest(ALLEGRO_MSECS_TO_SECS(50));
        }
        al_get_keyboard_state(&stonesenseState.keyboard);
        if (al_key_down(&stonesenseState.keyboard,ALLEGRO_KEY_SPACE)) {
            break;
        }
        DrawSpriteIndexOverlay(index);
        index++;
        if (index >= max) {
            index = -1;
        }
        //debounce f10
        al_get_keyboard_state(&stonesenseState.keyboard);
        while(al_key_down(&stonesenseState.keyboard,ALLEGRO_KEY_F10)) {
            al_get_keyboard_state(&stonesenseState.keyboard);
            al_rest(ALLEGRO_MSECS_TO_SECS(50));
        }
    }
    //redraw screen again
    al_clear_to_color(stonesenseState.ssConfig.config.backcol);
    paintboard();
}

float clockToMs(float clockTicks) {
    return clockTicks / (CLOCKS_PER_SEC/1000);
}

void paintboard()
{
    DFHack::CoreSuspender suspend;

    auto font = stonesenseState.font;
    auto fontHeight = al_get_font_line_height(font);
    auto& ssConfig = stonesenseState.ssConfig;
    auto& ssState = stonesenseState.ssState;

    //do the starting timer stuff
    clock_t starttime = clock();

    int op, src, dst, alpha_op, alpha_src, alpha_dst;
    al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
    al_set_separate_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO,ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    if(ssConfig.config.transparentScreenshots) {
        al_clear_to_color(al_map_rgba(0,0,0,0));
    } else {
        al_clear_to_color(ssConfig.config.backcol);
    }
    al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);

    // lock segment for painting and retrieve it.
    stonesenseState.map_segment.lockDraw();
    WorldSegment * segment = stonesenseState.map_segment.getDraw();
    if( segment == NULL ) {
        draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "Could not find DF process");
        stonesenseState.map_segment.unlockDraw();
        return;
    }

    if (df::global::plotinfo->follow_unit != -1) {
        if (DFHack::Screen::inGraphicsMode()) {
            auto zoom = stonesenseState.ssConfig.zoom;
            auto xOff = int(-0.134 * std::pow(zoom, 2) + 2.911 * zoom - 21.214);
            stonesenseState.ssConfig.config.viewOffset = { xOff,0,0 };
        }
        else {
            stonesenseState.ssConfig.config.viewOffset = { -46,0,0 };
        }
    }


    segment->DrawAllTiles();

    if (ssConfig.config.show_osd) {
        DrawCurrentLevelOutline(false);
    }

    //do the closing timer stuff
    clock_t donetime = clock();
    stonesenseState.stoneSenseTimers.draw_time.update(donetime - starttime);
    stonesenseState.stoneSenseTimers.frame_total.update(donetime - stonesenseState.stoneSenseTimers.prev_frame_time);
    stonesenseState.stoneSenseTimers.prev_frame_time = donetime;

    stonesenseState.UIState = "DEFAULT";

    auto buttonW = 30;
    auto buttonH = buttonW;
    addLabeledButton(stonesenseState.ssState.ScreenW - buttonW, 0, buttonW, buttonH, dfColors::yellow, dfColors::blue, "i", stonesenseState.font, action_toggleinfopanel, { "DEFAULT" });
    addLabeledButton(stonesenseState.ssState.ScreenW - buttonW - stonesenseState.ssState.InfoW, 0, buttonW, buttonH, dfColors::yellow, dfColors::red, "x", stonesenseState.font, action_toggleinfopanel, { "INFO_PANEL","INFO_PANEL/ANNOUNCEMENTS","INFO_PANEL/KEYBINDS" });
    if (ssConfig.config.show_info_panel) {
        stonesenseState.UIState = "INFO_PANEL";
        drawInfoPanel();
    }
    if (stonesenseState.ssState.selectedTab == GameState::tabs::announcements) {
        stonesenseState.UIState = "INFO_PANEL/ANNOUNCEMENTS";
        al_hold_bitmap_drawing(true);
        draw_announcements(font, ssState.ScreenW - 10, 10/*ssState.ScreenH - 10 - al_get_font_line_height(font)*/, ALLEGRO_ALIGN_RIGHT, df::global::world->status.announcements);
        al_hold_bitmap_drawing(false);
    }
    if(stonesenseState.ssState.selectedTab == GameState::tabs::keybinds){
        stonesenseState.UIState = "INFO_PANEL/KEYBINDS";
        std::string *keyname, *actionname;
        keyname = actionname = NULL;
        int line = 1;
        al_hold_bitmap_drawing(true);

        for(int32_t i=1; true; i++){
            if(getKeyStrings(i, keyname, actionname)){
                std::ostringstream oss;
                oss << *keyname << ": " << *actionname;
                if (isRepeatable(i)) {
                    oss << " (repeats)";
                }
                std::string keyStr = oss.str();

                int maxWidth = std::max(0, stonesenseState.ssState.InfoW - 20);
                std::vector<std::string> splitLines = splitLinesToWidth(keyStr, font, maxWidth);

                for (const auto& lineText : splitLines) {
                    draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW - 10, line * fontHeight, ALLEGRO_ALIGN_RIGHT, lineText.c_str());
                    line++;
                }


            }
            if(keyname == NULL) {
                break;
            }
        }
        al_hold_bitmap_drawing(false);
    } else if (ssConfig.config.show_osd) {
        stonesenseState.UIState = "OSD";
        al_hold_bitmap_drawing(true);

        draw_textf_border(font, uiColor(dfColors::white), 10,fontHeight, 0, "%i,%i,%i, r%i, z%i", ssState.Position.x,ssState.Position.y,ssState.Position.z, ssState.Rotation, ssConfig.zoom);

        drawSelectionCursor(segment);

        drawDebugCursor(segment);

        drawAdvmodeMenuTalk(font, 5, ssState.ScreenH - 5);

        if(ssConfig.config.debug_mode) {
            stonesenseState.UIState = "DEBUG";
            auto& contentLoader = stonesenseState.contentLoader;
            draw_textf_border(font, uiColor(dfColors::white), 10, 3*fontHeight, 0, "Map Read Time: %.2fms", clockToMs(stonesenseState.stoneSenseTimers.read_time));
            draw_textf_border(font, uiColor(dfColors::white), 10, 4*fontHeight, 0, "Map Beautification Time: %.2fms", clockToMs(stonesenseState.stoneSenseTimers.beautify_time));
            draw_textf_border(font, uiColor(dfColors::white), 10, 5*fontHeight, 0, "Tile Sprite Assembly Time: %.2fms", clockToMs(stonesenseState.stoneSenseTimers.assembly_time));
            draw_textf_border(font, uiColor(dfColors::white), 10, 6*fontHeight, 0, "DF Renderer Overlay Time: %.2fms", clockToMs(stonesenseState.stoneSenseTimers.overlay_time));
            draw_textf_border(font, uiColor(dfColors::white), 10, 2*fontHeight, 0, "FPS: %.2f", 1000.0/clockToMs(stonesenseState.stoneSenseTimers.frame_total));
            draw_textf_border(font, uiColor(dfColors::white), 10, 7*fontHeight, 0, "Draw: %.2fms", clockToMs(stonesenseState.stoneSenseTimers.draw_time));
            draw_textf_border(font, uiColor(dfColors::white), 10, 9*fontHeight, 0, "%i/%i/%i, %i:%i", contentLoader->currentDay+1, contentLoader->currentMonth+1, contentLoader->currentYear, contentLoader->currentHour, (contentLoader->currentTickRel*60)/50);

            drawDebugInfo(segment);
        }
        ssConfig.platecount = 0;
        int top = 0;
        if(ssConfig.config.track_mode != Config::TRACKING_NONE) {
            top += fontHeight;
            draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Locked on DF screen + (%d,%d,%d)",ssConfig.config.viewOffset.x,ssConfig.config.viewOffset.y,ssConfig.config.viewOffset.z);
        }
        if(ssConfig.config.follow_DFcursor && ssConfig.config.debug_mode) {
            top += fontHeight;
            if(segment->segState.dfCursor) {
                draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Following DF Cursor at: %d,%d,%d", segment->segState.dfCursor->x,segment->segState.dfCursor->y,segment->segState.dfCursor->z);
            }
        }
        if(ssConfig.single_layer_view) {
            top += fontHeight;
            draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Single layer view");
        }
        if(ssConfig.config.automatic_reload_time) {
            top += fontHeight;
            draw_textf_border(font, uiColor(dfColors::white), ssState.ScreenW/2,top, ALLEGRO_ALIGN_CENTRE, "Reloading every %0.1fs", (float)ssConfig.config.automatic_reload_time/1000);
        }

        al_hold_bitmap_drawing(false);
        if (!ssConfig.config.show_info_panel) {
            DrawMinimap(segment);
        }
    }

    al_hold_bitmap_drawing(true);
    updateAll();
    al_hold_bitmap_drawing(false);

    stonesenseState.map_segment.unlockDraw();
}


bool load_from_path (const std::filesystem::path p, const std::string& filename, ALLEGRO_BITMAP *& imgd)
{
    int index;
    index = loadImgFile(p / filename);
    if(index == -1) {
        return false;
    }
    auto bmp = IMGFileList.getBitmap(index);
    imgd = al_create_sub_bitmap(bmp, 0, 0,
        al_get_bitmap_width(bmp),
        al_get_bitmap_height(bmp));
    return true;
}

namespace {
    struct Img {
        std::string name;
        ALLEGRO_BITMAP*& bPtr;
    };

    const std::array img_list{
        Img{ "objects.png", stonesenseState.IMGObjectSheet },
        Img{ "creatures.png", stonesenseState.IMGCreatureSheet },
        Img{ "ramps.png", stonesenseState.IMGRampSheet },
        Img{ "SSStatusIcons.png", stonesenseState.IMGStatusSheet },
        Img{ "SSProfIcons.png", stonesenseState.IMGProfSheet },
        Img{ "gibs.png", stonesenseState.IMGBloodSheet },
        Img{ "engravings_floor.png", stonesenseState.IMGEngFloorSheet },
        Img{ "engravings_left.png", stonesenseState.IMGEngLeftSheet },
        Img{ "engravings_right.png", stonesenseState.IMGEngRightSheet },
        Img{ "Sir_Henry_s_32x32.png", stonesenseState.IMGLetterSheet }
    };
}

void loadGraphicsFromDisk()
{
    std::filesystem::path path{ "stonesense" };
    for (auto& img : img_list)
    {
        if (!load_from_path(path, img.name, img.bPtr))
            return;
    }
    createEffectSprites();
}

//delete and clean out the image files
void flushImgFiles()
{
    LogVerbose("flushing images...\n");
    destroyEffectSprites();

    //should be OK because we keep others from directly acccessing this stuff
    for (auto& img : img_list)
        if (img.bPtr != nullptr)
        {
            al_destroy_bitmap(img.bPtr);
            img.bPtr = nullptr;
        }

    IMGFileList.flush();
    IMGCache.flush();

}

ALLEGRO_BITMAP* getImgFile(int index)
{
    return IMGFileList.getBitmap(index);
}

int loadImgFile(std::filesystem::path filename)
{
    auto& ssConfig = stonesenseState.ssConfig;

    if (IMGFileList.contains(filename))
    {
        return IMGFileList.lookup(filename);
    }

    ALLEGRO_BITMAP* tempfile = load_bitmap_withWarning(filename);

    if (ssConfig.config.cache_images) {
        static bool foundSize = false;
        if (!foundSize) {
            ALLEGRO_BITMAP* test = 0;
            while (true) {
                test = al_create_bitmap(ssConfig.config.imageCacheSize, ssConfig.config.imageCacheSize);
                if (test) {
                    LogError("%i works.\n", ssConfig.config.imageCacheSize);
                    break;
                }
                LogError("%i is too large. chopping it.\n", ssConfig.config.imageCacheSize);
                ssConfig.config.imageCacheSize = ssConfig.config.imageCacheSize / 2;
            }
            foundSize = true;
            al_destroy_bitmap(test);
        }

        int op, src, dst, alpha_op, alpha_src, alpha_dst;
        al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);

        static int xOffset = 0;
        static int yOffset = 0;
        int currentCache = IMGCache.size() -1;
        static int columnWidth = 0;
        if(!tempfile) {
            return -1;
        }
        LogVerbose("New image: %s\n", filename.string().c_str());
        if (currentCache < 0) {
            currentCache = IMGCache.extend();
            LogVerbose("Creating image cache #%d\n",currentCache);
        }
        if((yOffset + al_get_bitmap_height(tempfile)) <= ssConfig.config.imageCacheSize) {
            // nothing
        } else if ((xOffset + al_get_bitmap_width(tempfile) + columnWidth) <= ssConfig.config.imageCacheSize) {
            yOffset = 0;
            xOffset += columnWidth;
            columnWidth = 0;
        } else {
            yOffset = 0;
            xOffset = 0;
            currentCache = IMGCache.extend();
            LogVerbose("Creating image cache #%d\n",currentCache);
        }

        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
        al_set_target_bitmap(IMGCache.get(currentCache));
        al_draw_bitmap(tempfile, xOffset, yOffset, 0);
        IMGFileList.add(filename, IMGCache.get(currentCache), xOffset, yOffset, tempfile);
        yOffset += al_get_bitmap_height(tempfile);
        columnWidth = std::max(columnWidth, al_get_bitmap_width(tempfile));

        if(ssConfig.config.saveImageCache) {
            saveImage(tempfile);
        }
        al_destroy_bitmap(tempfile);
        al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
        al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
        if(ssConfig.config.saveImageCache) {
            saveImage(IMGCache.get(currentCache));
        }
        al_clear_to_color(al_map_rgb(0,0,0));
        al_flip_display();
        return (int)IMGFileList.size() - 1;
    }
    else
    {
        if(!tempfile) {
            return -1;
        }
        IMGFileList.add(filename, tempfile);
        LogVerbose("New image: %s\n", filename.string().c_str());
        return (int)IMGFileList.size() - 1;
    }
}

namespace
{
    std::filesystem::path getAvailableFilename(std::string prefix, std::string suffix = ".png")
    {
        //get filename
        int index = 1;
        //search for the first screenshot# that does not exist already
        while (true) {
            std::stringstream buf{};
            buf << prefix << index << suffix;
            std::filesystem::path filename{ buf.str() };
            if (!std::filesystem::exists(filename))
                return filename;
            index++;
        }
    }
}

void saveScreenshot()
{
    al_clear_to_color(stonesenseState.ssConfig.config.backcol);
    paintboard();
    std::filesystem::path filename = getAvailableFilename("screenshot");
    auto& ssState = stonesenseState.ssState;

    ALLEGRO_BITMAP* temp = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);
    al_set_target_bitmap(temp);
    PrintMessage("saving screenshot to %s\n", filename.string().c_str());
    auto& ssConfig = stonesenseState.ssConfig;
    if(!ssConfig.config.transparentScreenshots) {
        al_clear_to_color(ssConfig.config.backcol);
    }
    paintboard();
    al_save_bitmap(filename.string().c_str(), temp);
    al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
    al_destroy_bitmap(temp);
    //al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY);
}

void saveImage(ALLEGRO_BITMAP* image)
{
    std::filesystem::path filename = getAvailableFilename("Image");
    al_save_bitmap(filename.string().c_str(), image);
}

void saveMegashot(bool tall)
{
    stonesenseState.map_segment.lockRead();

    auto& ssState = stonesenseState.ssState;
    draw_textf_border(stonesenseState.font, uiColor(dfColors::white), ssState.ScreenW/2, ssState.ScreenH/2, ALLEGRO_ALIGN_CENTRE, "saving large screenshot...");
    al_flip_display();
    std::filesystem::path filename = getAvailableFilename("screenshot");
    int timer = clock();
    //back up all the relevant values
    auto& ssConfig = stonesenseState.ssConfig;
    GameConfiguration tempConfig = ssConfig;
    GameState tempState = ssState;
    uint32_t templiftX = stonesenseState.lift_segment_offscreen_x;
    uint32_t templiftY = stonesenseState.lift_segment_offscreen_y;
    int tempflags = al_get_new_bitmap_flags();

    //now make them real big.
    ssConfig.config.show_osd = false;
    ssConfig.config.track_mode = Config::TRACKING_NONE;
    ssConfig.config.fogenable = false;
    ssConfig.track_screen_center = false;

    // Disable due to minor visual artifacts
    ssConfig.config.extrude_tiles = false;
    // Ensure the scale is exactly 1
    ssConfig.config.pixelperfect_zoom = true;
    ssConfig.zoom = 1;
    ssConfig.recalculateScale();

    //make the image
    ssState.ScreenW = ((ssState.RegionDim.x + ssState.RegionDim.y) * TILEWIDTH / 2)*ssConfig.scale;
    if(tall) {
        ssState.ScreenH = ( ((ssState.RegionDim.x + ssState.RegionDim.y) * TILETOPHEIGHT / 2) + (ssState.RegionDim.z * TILEHEIGHT) )*ssConfig.scale;
    } else {
        ssState.ScreenH = ( ((ssState.RegionDim.x + ssState.RegionDim.y) * TILETOPHEIGHT / 2) + ((ssState.Size.z - 1) * TILEHEIGHT) )*ssConfig.scale;
    }

    bigFile = al_create_bitmap(ssState.ScreenW, ssState.ScreenH);

    //draw and save the image
    if(bigFile) {
        PrintMessage("saving large screenshot to %s\n", filename.string().c_str());
        al_set_target_bitmap(bigFile);
        if(!ssConfig.config.transparentScreenshots) {
            al_clear_to_color(ssConfig.config.backcol);
        }

        //zero out the segment lift
        int startlifty, startliftx;
        startlifty = 0;
        //realign the image if the region is rectangular
        if ((ssState.Rotation & 1) == 0)
        {
            startliftx = (TILEWIDTH/2)*ssState.RegionDim.y;
        }
        else
        {
            startliftx = (TILEWIDTH/2)*ssState.RegionDim.x;
        }
        stonesenseState.lift_segment_offscreen_y = startlifty;
        stonesenseState.lift_segment_offscreen_x = startliftx;

        //here we deal with the rotations
        int startx, incrx, numx;
        int starty, incry, numy;
        int sizex, sizey;
        int numz;

        startx = -1;
        starty = -1;
        sizex = ssState.Size.x-2;
        sizey = ssState.Size.y-2;
        if ((ssState.Rotation & 1) == 0)
        {
            incrx = sizex;
            incry = sizey;
            numx = (int)(ssState.RegionDim.x+3);
            numy = (int)(ssState.RegionDim.y+3);
        }
        else
        {
            incrx = sizey;
            incry = sizex;
            numx = (int)(ssState.RegionDim.y+3);
            numy = (int)(ssState.RegionDim.x+3);
        }
        numx = numx/incrx + (numx%incrx==0 ? 0 : 1);
        numy = numy/incry + (numx%incry==0 ? 0 : 1);
        numz = tall ? ((ssState.RegionDim.z/(ssState.Size.z-1)) + 1) : 1;


        if(ssState.Rotation == 1 || ssState.Rotation == 2) {
            starty = (int)ssState.RegionDim.y - incry - 1;
            ssState.Position.y = (int)ssState.RegionDim.y - incry + 1;
            incry = -incry;
        } else {
            ssState.Position.y = -1;
        }

        if(ssState.Rotation == 3 || ssState.Rotation == 2) {
            startx = (int)ssState.RegionDim.x - incrx - 1;
            ssState.Position.x = (int)ssState.RegionDim.x - incrx + 1;
            incrx = -incrx;
        } else {
            ssState.Position.x = -1;
        }
        if(tall) {
            ssState.Position.z = ssState.RegionDim.z;
        }
        ssState.Position.x = startx;
        ssState.Position.y = starty;
        ssState.Position.z = tall ? 0 : ssState.Position.z;

        //set up the pixel-shifts
        int32_t movexx, moveyx, movexy, moveyy;
        if(ssState.Rotation == 1 || ssState.Rotation == 3) {
            movexx = -sizey;
            moveyx = sizey;

            movexy = -sizex;
            moveyy = sizex;
        } else {
            movexx = sizex;
            moveyx = sizex;

            movexy = sizey;
            moveyy = sizey;
        }

        //now actually loop through and draw the subsegments
        int32_t startstartlifty = startlifty;
        for(int k=0; k<numz; k++) {
            startlifty = startstartlifty - TILEHEIGHT*(numz-k-1)*(ssState.Size.z - 1);
            for(int i=0; i<numy; i++) {
                stonesenseState.lift_segment_offscreen_x = startliftx - (TILEWIDTH/2)*i*movexy;
                stonesenseState.lift_segment_offscreen_y = startlifty - (TILETOPHEIGHT/2)*i*moveyy;
                for(int j=0; j<numx; j++) {
                    //read and draw each individual segment
                    read_segment(NULL);
                    stonesenseState.map_segment.lockDraw();
                    WorldSegment * segment = stonesenseState.map_segment.getDraw();
                    segment->DrawAllTiles();
                    stonesenseState.map_segment.unlockDraw();

                    ssState.Position.x += incrx;
                    stonesenseState.lift_segment_offscreen_x += (TILEWIDTH/2)*movexx;
                    stonesenseState.lift_segment_offscreen_y -= (TILETOPHEIGHT/2)*moveyx;
                }
                ssState.Position.x = startx;
                ssState.Position.y += incry;
            }
            ssState.Position.x=startx;
            ssState.Position.y=starty;
            ssState.Position.z += ssState.Size.z - 1;
        }


        al_save_bitmap(filename.string().c_str(), bigFile);
        al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
        timer = clock() - timer;
        PrintMessage("\tcreating screenshot took %.2fms\n", clockToMs(timer));
        PrintMessage("\tlarge screenshot complete\n");
    } else {
        LogError("failed to take large screenshot; try zooming out\n");
    }
    al_destroy_bitmap(bigFile);
    //restore everything that we changed.
    stonesenseState.lift_segment_offscreen_x = templiftX;
    stonesenseState.lift_segment_offscreen_y = templiftY;
    ssConfig = tempConfig;
    ssState = tempState;
    al_set_new_bitmap_flags(tempflags);

    al_flip_display();
    stonesenseState.map_segment.unlockRead();
}

ALLEGRO_COLOR morph_color(ALLEGRO_COLOR source, ALLEGRO_COLOR reference, ALLEGRO_COLOR target)
{
    float sH, sS, sL, rH, rS, rL, tH, tS, tL;
    al_color_rgb_to_hsv(source.r, source.g, source.b, &sH, &sS, &sL);
    al_color_rgb_to_hsv(reference.r, reference.g, reference.b, &rH, &rS, &rL);
    al_color_rgb_to_hsv(target.r, target.g, target.b, &tH, &tS, &tL);

    sH += tH - rH;
    sS += tS - rS;
    sL += tL - rL;
    if (sH > 360.0f)
        sH -= 360.0f;
    if (sH < 0.0f)
        sH += 360.0f;
    if (sS > 1.0f)
        sS = 1.0f;
    if (sS < 0.0f)
        sS = 0.0f;
    if (sL > 1.0f)
        sL = 1.0f;
    if (sL < 0.0f)
        sL = 0.0f;

    return al_color_hsv(sH, sS, sL);
}
