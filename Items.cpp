#include "Items.h"
#include "GUI.h"
#include "ContentLoader.h"

#include <unordered_map>

struct ItemLocKey{
	uint32_t x;
    uint32_t y;
    uint32_t z;
} ;

size_t search_offset=0;

// this hash assumes we wont use more than about 10 bits in each coordinate
// it could probably be made better but more expensive with more advanced bit twiddling
struct LocKeyHash {
    size_t operator() (const ItemLocKey& loc) const
    { return loc.x |
            (loc.y << 10) | 
            (loc.z << 20); 
    }
};

struct LocKeyEqual {
    bool operator() (const ItemLocKey& loca, const ItemLocKey& locb) const { return (loca.x == locb.x) && (loca.y == locb.y) && (loca.z == locb.z); }
};

unordered_map<ItemLocKey,t_CachedItem*,LocKeyHash,LocKeyEqual> itemCache;

void clearItemCache()
{
    // MEMORY LEAK - need to free the stored pointers here
	itemCache.clear();
}

void clearCachedItem(uint32_t x,uint32_t y,uint32_t z)
{
    ItemLocKey lookup;
    lookup.x = x;
    lookup.y = y;
    lookup.z = z;
    delete (itemCache[lookup]);
    //ugh, gotta be a way to get rid of it altogether
    itemCache[lookup] = NULL;
}

t_CachedItem* getCachedItem(uint32_t x, uint32_t y, uint32_t z)
{
    ItemLocKey lookup;
    lookup.x = x;
    lookup.y = y;
    lookup.z = z;
    return itemCache[lookup];
}

void handleItem(df_item *item)
{
    ItemLocKey lookup;
    lookup.x = item->x;
    lookup.y = item->y;
    lookup.z= item->z;
    t_CachedItem* cached = itemCache[lookup];
    if (cached == NULL)
    {
        cached = new t_CachedItem;
        itemCache[lookup] = cached;
    }
    cached->itemType = item->getType();
    cached->matType = item->getMaterial();
    cached->matIndex = item->getMaterialIndex();
    cached->flags = 0;
    cached->itemIndex=0;
    cached->itemID=0;
    cached->cachePass=0;
    cached->fullPass=0;
}

// overly simple atm
void ReadItems(DFHack::Core& DF)
{
    //TODO error checking
    DFHack::Items *Items = DF.getItems();
    vector<df_item *> itemlist;
    Items->readItemVectorSubset(itemlist, search_offset, config.item_search_rate);
    for (std::size_t i=0; i < itemlist.size(); i++)
    {
        handleItem(itemlist[i]);
    }
    if (itemlist.size()<config.item_search_rate)
    {
        WriteErr("Looped on search at %d\n",(search_offset+itemlist.size()));
        search_offset=0;
    }
    else
    {
        search_offset += config.item_search_rate;
    }
}

void DrawItem(int drawx, int drawy, t_CachedItem& item )
{
	t_SpriteWithOffset sprite = GetItemSpriteMap( item );
	if (sprite.sheetIndex == INVALID_INDEX)
		return;
  	ALLEGRO_BITMAP* itemSheet;
    if (sprite.fileIndex == INVALID_INDEX)
    {
    	itemSheet = IMGObjectSheet;
	}
    else
    {
    	itemSheet = getImgFile(sprite.fileIndex);
	} 
	DrawSpriteFromSheet( sprite.sheetIndex, itemSheet, al_map_rgb(255,255,255), drawx, drawy );
}

t_SpriteWithOffset GetItemSpriteMap( t_CachedItem& item )
{	
	vector<ItemConfiguration>* testVector;
	/*if (item.flags & item_hidden_flag)
	{
		if (item.fullPass)
			return spriteItem_None;
		return spriteItem_NA;
	}*/
	uint32_t num = (uint32_t)contentLoader->itemConfigs.size();
	if (item.itemType >= num || item.itemType < 0)
	{
		return spriteItem_NA;
	}
	testVector = contentLoader->itemConfigs[item.itemType];
	if (testVector == NULL || testVector->size() == 0)
	{
		return spriteItem_NA;
	}
	int maxv = testVector->size();
	int mat = item.matType;
	for (int i=0;i<maxv;i++)
	{
		if (mat == (*testVector)[i].matType || (*testVector)[i].matType == INVALID_INDEX)
			return (*testVector)[i].sprite;
	}
	return spriteItem_NA;
}

