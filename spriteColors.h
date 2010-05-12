#pragma once

#include "common.h"

ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial);
ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, t_matglossPair material, t_matglossPair layerMaterial, t_matglossPair veinMaterial);
ALLEGRO_COLOR getSpriteColor(t_SpriteWithOffset &sprite, t_creature* creature);
ALLEGRO_COLOR getSpriteColor(t_subSprite &sprite, t_creature* creature);