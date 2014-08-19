#include "common.h"
#include "ItemConfiguration.h"
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"

ItemConfiguration::ItemConfiguration()
{
    configured=0;
}



ItemConfiguration::~ItemConfiguration()
{
    subItems.clear();
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

    if(contentLoader->itemConfigs[main_type] == NULL) {
        contentLoader->itemConfigs[main_type] = new ItemConfiguration;
    }
    //check for an existing item there.
    if(subtype == INVALID_INDEX) {
        if(!contentLoader->itemConfigs[main_type]->configured) {
            contentLoader->itemConfigs[main_type]->configured = true;
            contentLoader->itemConfigs[main_type]->default_sprite = sprite;
        }
    } else {
        if(contentLoader->itemConfigs[main_type]->subItems.size() <= subtype) {
            contentLoader->itemConfigs[main_type]->subItems.resize(subtype+1, NULL);
        }
        if(!contentLoader->itemConfigs[main_type]->subItems[subtype]) {
            contentLoader->itemConfigs[main_type]->subItems[subtype] = new ItemSubConfiguration;
            contentLoader->itemConfigs[main_type]->subItems[subtype]->sprite = sprite;
        }
    }
    return true;
}


void flushItemConfig(vector<ItemConfiguration *> &config)
{
    uint32_t currentsize = (uint32_t)config.size();
    for (uint32_t i=0; i<currentsize; i++) {
        if (config[i] != NULL) {
            delete(config[i]);
        }
    }

    config.clear();
    if (currentsize < ENUM_LAST_ITEM(item_type)) {
        currentsize = ENUM_LAST_ITEM(item_type);
    }
    config.resize(currentsize,NULL);
}