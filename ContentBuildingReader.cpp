#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "GameBuildings.h"
#include "BuildingConfiguration.h"
#include "MiscUtils.h"
#include "TileCondition.h"
#include "tinyxml.h"
#include "GUI.h"
#include "ContentLoader.h"

#include "df/building_def.h"
#include "df/world.h"

using std::string;

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent);
bool parseSpriteNode(SpriteNode* node, TiXmlElement* elemParent);
bool includeFile(SpriteNode* node, TiXmlElement* includeNode, SpriteTile* &oldSibling);

namespace {
    bool parseRecursiveNodes(ConditionalNode * pnode, TiXmlElement * pelem)
    {
        TiXmlElement* elemCondition = pelem->FirstChildElement();
        while (elemCondition) {
            if (!parseConditionNode(pnode, elemCondition, false)) {
                return false;
            }
            elemCondition = elemCondition->NextSiblingElement();
        }
        return true;
    }
}

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent)
{
    const char* strType = elemCondition->Value();
    std::unique_ptr<TileCondition> cond;
    if( strcmp(strType, "NeighbourWall") == 0) {
        cond = std::make_unique<NeighbourWallCondition>( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "PositionIndex") == 0) {
        cond = std::make_unique<PositionIndexCondition>( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "MaterialType") == 0) {
        cond = std::make_unique<MaterialTypeCondition>( elemCondition->Attribute("value") , elemCondition->Attribute("subtype"), elemCondition->Attribute("pattern_index"));
    }

    else if( strcmp(strType, "always") == 0) {
        cond = std::make_unique<AlwaysCondition>();
    }

    else if( strcmp(strType, "never") == 0) {
        cond = std::make_unique<NeverCondition>();
    }

    else if( strcmp(strType, "BuildingOccupancy") == 0) {
        cond = std::make_unique<BuildingOccupancyCondition>( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "BuildingSpecial") == 0) {
        cond = std::make_unique<BuildingSpecialCondition>( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "NeighbourSameBuilding") == 0) {
        cond = std::make_unique<NeighbourSameBuildingCondition>( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "NeighbourSameType") == 0) {
        cond = std::make_unique<NeighbourSameTypeCondition>( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "NeighbourOfType") == 0) {
        cond = std::make_unique<NeighbourOfTypeCondition>( elemCondition->Attribute("dir") , elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "NeighbourIdentical") == 0) {
        cond = std::make_unique<NeighbourIdenticalCondition>( elemCondition->Attribute("dir") );
    }

    else if( strcmp(strType, "AnimationFrame") == 0) {
        cond = std::make_unique<AnimationFrameCondition>( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "FluidBelow") == 0) {
        cond = std::make_unique<FluidBelowCondition>( elemCondition->Attribute("value") );
    }

    else if( strcmp(strType, "HaveFloor") == 0) {
        cond = std::make_unique<HaveFloorCondition>();
    }

    else if( strcmp(strType, "and") == 0) {
        auto andNode = std::make_unique<AndConditionalNode>();
        if (!parseRecursiveNodes(andNode.get(), elemCondition)) {
            return 0;
        }
        cond = std::move(andNode);
    }

    else if( strcmp(strType, "or") == 0) {
        auto orNode = std::make_unique<OrConditionalNode>();
        if (!parseRecursiveNodes(orNode.get(), elemCondition)) {
            return 0;
        }
        cond = std::move(orNode);
    }

    else if( strcmp(strType, "not") == 0) {
        auto notNode = std::make_unique<NotConditionalNode>();
        if (!parseRecursiveNodes(notNode.get(), elemCondition)) {
            return 0;
        }
        cond = std::move(notNode);
    }

    if (cond != nullptr) {
        if (!node->addCondition( std::move(cond) )) {
            contentError("Unable to add subcondition",elemCondition);
            return 0;
        }
        return 1;
    } else if (!silent) {
        contentError("Misplaced or invalid element in Condition",elemCondition);
        return 0;
    }
    return -1;
}

