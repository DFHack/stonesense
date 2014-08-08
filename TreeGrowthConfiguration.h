#pragma once

#include "ContentLoader.h"

void parseGrowthElement(TiXmlElement* elemGrowthSprite, MaterialMatcher<c_sprite> & growthTopConfigs, MaterialMatcher<c_sprite> & growthBottomConfigs, int basefile);
bool addSingleGrowthConfig(TiXmlElement* elemRoot);