#pragma once

#include "common.h"

//int getJobColor(unsigned char job);
ALLEGRO_COLOR premultiply(ALLEGRO_COLOR input);
ALLEGRO_COLOR operator*(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2);
ALLEGRO_COLOR operator+(const ALLEGRO_COLOR &color1, const ALLEGRO_COLOR &color2);
ALLEGRO_COLOR getDayShade(int hour, int tick);
ALLEGRO_COLOR partialBlend(const ALLEGRO_COLOR & color2, const ALLEGRO_COLOR & color1, int percent);
ALLEGRO_COLOR shadeAdventureMode(ALLEGRO_COLOR color, bool foggy, bool outside=true);
ALLEGRO_COLOR blink(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2);
ALLEGRO_COLOR blinkTechnicolor();
ALLEGRO_COLOR uiColor(int32_t index=0);