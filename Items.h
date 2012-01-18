#pragma once
#include "common.h"
#include "tinyxml.h"

static t_SpriteWithOffset spriteItem_NA = {142, 0, 0,-1,1+2+4+8+16+32};
static t_SpriteWithOffset spriteItem_None = {-1, 0, 0,-1,1+2+4+8+16+32};

void ReadItems(DFHack::Core& DF);
void clearItemCache();
void clearCachedItem(uint32_t x,uint32_t y,uint32_t z);
t_CachedItem* getCachedItem(uint32_t x, uint32_t y, uint32_t z);

void DrawItem(int drawx, int drawy, t_CachedItem& item );
t_SpriteWithOffset GetItemSpriteMap( t_CachedItem& item );

class ItemConfiguration
{
	public:
	t_SpriteWithOffset sprite;
	int matType;
	int matIndex;
	
	ItemConfiguration(){}
	ItemConfiguration(t_SpriteWithOffset &sprite, int matType, int matIndex);
	~ItemConfiguration(void);
};

bool addItemsConfig( TiXmlElement* elemRoot, vector<vector<ItemConfiguration>*>& knownItems );
