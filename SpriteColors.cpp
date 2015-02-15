#include "SpriteColors.h"
#include "common.h"
#include "GUI.h"
#include "SpriteMaps.h"
#include "GameBuildings.h"
#include "Creatures.h"
#include "WorldSegment.h"
#include "ContentLoader.h"

ALLEGRO_COLOR operator*(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2)
{
    ALLEGRO_COLOR temp;
    temp.r=color1.r*color2.r;
    temp.g=color1.g*color2.g;
    temp.b=color1.b*color2.b;
    temp.a=color1.a*color2.a;
    return temp;
}

ALLEGRO_COLOR operator+(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2)
{
    ALLEGRO_COLOR temp;
    temp.r=color1.r+(color2.r*(1-color1.r));
    temp.g=color1.g+(color2.g*(1-color1.g));
    temp.b=color1.b+(color2.b*(1-color1.b));
    temp.a=color1.a+(color2.a*(1-color1.a));
    return temp;
}

/*
int getJobColor(unsigned char job)
{
    switch (job) {
    case 0:
        return 7;
    case 1:
    case 2:
    case 3:
    case 4:
        return 14;
    case 5:
    case 6:
    case 7:
        return 15;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        return 2;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        return 8;
    case 20:
    case 21:
    case 22:
        return 10;
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
        return 9;
    case 32:
    case 33:
    case 34:
    case 35:
        return 1;
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
        return 6;
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
        return 12;
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
        return 5;
    case 68:
        return 5;
    case 69:
        return 13;
    case 70:
        return 6;
    case 71:
        return 14;
    case 72:
        return 2;
    case 73:
        return 10;
    case 74:
        return 7;
    case 75:
        return 15;
    case 76:
        return 1;
    case 77:
        return 9;
    case 78:
        return 3;
    case 79:
        return 11;
    case 80:
        return 4;
    case 81:
        return 12;
    case 82:
        return 6;
    case 83:
        return 14;
    case 84:
        return 2;
    case 85:
        return 10;
    case 86:
        return 2;
    case 87:
        return 10;
    case 88:
    case 89:
        return 8;
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
        return 3;
    case 96:
    case 97:
        return 4;
    case 98:
        return 3;
    }
    return 3;
}
*/

ALLEGRO_COLOR premultiply(ALLEGRO_COLOR input)
{
    ALLEGRO_COLOR out;
    out.a = input.a;
    out.r = input.r * input.a;
    out.g = input.g * input.a;
    out.b = input.b * input.a;
    return out;
}

ALLEGRO_COLOR shadeAdventureMode(ALLEGRO_COLOR color, bool foggy, bool outside)
{
    if(!contentLoader->gameMode.g_mode == GAMEMODE_ADVENTURE) {
        return color;
    }

    if(foggy && ssConfig.fog_of_war) {
        color.r *= 0.25f;
        color.g *= 0.25f;
        color.b *= 0.25f;
    }

    if(ssConfig.dayNightCycle) {
        if(outside) {
            color = color*getDayShade(contentLoader->currentHour, contentLoader->currentTickRel);
        } else {
            color.r *= 0.5f;
            color.g *= 0.5f;
            color.b *= 0.5f;
        }
    }

    return color;
}

ALLEGRO_COLOR partialBlend(const ALLEGRO_COLOR & color2, const ALLEGRO_COLOR & color1, int percent)
{
    float blend = percent/100.0;
    ALLEGRO_COLOR result;
    result.r=(blend*color1.r)+((1.0-blend)*color2.r);
    result.g=(blend*color1.g)+((1.0-blend)*color2.g);
    result.b=(blend*color1.b)+((1.0-blend)*color2.b);
    if(color1.a > color2.a) {
        result.a = color1.a;
    } else {
        result.a = color2.a;
    }
    return result;
}

ALLEGRO_COLOR getDayShade(int hour, int tick)
{
    ALLEGRO_COLOR nightShade = al_map_rgb(158,155,255);
    ALLEGRO_COLOR dawnShade = al_map_rgb(254,172,142);

    if(hour < 6) {
        return nightShade;
    } else if((hour < 7) && (tick < 25)) {
        return partialBlend(nightShade, dawnShade, (tick * 4));
    } else if(hour < 7) {
        return partialBlend(dawnShade, al_map_rgb(255,255,255), ((tick-25) * 4));
    } else if((hour > 20) && (hour <= 21) && (tick < 25)) {
        return partialBlend(al_map_rgb(255,255,255), dawnShade, (tick * 4));
    } else if((hour > 20) && (hour <= 21)) {
        return partialBlend(dawnShade, nightShade, ((tick-25) * 4));
    } else if(hour > 21) {
        return nightShade;
    }
    return al_map_rgb(255,255,255);
}

ALLEGRO_COLOR blink(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
    if((currentAnimationFrame>>2)&0x01) {
        return c2;
    }
    return c1;
}

ALLEGRO_COLOR blinkTechnicolor()
{
    switch(currentAnimationFrame & 0x07){
    case 0x00:
        //yellow;
        return ssConfig.colors.getDfColor(dfColors::yellow, ssConfig.useDfColors);
    case 0x01:
        //blue
        return ssConfig.colors.getDfColor(dfColors::blue, ssConfig.useDfColors);
    case 0x02:
        //red
        return ssConfig.colors.getDfColor(dfColors::red, ssConfig.useDfColors);
    case 0x03:
        //magenta
        return ssConfig.colors.getDfColor(dfColors::magenta, ssConfig.useDfColors);
    case 0x04:
        //lred
        return ssConfig.colors.getDfColor(dfColors::lred, ssConfig.useDfColors);
    case 0x05:
        //green
        return ssConfig.colors.getDfColor(dfColors::green, ssConfig.useDfColors);
    case 0x06:
        //brown
        return ssConfig.colors.getDfColor(dfColors::brown, ssConfig.useDfColors);
    default:
        //lime
        return ssConfig.colors.getDfColor(dfColors::lgreen, ssConfig.useDfColors);
    }
}

// Meant to unify the different UI coloring techniques used.
ALLEGRO_COLOR uiColor(int32_t index)
{
    switch(index)
    {
    case 0:
        //black;
        return ssConfig.colors.getDfColor(dfColors::black, ssConfig.useDfColors);
    case 1:
        //white
        return ssConfig.colors.getDfColor(dfColors::white, ssConfig.useDfColors);
    case 2:
        //yellow;
        return ssConfig.colors.getDfColor(dfColors::yellow, ssConfig.useDfColors);
    case 3:
        //lime
        return ssConfig.colors.getDfColor(dfColors::lgreen, ssConfig.useDfColors);
    default:
        //white
        return ssConfig.colors.getDfColor(dfColors::white, ssConfig.useDfColors);
    }
}