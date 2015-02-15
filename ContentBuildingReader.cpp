#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "GameBuildings.h"
#include "BuildingConfiguration.h"
#include "TileCondition.h"
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"
#include "df/world_raws.h"
#include "df/building_def.h"

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent);
bool parseSpriteNode(SpriteNode* node, TiXmlElement* elemParent);
bool includeFile(SpriteNode* node, TiXmlElement* includeNode, SpriteTile* &oldSibling);

bool parseRecursiveNodes (ConditionalNode* pnode, TiXmlElement* pelem)
{
    TiXmlElement* elemCondition = pelem->FirstChildElement();
    while( elemCondition ) {
        if (!parseConditionNode( pnode, elemCondition, false )) {
            return false;
        }
        elemCondition = elemCondition->NextSiblingElement();
    }
    return true;
}

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent)
{
    const char* strType = elemCondition->Value();
    TileCondition* cond = NULL;
    if( strcmp(strType, "NeighbourWall") == 0) {
        cond = new NeighbourWallCondition( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "PositionIndex") == 0) {
        cond = new PositionIndexCondition( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "MaterialType") == 0) {
        cond = new MaterialTypeCondition( elemCondition->Attribute("value") , elemCondition->Attribute("subtype"), elemCondition->Attribute("pattern_index"));
    }

    else if( strcmp(strType, "always") == 0) {
        cond = new AlwaysCondition();
    }

    else if( strcmp(strType, "never") == 0) {
        cond = new NeverCondition();
    }

    else if( strcmp(strType, "BuildingOccupancy") == 0) {
        cond = new BuildingOccupancyCondition( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "BuildingSpecial") == 0) {
        cond = new BuildingSpecialCondition( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "NeighbourSameBuilding") == 0) {
        cond = new NeighbourSameBuildingCondition( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "NeighbourSameType") == 0) {
        cond = new NeighbourSameTypeCondition( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "NeighbourOfType") == 0) {
        cond = new NeighbourOfTypeCondition( elemCondition->Attribute("dir") , elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "NeighbourIdentical") == 0) {
        cond = new NeighbourIdenticalCondition( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "AnimationFrame") == 0) {
        cond = new AnimationFrameCondition( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "FluidBelow") == 0) {
        cond = new FluidBelowCondition( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "HaveFloor") == 0) {
        cond = new HaveFloorCondition();
    }

    else if( strcmp(strType, "and") == 0) {
        AndConditionalNode* andNode = new AndConditionalNode();
        cond = andNode;
        if (!parseRecursiveNodes(andNode, elemCondition)) {
            delete(andNode);
            return 0;
        }
    }

    else if( strcmp(strType, "or") == 0) {
        OrConditionalNode* orNode = new OrConditionalNode();
        cond = orNode;
        if (!parseRecursiveNodes(orNode, elemCondition)) {
            delete(orNode);
            return 0;
        }
    }

    else if( strcmp(strType, "not") == 0) {
        NotConditionalNode* notNode = new NotConditionalNode();
        cond = notNode;
        if (!parseRecursiveNodes(notNode, elemCondition)) {
            delete(notNode);
            return 0;
        }
    }

    if (cond != NULL) {
        if (!node->addCondition( cond )) {
            contentError("Unable to add subcondition",elemCondition);
            delete(cond);
            return 0;
        }
        return 1;
    } else if (!silent) {
        contentError("Misplaced or invalid element in Condition",elemCondition);
        return 0;
    }
    return -1;
}

inline bool readNode(SpriteNode* node, TiXmlElement* elemNode, TiXmlElement* elemParent, SpriteTile* &oldSibling)
{
    const char* strType = elemNode->Value();
    if (strcmp(strType, "if") == 0 || strcmp(strType, "else") == 0) {
        SpriteTile* tile = new SpriteTile();
        if (!elemNode->Attribute("file") && elemParent->Attribute("file")) {
            elemNode->SetAttribute("file", elemParent->Attribute("file"));
        }
        if (!parseSpriteNode(tile,elemNode)) {
            delete(tile);
            return false;
        }
        if (elemNode->Attribute("else") || strcmp(strType, "else") == 0) {
            if (!oldSibling) {
                contentError("Misplaced or invalid element in SpriteNode",elemNode);
                return false;
            }
            oldSibling->addElse(tile);
        } else {
            node->addChild(tile);
        }
        oldSibling = tile;
    } else if (strcmp(strType, "rotate") == 0) {
        RotationTile* tile = new RotationTile();
        if (!elemNode->Attribute("file") && elemParent->Attribute("file")) {
            elemNode->SetAttribute("file",elemParent->Attribute("file"));
        }
        if (!parseSpriteNode(tile,elemNode)) {
            delete(tile);
            return false;
        } else {
            node->addChild(tile);
        }
        oldSibling = NULL;
    } else if ((strcmp(strType, "sprite") == 0) || (strcmp(strType, "empty") == 0)) {
        int fileindex = 0;
        const char* pfilename = elemParent->Attribute("file");
        if (pfilename != NULL && pfilename[0] != 0) {
            fileindex = loadConfigImgFile((char*)pfilename,elemNode);
            if(fileindex == -1) {
                return false;
            }
        }
        SpriteElement* sprite = new SpriteElement();
        sprite->sprite.set_by_xml(elemNode, fileindex);
        node->addChild(sprite);
    } else if (strcmp(strType, "include") == 0) {
        if (!includeFile(node,elemNode,oldSibling)) {
            return false;
        }
    } else {
        contentError("Misplaced or invalid element in SpriteNode",elemNode);
        return false;
    }
    return true;
}

