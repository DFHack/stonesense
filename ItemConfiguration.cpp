#include "common.h"
#include "ItemConfiguration.h"
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "MiscUtils.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

ItemConfiguration::ItemConfiguration()
{
    configured=0;
}

bool addSingleItemConfig( TiXmlElement* elemRoot)
{
    int basefile = INVALID_INDEX;
    const char* filename = elemRoot->Attribute("file");
    if (filename != NULL && filename[0] != 0) {
        basefile = loadConfigImgFile((char*)filename,elemRoot);
        if(basefile == -1) {
            return false;
        }
    }


    TiXmlElement* elemFloor = elemRoot->FirstChildElement("item");
    while( elemFloor ) {
        parseItemElement( elemFloor, basefile);
        elemFloor = elemFloor->NextSiblingElement("item");
    }
    return true;
}

bool parseItemElement( TiXmlElement* elemRoot, int basefile)
{
    const char* strGameID = elemRoot->Attribute("game_type");
    const char* strGameSub = elemRoot->Attribute("game_subtype");

    if (strGameID == NULL || strGameID[0] == 0) {
        contentError("<item> node must game_type attribute",elemRoot);
        return false;
    }
    item_type::item_type main_type = (item_type::item_type) INVALID_INDEX;
    int subtype = INVALID_INDEX;
    string game_type_s;
    FOR_ENUM_ITEMS(item_type,i) {
        game_type_s = strGameID;
        if (game_type_s == ENUM_KEY_STR(item_type,i)) {
            main_type = i;
            break;
        }
    }
    if(main_type == (item_type::item_type) INVALID_INDEX) {
        contentWarning("<item> unknown game_type value",elemRoot);
        return false;
    }

    if(strGameSub && strGameSub[0] != 0) {
        // get subtype string, if available
        string sub;
        sub += strGameID;
        sub += ":";
        sub += strGameSub;

        //process subtypes
        ItemTypeInfo itemdef;
        if(!itemdef.find(sub)) {
            contentError("<item> unknown game_subtype value",elemRoot);
            return false;
        } else {
            subtype = itemdef.subtype;
        }
    }


    c_sprite sprite;

    sprite.set_by_xml(elemRoot, basefile);

    if(contentLoader->itemConfigs[main_type] == nullptr) {
        contentLoader->itemConfigs[main_type] = std::make_unique<ItemConfiguration>();
    }
    //check for an existing item there.
    if(subtype == INVALID_INDEX) {
        if(!contentLoader->itemConfigs[main_type]->configured) {
            contentLoader->itemConfigs[main_type]->configured = true;
            contentLoader->itemConfigs[main_type]->default_sprite = sprite;
        }
    } else {
        while(contentLoader->itemConfigs[main_type]->subItems.size() <= size_t(subtype)) {
            contentLoader->itemConfigs[main_type]->subItems.push_back(nullptr);
        }
        if(!contentLoader->itemConfigs[main_type]->subItems[subtype]) {
            contentLoader->itemConfigs[main_type]->subItems[subtype] = std::make_unique<ItemSubConfiguration>();
            contentLoader->itemConfigs[main_type]->subItems[subtype]->sprite = sprite;
        }
    }
    return true;
}


void flushItemConfig(vector<std::unique_ptr<ItemConfiguration>> &config)
{
    if (config.size() != (ENUM_LAST_ITEM(item_type) + 1))
        config.resize(ENUM_LAST_ITEM(item_type) + 1);

    for (size_t i = 0; i < config.size(); i++)
    {
        config[i] = nullptr;
    }
}
