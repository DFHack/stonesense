#pragma once

#include "SpriteObjects.h"

class ItemSubConfiguration
{
public:
    c_sprite sprite;
};

class ItemConfiguration
{
public:
    std::vector<ItemSubConfiguration*> subItems;
    c_sprite default_sprite;

    bool configured;

    ItemConfiguration();
    ~ItemConfiguration();
};

bool addSingleItemConfig( TiXmlElement* elemRoot);
bool parseItemElement( TiXmlElement* elemRoot, int basefile);
void flushItemConfig(std::vector<ItemConfiguration *> &config);