bool includeFile(SpriteNode* node, TiXmlElement* includeNode, SpriteTile* &oldSibling)
{
    // get path... ugly
    char configfilepath[FILENAME_BUFFERSIZE] = {0};
    const char* documentRef = getDocument(includeNode);

    if (!getLocalFilename(configfilepath,includeNode->Attribute("file"),documentRef)) {
        return false;
    }
    ALLEGRO_PATH * incpath = al_create_path(configfilepath);
    al_append_path_component(incpath, "include");
    TiXmlDocument doc( al_path_cstr(incpath, ALLEGRO_NATIVE_PATH_SEP) );
    al_destroy_path(incpath);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle hDoc(&doc);
    TiXmlElement* elemParent;
    if(!loadOkay) {
        contentError("Include failed",includeNode);
        LogError("File load failed: %s\n",configfilepath);
        LogError("Line %d: %s\n",doc.ErrorRow(),doc.ErrorDesc());
        return false;
    }
    elemParent = hDoc.FirstChildElement("include").Element();
    if( elemParent == NULL) {
        contentError("Main <include> node not present",&doc);
        return false;
    }
    TiXmlElement* elemNode =  elemParent->FirstChildElement();
    if (elemNode == NULL) {
        contentError("Empty include",elemParent);
        return false;
    }
    while (elemNode) {
        if (!readNode(node, elemNode, elemParent, oldSibling)) {
            return false;
        }
        elemNode = elemNode->NextSiblingElement();
    }
    return true;
}

bool parseSpriteNode(SpriteNode* node, TiXmlElement* elemParent)
{
    //TODO: there is a leak here somewhere.
    SpriteTile* oldSibling = NULL;
    TiXmlElement* elemNode =  elemParent->FirstChildElement();
    const char* strParent = elemParent->Value();
    if (elemNode == NULL) {
        contentError("Empty SpriteNode Element",elemParent);
        return false;
    }
    if ( strcmp(strParent,"building") != 0 && strcmp(strParent,"custom_workshop") != 0 && strcmp(strParent,"rotate") != 0) {
        //flag to allow else statements to be empty, rather than needing an "always" tag
        bool allowBlank = (strcmp(strParent,"else") == 0 || elemParent->Attribute("else"));
        // cast should be safe, because only spritetiles
        // should get here
        int retvalue =parseConditionNode((SpriteTile *)node,elemNode,allowBlank);
        if (retvalue == 0) {
            return false;
        }
        if (retvalue > 0) {
            elemNode = elemNode->NextSiblingElement();
        }
    }
    while (elemNode) {
        if (!readNode(node, elemNode, elemParent, oldSibling)) {
            return false;
        }
        elemNode = elemNode->NextSiblingElement();
    }
    return true;
}

#include "df/siegeengine_type.h"
#include "df/workshop_type.h"
#include "df/trap_type.h"
#include "df/shop_type.h"
#include "df/construction_type.h"
#include "df/furnace_type.h"

