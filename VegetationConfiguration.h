#pragma once

#include "dfhack/depends/tinyxml/tinyxml.h"
#include "SpriteObjects.h"
class VegetationConfiguration
{
public:
  int gameID;
  bool live;
  bool grown;
  c_sprite sprite;

  VegetationConfiguration(){}
  VegetationConfiguration(int gameID, c_sprite &sprite, bool live, bool grown);
  ~VegetationConfiguration(void);
};


bool addSingleVegetationConfig( TiXmlElement* elemRoot,  vector<VegetationConfiguration>* vegetationConfigs, vector<t_matgloss>& plantNames );
c_sprite getVegetationSprite(vector<VegetationConfiguration>& vegetationConfigs,int index,bool live,bool grown);
