#pragma once

#include "tinyxml.h"
#include "TileTree.h"
class VegetationConfiguration
{
public:
    int gameID;
    bool live;
    bool grown;
    c_tile_tree tree;

    VegetationConfiguration() {}
    VegetationConfiguration(int gameID, c_tile_tree &tree, bool live, bool grown);
    ~VegetationConfiguration(void);
};


bool addSingleVegetationConfig(TiXmlElement* elemRoot, std::vector<std::unique_ptr<VegetationConfiguration>>* vegetationConfigs, std::vector<DFHack::t_matgloss>& plantNames);
c_tile_tree * getVegetationTree(std::vector<std::unique_ptr<VegetationConfiguration>>& vegetationConfigs, int index, bool live, bool grown);
