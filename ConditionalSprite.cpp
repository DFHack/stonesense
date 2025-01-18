#include "common.h"
#include "ConditionalSprite.h"
#include "Tile.h"
#include "GameBuildings.h"
#include "GUI.h"
#include "GameState.h"
#include "StonesenseState.h"

/* RootTile */

RootTile::RootTile() : SpriteNode()
{
    //cout << "RootTile +" << endl;
}

RootTile::~RootTile(void)
{
    //cout << "RootTile -" << endl;
}

bool RootTile::copyToTile(Tile* b)
{
    bool haveMatch{ false };
    for (auto& child : children) {
        haveMatch |= child->copyToTile(b);
    }
    return haveMatch;
}

void RootTile::addChild(std::unique_ptr<SpriteNode> child)
{
    children.push_back(std::move(child));
}

/* SpriteTile */

SpriteTile::SpriteTile() : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
}

SpriteTile::~SpriteTile(void)
{
    //cout << "SpriteTile -" << endl;
};

bool SpriteTile::copyToTile(Tile* b)
{
    bool condMatch = conditions ? conditions->Matches(b) : true;

    bool haveMatch{ false };

    if (condMatch) {
        for (auto& child : children)
            haveMatch |= child->copyToTile(b);
    } else if (elsenode != nullptr) {
        haveMatch = elsenode->copyToTile(b);
    }
    return haveMatch;
}

bool SpriteTile::addCondition(std::unique_ptr<TileCondition> cond)
{
    if (conditions) {
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

RotationTile::RotationTile() : ConditionalNode(), SpriteNode()
{
    //cout << "SpriteTile +" << endl;
}

RotationTile::~RotationTile(void)
{
    //cout << "SpriteTile -" << endl;
};

bool RotationTile::copyToTile(Tile* b)
{
    int index = stonesenseState.ssState.Rotation;
    size_t max = children.size();
    if (max == 0) {
        return false;
    }
    index = index % max;
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
