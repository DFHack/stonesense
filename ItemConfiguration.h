#pragma once

#include <memory>

#include "SpriteObjects.h"

class ItemSubConfiguration
{
public:
    c_sprite sprite;
};

class ItemConfiguration
{
public:
    std::vector<std::unique_ptr<ItemSubConfiguration>> subItems;
    c_sprite default_sprite;

    bool configured;

    ItemConfiguration();
};

bool addSingleItemConfig( TiXmlElement* elemRoot);
bool parseItemElement( TiXmlElement* elemRoot, int basefile);
void flushItemConfig(std::vector<std::unique_ptr<ItemConfiguration>> &config);
