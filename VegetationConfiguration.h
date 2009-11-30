#pragma once

#include "dfhack/library/tinyxml/tinyxml.h"

class VegetationConfiguration
{
public:
  int gameID;
  bool live;
  t_SpriteWithOffset sprite;

  VegetationConfiguration(){}
  VegetationConfiguration(int gameID, t_SpriteWithOffset &sprite, bool live);
  ~VegetationConfiguration(void);
};


bool addSingleVegetationConfig( TiXmlElement* elemRoot,  vector<VegetationConfiguration>* vegetationConfigs, vector<t_matgloss>& plantNames );
