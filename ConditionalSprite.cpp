#include "common.h"
#include "ConditionalSprite.h"
#include "Tile.h"
#include "GameBuildings.h"
#include "GUI.h"

using namespace std;
using namespace DFHack;
using namespace df::enums;

/* RootTile */

RootTile::RootTile()
    : SpriteNode()
{
    //cout << "RootTile +" << endl;
}

RootTile::~RootTile(void)
{
    //cout << "RootTile -" << endl;
}

bool RootTile::copyToTile(Tile* b)
{
    bool haveMatch = false;
    uint32_t max = (uint32_t)children.size();

    for(uint32_t i=0; i<max; i++) {
        if (children[i]->copyToTile(b)) {
            haveMatch = true;
        }
    }
    return haveMatch;
}

void RootTile::addChild(std::unique_ptr<SpriteNode> child)
{
    children.push_back(std::move(child));
}

/* SpriteTile */

SpriteTile::SpriteTile()
    : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
}

SpriteTile::~SpriteTile(void)
{
    //cout << "SpriteTile -" << endl;
};

bool SpriteTile::copyToTile(Tile* b)
{
    bool condMatch = false;
    if (conditions == nullptr) {
        condMatch = true;
    } else {
        condMatch = conditions->Matches( b );
    }

    bool haveMatch=false;
    if (condMatch) {
        uint32_t max = (uint32_t)children.size();
        for(uint32_t i=0; i<max; i++) {
            if (children[i]->copyToTile(b)) {
                haveMatch = true;
            }
        }
    } else if (elsenode != nullptr) {
        haveMatch = elsenode->copyToTile(b);
    }
    return haveMatch;
}

bool SpriteTile::addCondition(std::unique_ptr<TileCondition> cond)
{
    if (conditions != nullptr) {
        LogError("Too many condition elements for SpriteTile\n");
        return false;
    }
    conditions = std::move(cond);
    return true;
}

void SpriteTile::addChild(std::unique_ptr<SpriteNode> child)
{
    children.push_back(std::move(child));
}

void SpriteTile::addElse(std::unique_ptr<SpriteNode> child)
{
    elsenode = std::move(child);
}


/* RotationTile */

RotationTile::RotationTile()
    : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
}

RotationTile::~RotationTile(void)
{
    //cout << "SpriteTile -" << endl;
};

bool RotationTile::copyToTile(Tile* b)
{
    int index = ssState.Rotation;
    int max = (int)children.size();
    if (max == 0) {
        return false;
    }
    while (index >= max) {
        index = index - max;
    }
    return children[index]->copyToTile(b);
}

bool RotationTile::addCondition(std::unique_ptr<TileCondition> cond)
{
    LogError("Condition elements not permitted for RotationTile\n");
    return false;
}

void RotationTile::addChild(std::unique_ptr<SpriteNode> child)
{
    children.push_back(std::move(child));
}

/* SpriteElement */

SpriteElement::SpriteElement()
    : SpriteNode()
{
    //cout << "SpriteElement +" << endl;
    sprite.set_sheetindex(-1);
}

bool SpriteElement::copyToTile(Tile* b)
{
    if (sprite.get_sheetindex() > -1) {
        b->building.sprites.push_back(sprite);
    }
    return true;
}
