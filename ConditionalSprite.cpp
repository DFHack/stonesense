#include "common.h"
#include "ConditionalSprite.h"
#include "Block.h"
#include "GameBuildings.h"
#include "GUI.h"

/* RootBlock */

RootBlock::RootBlock()
	: SpriteNode()
{
	//cout << "RootBlock +" << endl;
}

RootBlock::~RootBlock(void)
{
	//cout << "RootBlock -" << endl;
	uint32_t max = (uint32_t)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		delete(children[i]);
	}
}

bool RootBlock::BlockMatches(Block* b)
{
	bool haveMatch = false;
	uint32_t max = (uint32_t)children.size();
	
	for(uint32_t i=0; i<max; i++)
	{
		if (children[i]->BlockMatches(b))
		{
			haveMatch = true;	
		}
	}
	return haveMatch;
}

void RootBlock::addChild(SpriteNode* child){
	children.push_back(child);
}

/* SpriteBlock */

SpriteBlock::SpriteBlock()
	: ConditionalNode(), SpriteNode()
{
	//cout << "SpriteBlock +" << endl;
	conditions = NULL;
	elsenode = NULL;
}

SpriteBlock::~SpriteBlock(void)
{
	//cout << "SpriteBlock -" << endl;
	delete(elsenode);
	delete(conditions);
	uint32_t max = (uint32_t)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		delete(children[i]);
	}
};

bool SpriteBlock::BlockMatches(Block* b)
{
	bool condMatch = false;
	if (conditions == NULL)
	{
		condMatch = true;	
	}
	else
	{
		condMatch = conditions->Matches( b );
	}
	
	bool haveMatch=false;
	if (condMatch)
	{
		uint32_t max = (uint32_t)children.size();
		for(uint32_t i=0; i<max; i++)
		{
			if (children[i]->BlockMatches(b))
			{
				haveMatch = true;	
			}
		}
	}
	else if (elsenode != NULL)
	{
		haveMatch = elsenode->BlockMatches(b);
	}
	return haveMatch;
}

bool SpriteBlock::addCondition(BlockCondition* cond){
	if (conditions != NULL)
	{
		WriteErr("Too many condition elements for SpriteBlock\n");
		return false;
	}
	conditions = cond;
	return true;
}

void SpriteBlock::addChild(SpriteNode* child){
	children.push_back(child);
}

void SpriteBlock::addElse(SpriteNode* child){
	elsenode = child;
}


/* RotationBlock */

RotationBlock::RotationBlock()
	: ConditionalNode(), SpriteNode()
{
	//cout << "SpriteBlock +" << endl;
}

RotationBlock::~RotationBlock(void)
{
	//cout << "SpriteBlock -" << endl;
	uint32_t max = (uint32_t)children.size();
	for(uint32_t i=0; i<max; i++)
	{
		delete(children[i]);
	}
};

bool RotationBlock::BlockMatches(Block* b)
{
	int index = DisplayedRotation;
	int max = (int)children.size();
	if (max == 0)
		return false;
	while (index >= max)
	{
		index = index - max;
	}
	return children[index]->BlockMatches(b);
}

bool RotationBlock::addCondition(BlockCondition* cond){
	WriteErr("Condition elements not permitted for RotationBlock\n");
	return false;
}

void RotationBlock::addChild(SpriteNode* child){
	children.push_back(child);
}

/* SpriteElement */

SpriteElement::SpriteElement()
	: SpriteNode()
{
	//cout << "SpriteElement +" << endl;
	sprite.sheetIndex = -1;
	sprite.x = 0;
	sprite.y = 0;
}

bool SpriteElement::BlockMatches(Block* b)
{
	if (sprite.sheetIndex > -1)
	{
		b->building.sprites.push_back(sprite);
	}
	return true;
}
