#pragma once

#include "BlockCondition.h"

// generic superclass
class SpriteNode
{
	public:
		SpriteNode(void){};
		virtual ~SpriteNode(void){};
	
	// this returns true if the sprite matched, and also adds
	// sprites to the block as required
    virtual bool BlockMatches(Block* b)=0;
    // adds a child if appropriate (vestigial in some cases)
    virtual void addChild(SpriteNode* child){};	
};

// root nesting structure
class RootBlock : public SpriteNode
{
	vector<SpriteNode*> children;
	  
	public:
		RootBlock(void);
		~RootBlock(void); 
	
    bool BlockMatches(Block* b);
    void addChild(SpriteNode* child);	
};

// nesting conditional structure
class SpriteBlock : public ConditionalNode, public SpriteNode
{
	BlockCondition* conditions;
	vector<SpriteNode*> children;
	SpriteNode* elsenode;
	  
	public:
		SpriteBlock(void);
		~SpriteBlock(void);
	
    bool BlockMatches(Block* b);	
    bool addCondition(BlockCondition* cond);
    void addChild(SpriteNode* child);
	void addElse(SpriteNode* child);
};

// display element
class SpriteElement : public SpriteNode
{
private:

public:
  t_SpriteWithOffset sprite;
  
  SpriteElement(void);
  ~SpriteElement(void){};

  bool BlockMatches(Block* b);
};
