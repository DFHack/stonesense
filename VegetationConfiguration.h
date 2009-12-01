#pragma once

#include "dfhack/library/tinyxml/tinyxml.h"

class VegetationConfiguration
{
public:
  int gameID;
  bool live;
  bool grown;
  t_SpriteWithOffset sprite;

  VegetationConfiguration(){}
  VegetationConfiguration(int gameID, t_SpriteWithOffset &sprite, bool live, bool grown);
  ~VegetationConfiguration(void);
};


bool addSingleVegetationConfig( TiXmlElement* elemRoot,  vector<VegetationConfiguration>* vegetationConfigs, vector<t_matgloss>& plantNames );
t_SpriteWithOffset getVegetationSprite(vector<VegetationConfiguration>& vegetationConfigs,int index,bool live,bool grown);