namespace {
    bool readNode(SpriteNode* node, TiXmlElement* elemNode, TiXmlElement* elemParent, SpriteTile*& oldSibling)
    {
        const char* strType = elemNode->Value();
        if (strcmp(strType, "if") == 0 || strcmp(strType, "else") == 0) {
            if (!elemNode->Attribute("file") && elemParent->Attribute("file")) {
                elemNode->SetAttribute("file", elemParent->Attribute("file"));
            }

            auto tile = std::make_unique<SpriteTile>();
            if (!parseSpriteNode(tile.get(), elemNode)) {
                return false;
            }

            auto tilePtr = tile.get();
            if (elemNode->Attribute("else") || strcmp(strType, "else") == 0) {
                if (!oldSibling) {
                    contentError("Misplaced or invalid element in SpriteNode", elemNode);
                    return false;
                }
                oldSibling->addElse(std::move(tile));
            }
            else {
                node->addChild(std::move(tile));
            }
            oldSibling = tilePtr;
        }
        else if (strcmp(strType, "rotate") == 0) {
            if (!elemNode->Attribute("file") && elemParent->Attribute("file")) {
                elemNode->SetAttribute("file", elemParent->Attribute("file"));
            }

            auto tile = std::make_unique<RotationTile>();
            if (!parseSpriteNode(tile.get(), elemNode)) {
                return false;
            }
            else {
                node->addChild(std::move(tile));
            }
            oldSibling = nullptr;
        }
        else if ((strcmp(strType, "sprite") == 0) || (strcmp(strType, "empty") == 0)) {
            int fileindex = 0;
            const char* pfilename = elemParent->Attribute("file");
            if (pfilename != NULL && pfilename[0] != 0) {
                fileindex = loadConfigImgFile((char*)pfilename, elemNode);
                if (fileindex == -1) {
                    return false;
                }
            }
            auto sprite = std::make_unique<SpriteElement>();
            sprite->sprite.set_by_xml(elemNode, fileindex);
            node->addChild(std::move(sprite));
        }
        else if (strcmp(strType, "include") == 0) {
            if (!includeFile(node, elemNode, oldSibling)) {
                return false;
            }
        }
        else {
            contentError("Misplaced or invalid element in SpriteNode", elemNode);
            return false;
        }
        return true;
    }
}

bool includeFile(SpriteNode* node, TiXmlElement* includeNode, SpriteTile* &oldSibling)
{
    // get path... ugly
    std::filesystem::path documentRef = getDocument(includeNode);

    std::filesystem::path configfilepath = getLocalFilename(includeNode->Attribute("file"), documentRef);
    std::filesystem::path incpath = configfilepath.remove_filename() / "include" / configfilepath.filename();
    TiXmlDocument doc( incpath.string().c_str() );
    bool loadOkay = doc.LoadFile();
    TiXmlHandle hDoc(&doc);
    TiXmlElement* elemParent;
    if(!loadOkay) {
        contentError("Include failed",includeNode);
        LogError("File load failed: %s\n", incpath.string().c_str());
        LogError("Line %d: %s\n",doc.ErrorRow(),doc.ErrorDesc() ? doc.ErrorDesc() : "(null)");
        return false;
    }
    elemParent = hDoc.FirstChildElement("include").Element();
    if( elemParent == nullptr) {
        contentError("Main <include> node not present",&doc);
        return false;
    }
    TiXmlElement* elemNode =  elemParent->FirstChildElement();
    if (elemNode == nullptr) {
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
    SpriteTile* oldSibling = nullptr;
    TiXmlElement* elemNode =  elemParent->FirstChildElement();
    if (elemNode == nullptr) {
        contentError("Empty SpriteNode Element",elemParent);
        return false;
    }

    const char* strParent = elemParent->Value();
    if ( strcmp(strParent,"building") != 0 && strcmp(strParent,"custom_workshop") != 0 && strcmp(strParent,"rotate") != 0) {
        //flag to allow else statements to be empty, rather than needing an "always" tag
        bool allowBlank = (strcmp(strParent,"else") == 0 || elemParent->Attribute("else"));
        // cast should be safe, because only spritetiles
        // should get here
        int retvalue = parseConditionNode((SpriteTile *)node,elemNode,allowBlank);
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

bool addSingleBuildingConfig( TiXmlElement* elemRoot,  std::vector<std::unique_ptr<BuildingConfiguration>>*knownBuildings)
{
    using df::building_type;
    const char* strName = elemRoot->Attribute("name");
    const char* strGameID = elemRoot->Attribute("game_type");
    const char* strGameSub = elemRoot->Attribute("game_subtype");
    const char* strGameCustom = elemRoot->Attribute("game_custom");

    if (strName == nullptr || strGameID == nullptr || strName[0] == 0 || strGameID[0] == 0) {
        contentError("<building> node must have name and game_type attributes",elemRoot);
        return false;
    }
    df::building_type main_type = BUILDINGTYPE_NA;
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
        if(subtype == df::furnace_type::Custom) {
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
        if(subtype == df::workshop_type::Custom) {
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
        auto& bld = df::global::world->raws.buildings.all;
        auto it = std::find_if(bld.begin(), bld.end(), [&](auto b) { return b->code == strGameCustom; });
        if (it != bld.end())
            custom = it - bld.begin();
        if (custom == -1)
        {
            contentWarning("<building> game_custom attribute is invalid", elemRoot);
            return false;
        }
    }
    auto building = std::make_unique<BuildingConfiguration>(strName, main_type, subtype, custom );
    auto spriteroot = std::make_unique<RootTile>();
    if (!parseSpriteNode(spriteroot.get(), elemRoot)) {
        contentError("<building> Failed while parsing sprite node",elemRoot);
        return false;
    }
    building->sprites = std::move(spriteroot);

    //add a copy of 'building' to known buildings
    knownBuildings->push_back( std::move(building) );
    return true;
}