void pushItemConfig( vector<vector<ItemConfiguration>*>& knownItems, int gameID, ItemConfiguration& iconf)
{
	//get or make the vector to store it
	vector<ItemConfiguration>* itemVector;
	if (knownItems.size() <= gameID)
	{
		//resize using hint from creature name list
		int newsize = gameID +1;
		// wont work yet
		//if (newsize <= contentLoader.itemNameStrings.size())
		//{
		//	newsize = contentLoader.itemNameStrings.size() + 1;
		//}
		knownItems.resize(newsize,NULL);
	}
	itemVector = knownItems[gameID];
	if (itemVector == NULL)
	{
		itemVector = new vector<ItemConfiguration>;
		knownItems[gameID] = itemVector;
	}
	//add a copy to known items
	itemVector->push_back(iconf);
}

int getItemnameFromString(const char* strType)
{
  /*for (uint32_t i=0; i<contentLoader->buildingNameStrings.size(); i++){
		if (contentLoader->buildingNameStrings[i].compare(strType) == 0)
		{
			return i;
		}
	}*/
	return INVALID_INDEX;	
}

bool addSingleItemConfig( TiXmlElement* elemItem, vector<vector<ItemConfiguration>*>& knownItems, int basefile ){
	// names are stored in with buildings at the moment...
	/*int gameID = getItemnameFromString(elemItem->Attribute("gameID"));
	if (gameID == INVALID_INDEX)
	{
		WriteErr("Item name %d not recognised\n",elemItem->Attribute("gameID"));
		contentError("in item config",elemItem);
		return false;
	}*/
	//no item names yet!
	const char* gameIdStr = elemItem->Attribute("gameID");
	int gameID = atoi(gameIdStr);
	if (gameID == 0 && gameIdStr[0] != '0')
		return false;
	const char* sheetIndexStr;
	t_SpriteWithOffset sprite;
	sprite.fileIndex=basefile;
	sprite.x=0;
	sprite.y=0;
	sprite.animFrames=ALL_FRAMES;
	const char* filename = elemItem->Attribute("file");
	if (filename != NULL && filename[0] != 0)
	{
		sprite.fileIndex = loadConfigImgFile((char*)filename,elemItem);
	}
	int mattype = INVALID_INDEX;
	const char* matname = elemItem->Attribute("material");
	if (matname != NULL && matname[0] != 0)
	{
		mattype = lookupMaterialType(matname);
		if (mattype == INVALID_INDEX)
		{
			WriteErr("Material name %d not recognised\n",matname);
			contentError("in item config",elemItem);			
		}
	}
	//create default config
	sheetIndexStr = elemItem->Attribute("sheetIndex");
	sprite.animFrames = ALL_FRAMES;
	sprite.sheetIndex = atoi( sheetIndexStr );
	ItemConfiguration iconf(sprite, mattype, INVALID_INDEX);
	pushItemConfig(knownItems, gameID, iconf);
	return true;
}

bool addItemsConfig( TiXmlElement* elemRoot, vector<vector<ItemConfiguration>*>& knownItems )
{
  int basefile = -1;
  const char* filename = elemRoot->Attribute("file");
  if (filename != NULL && filename[0] != 0)
  {
	basefile = loadConfigImgFile((char*)filename,elemRoot);
  } 
  TiXmlElement* elemItem = elemRoot->FirstChildElement("item");
  if (elemItem == NULL)
  {
     contentError("No items found",elemRoot);
     return false;
  }
  while( elemItem ){
	addSingleItemConfig(elemItem,knownItems,basefile );
	elemItem = elemItem->NextSiblingElement("item");
  }
  return true;
}

ItemConfiguration::ItemConfiguration(t_SpriteWithOffset &sprite, int matType, int matIndex)
{
	this->sprite=sprite;
	this->matType=matType;
	this->matIndex=matIndex;
}

ItemConfiguration::~ItemConfiguration(void)
{}