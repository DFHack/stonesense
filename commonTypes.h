#pragma once



struct t_SpriteWithOffset{
  int32_t sheetIndex;
  int16_t x;
  int16_t y;
} ;


typedef struct Crd2D {
	int32_t x,y;
}Crd2D;
typedef struct Crd3D {
	int32_t x,y,z;
}Crd3D;




typedef struct {
  bool show_zones;
  bool show_stockpiles;
  bool single_layer_view;
  bool shade_hidden_blocks;
  bool show_hidden_blocks;
  int automatic_reload_time;

  int screenWidth;
  int screenHeight;
  bool Fullscreen;

  Crd3D segmentSize;

} GameConfiguration;