bool addSingleBuildingConfig( TiXmlElement* elemRoot,  vector<BuildingConfiguration>* knownBuildings )
{
    const char* strName = elemRoot->Attribute("name");
    const char* strGameID = elemRoot->Attribute("game_type");
    const char* strGameSub = elemRoot->Attribute("game_subtype");
    const char* strGameCustom = elemRoot->Attribute("game_custom");

    if (strName == NULL || strGameID == NULL || strName[0] == 0 || strGameID[0] == 0) {
        contentError("<building> node must have name and game_type attributes",elemRoot);
        return false;
    }
    building_type::building_type main_type = BUILDINGTYPE_NA;
    int subtype = INVALID_INDEX;
    string game_type_s;
    FOR_ENUM_ITEMS(building_type,i) {
        game_type_s = strGameID;
        if (game_type_s == ENUM_KEY_STR(building_type,i)) {
            main_type = i;
            break;
        }
    }
    if(main_type == BUILDINGTYPE_NA) {
        contentError("<building> unknown game_type value",elemRoot);
        return false;
    }

    // get subtype string, if available
    string sub;
    if(strGameSub) {
        sub = strGameSub;
    }

    bool needs_custom = false;

    // process types
    switch (main_type) {
    case building_type::Furnace: {
        if(!strGameSub) {
            contentWarning("<building> Is generic - game_subtype missing.",elemRoot);
            break;
        }
        FOR_ENUM_ITEMS(furnace_type,i) {
            if (sub == ENUM_KEY_STR(furnace_type,i)) {
                subtype = i;
                break;
            }
        }
        if(subtype == INVALID_INDEX) {
            contentError("<building> unknown game_subtype value",elemRoot);
            return false;
        }
        if(subtype == furnace_type::Custom) {
            needs_custom = true;
        }
        break;
    }
    case building_type::Construction: {
        if(!strGameSub) {
            contentWarning("<building> Is generic - game_subtype missing.",elemRoot);
            break;
        }
        FOR_ENUM_ITEMS(construction_type,i) {
            if (sub == ENUM_KEY_STR(construction_type,i)) {
                subtype = i;
                break;
            }
        }
        if(subtype == INVALID_INDEX) {
            contentError("<building> unknown game_subtype value",elemRoot);
            return false;
        }
        break;
    }
    case building_type::SiegeEngine: {
        if(!strGameSub) {
            contentWarning("<building> Is generic - game_subtype missing.",elemRoot);
            break;
        }
        FOR_ENUM_ITEMS(siegeengine_type,i) {
            if (sub == ENUM_KEY_STR(siegeengine_type,i)) {
                subtype = i;
                break;
            }
        }
        if(subtype == INVALID_INDEX) {
            contentError("<building> unknown game_subtype value",elemRoot);
            return false;
        }
        break;
    }
    case building_type::Shop: {
        if(!strGameSub) {
            contentWarning("<building> Is generic - game_subtype missing.",elemRoot);
            break;
        }
        FOR_ENUM_ITEMS(shop_type,i) {
            if (sub == ENUM_KEY_STR(shop_type,i)) {
                subtype = i;
                break;
            }
        }
        if(subtype == INVALID_INDEX) {
            contentError("<building> unknown game_subtype value",elemRoot);
            return false;
        }
        break;
    }
    case building_type::Workshop: {
        if(!strGameSub) {
            contentWarning("<building> Is generic - game_subtype missing.",elemRoot);
            break;
        }
        FOR_ENUM_ITEMS(workshop_type,i) {
            if (sub == ENUM_KEY_STR(workshop_type,i)) {
                subtype = i;
                break;
            }
        }
        if(subtype == INVALID_INDEX) {
            contentError("<building> unknown game_subtype value",elemRoot);
            return false;
        }
        if(subtype == workshop_type::Custom) {
            needs_custom = true;
        }
        break;
    }
    default: {
        // we need no sub/custom type.
        break;
    }
    }
    int32_t custom = -1;
    // needs custom building spec, doesn't have a string... FAIL
    if (needs_custom && (strGameCustom == 0 || strGameCustom[0] == 0)) {
        contentError("<building> game_custom attribute is required, but missng.",elemRoot);
        return false;
    }
    else if (strGameCustom && strGameCustom[0])
    {
        for (int i = 0; i < df::global::world->raws.buildings.all.size(); i++)
        {
            if (strcmp(strGameCustom, df::global::world->raws.buildings.all[i]->code.c_str()) == 0)
                custom = i;
        }
        if (custom == -1)
        {
            contentWarning("<building> game_custom attribute is invalid", elemRoot);
            return false;
        }
    }
    BuildingConfiguration building(strName, main_type, subtype, custom );
    RootTile* spriteroot = new RootTile();
    building.sprites = spriteroot;
    if (!parseSpriteNode(spriteroot,elemRoot)) {
        delete(spriteroot);
        contentError("<building> Failed while parsing sprite node",elemRoot);
        return false;
    }

    //add a copy of 'building' to known buildings
    knownBuildings->push_back( building );
    return true;
}

void flushBuildingConfig( vector<BuildingConfiguration>* knownBuildings )
{
    // clean up building data trees before deleting them
    // a nasty cludge that only works cause knownbuildings
    // isnt modified anywhere else
    // TODO: look into smart pointers or something
    uint32_t numBuildings = (uint32_t)knownBuildings->size();
    for(uint32_t i = 0; i < numBuildings; i++) {
        delete(knownBuildings->at(i).sprites);
        //should set to null, but we will nuke the lot in a second
    }
    knownBuildings->clear();
}

