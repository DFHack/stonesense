#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "BuildingConfiguration.h"
#include "BlockCondition.h"
#include "dfhack/library/tinyxml/tinyxml.h"
#include "GUI.h"

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent);

bool parseRecursiveNodes (ConditionalNode* pnode, TiXmlElement* pelem)
{
	TiXmlElement* elemCondition = pelem->FirstChildElement();
	while( elemCondition )
	{
		if (!parseConditionNode( pnode, elemCondition, false ))
			return false;
		elemCondition = elemCondition->NextSiblingElement();
	}
	return true;
}

int parseConditionNode(ConditionalNode* node, TiXmlElement* elemCondition, bool silent){
   const char* strType = elemCondition->Value();
   BlockCondition* cond = NULL;	
  if( strcmp(strType, "NeighbourWall") == 0){
     cond = new NeighbourWallCondition( elemCondition->Attribute("dir") );
  }
  
  else if( strcmp(strType, "PositionIndex") == 0){
     cond = new PositionIndexCondition( elemCondition->Attribute("value") );
  } 
  
  else if( strcmp(strType, "MaterialType") == 0){
     cond = new MaterialTypeCondition( elemCondition->Attribute("value") );
  }   
  else if( strcmp(strType, "always") == 0){
     cond = new AlwaysCondition();
  } 
  
  else if( strcmp(strType, "never") == 0){
    cond = new NeverCondition();
  }     
 
  else if( strcmp(strType, "BuildingOccupancy") == 0){
     cond = new BuildingOccupancyCondition( elemCondition->Attribute("value") );
  } 
  
  else if( strcmp(strType, "NeighbourSameBuilding") == 0){
    cond = new NeighbourSameBuildingCondition( elemCondition->Attribute("dir") );
  }     
  
  else if( strcmp(strType, "NeighbourSameType") == 0){
    cond = new NeighbourSameTypeCondition( elemCondition->Attribute("dir") );
  }
  
  else if( strcmp(strType, "NeighbourOfType") == 0){
    cond = new NeighbourOfTypeCondition( elemCondition->Attribute("dir") , elemCondition->Attribute("value") );
  }
  
  else if( strcmp(strType, "NeighbourIdentical") == 0){
    cond = new NeighbourIdenticalCondition( elemCondition->Attribute("dir") );
  }   
  
  else if( strcmp(strType, "NeighbourIdentical") == 0){
    cond = new NeighbourIdenticalCondition( elemCondition->Attribute("dir") );
  }    
  
  else if( strcmp(strType, "AnimationFrame") == 0){
    cond = new AnimationFrameCondition( elemCondition->Attribute("value") );
  }  
  
  else if( strcmp(strType, "HaveFloor") == 0){
     cond = new HaveFloorCondition();
  }

  else if( strcmp(strType, "and") == 0){
	AndConditionalNode* andNode = new AndConditionalNode();
	cond = andNode;
	if (!parseRecursiveNodes(andNode, elemCondition))
	{
		delete(andNode);
		return 0;
	}
  }
  
   else if( strcmp(strType, "or") == 0){
	OrConditionalNode* orNode = new OrConditionalNode();
	cond = orNode;
	if (!parseRecursiveNodes(orNode, elemCondition))
	{
		delete(orNode);
		return 0;
	}
  }

  else if( strcmp(strType, "not") == 0){
	NotConditionalNode* notNode = new NotConditionalNode();
	cond = notNode;
	if (!parseRecursiveNodes(notNode, elemCondition))
	{
		delete(notNode);
		return 0;
	}
  }
  
	if (cond != NULL)
	{ 
		if (!node->addCondition( cond ))
		{
			delete(cond);
			return 0;	
		}
		return 1;
	}
	else if (!silent)
	{
		WriteErr("Misplaced or invalid element in Condition: %s (Line %d)\n",strType,elemCondition->Row());
		return 0;		  
  	}
  	return -1;
}

bool parseSpriteNode(SpriteNode* node, TiXmlElement* elemParent)
{
	SpriteBlock* oldSibling = NULL;
	TiXmlElement* elemNode =  elemParent->FirstChildElement();
	const char* strParent = elemParent->Value();
	if (elemNode == NULL)
	{
		WriteErr("Empty SpriteNode Element: %s (Line %d)\n",strParent,elemParent->Row());
		return false;		
	}
	if ( strcmp(strParent,"building") != 0 && strcmp(strParent,"rotate") != 0)
	{
		//flag to allow else statements to be empty, rather than needing an "always" tag
		bool allowBlank = (strcmp(strParent,"else") == 0 || elemParent->Attribute("else"));
		// cast should be safe, because only spriteblocks
		// should get here
		int retvalue =parseConditionNode((SpriteBlock *)node,elemNode,allowBlank);
		if (retvalue == 0)
			return false;
		if (retvalue > 0)
			elemNode = elemNode->NextSiblingElement();
	}
	while (elemNode)
	{
		bool match = false;
		const char* strType = elemNode->Value();
		if (strcmp(strType, "if") == 0 || strcmp(strType, "else") == 0)
		{
			SpriteBlock* block = new SpriteBlock();
			if (!elemNode->Attribute("file") && elemParent->Attribute("file"))
			{
				elemNode->SetAttribute("file",elemParent->Attribute("file"));
			}
			if (!parseSpriteNode(block,elemNode))
			{
				delete(block);
				return false;
			}
			if (elemNode->Attribute("else") || strcmp(strType, "else") == 0)
			{
				if (!oldSibling)
				{
					WriteErr("Misplaced or invalid element in SpriteNode: %s (Line %d)\n",strType,elemNode->Row());
					return false;						
				}
				oldSibling->addElse(block);	
			}
			else
			{
				node->addChild(block);
			}
			oldSibling = block;
		}
		else if (strcmp(strType, "rotate") == 0)
		{
			RotationBlock* block = new RotationBlock();
			if (!elemNode->Attribute("file") && elemParent->Attribute("file"))
			{
				elemNode->SetAttribute("file",elemParent->Attribute("file"));
			}
			if (!parseSpriteNode(block,elemNode))
			{
				delete(block);
				return false;
			}
			else
			{
				node->addChild(block);
			}
			oldSibling = NULL;
		}
		else if ((strcmp(strType, "sprite") == 0) || (strcmp(strType, "empty") == 0))
		{
			SpriteElement* sprite = new SpriteElement();
			const char* strSheetIndex = elemNode->Attribute("index");
			const char* strOffsetX = elemNode->Attribute("offsetx");
			const char* strOffsetY = elemNode->Attribute("offsety");
			const char* filename = elemNode->Attribute("file");
			sprite->sprite.sheetIndex = (strSheetIndex != 0 ? atoi(strSheetIndex) : -1);
			sprite->sprite.x    = (strOffsetX    != 0 ? atoi(strOffsetX)    : 0);
			sprite->sprite.y   = (strOffsetY    != 0 ? atoi(strOffsetY)    : 0);
			sprite->sprite.fileIndex = -1;
			if (filename != NULL)
			{
			  sprite->sprite.fileIndex = loadImgFile((char*)filename);
			}
			else
			{
			  const char* pfilename = elemParent->Attribute("file");
			  if (pfilename)
			  {
				sprite->sprite.fileIndex = loadImgFile((char*)pfilename);
			  }
			}
			node->addChild(sprite);
		}
		else
		{
			WriteErr("Misplaced or invalid element in SpriteNode: %s (Line %d)\n",strType,elemNode->Row());
			return false;
		}		
		elemNode = elemNode->NextSiblingElement();
	}
	return true;
}

bool addSingleConfig( const char* filename,  vector<BuildingConfiguration>* knownBuildings ){
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  TiXmlHandle hDoc(&doc);
  TiXmlElement* elemBuilding;
  if(!loadOkay)
  {
	WriteErr("File load failed\n");
	WriteErr("Line %d: %s\n",doc.ErrorRow(),doc.ErrorDesc());
	return false;
  }

  elemBuilding = hDoc.FirstChildElement("building").Element();
  if( elemBuilding == 0)
  {
	  WriteErr("Main <building> node not present\n");
    return false;
  }

  const char* strName = elemBuilding->Attribute("name");
  const char* strGameID = elemBuilding->Attribute("gameID");
  
  if (strName[0] == 0 || strGameID[0] == 0)
  {
	  WriteErr("<building> node must have name and gameID attributes\n");
	  return false;
  }
  
  BuildingConfiguration building(strName, (char*) strGameID );
  RootBlock* spriteroot = new RootBlock(); //leaky?
  building.sprites = spriteroot;
  if (!parseSpriteNode(spriteroot,elemBuilding))
  {
	delete(spriteroot);
	return false;
  }
  
  //add a copy of 'building' to known buildings
  knownBuildings->push_back( building );
  return true;
}


bool LoadBuildingConfiguration( vector<BuildingConfiguration>* knownBuildings ){
  string line;
  ifstream myfile ("buildings/index.txt");
  if (myfile.is_open() == false){
    WriteErr("Unable to load building config index file!\n");
    return false;
  }

  // clean up building data trees before deleting them
  // a nasty cludge that only works cause knownbuildings
  // isnt modified anywhere else
  // TODO: look into smart pointers or something
	uint32_t numBuildings = (uint32_t)knownBuildings->size();
	for(uint32_t i = 0; i < numBuildings; i++)
	{
		delete(knownBuildings->at(i).sprites);
		//should set to null, but we will nuke the lot in a second
	}
  knownBuildings->clear();
  //img files are now unused
  flushImgFiles();
  
  while ( !myfile.eof() )
  {
    char filepath[50] = {0};
    getline (myfile,line);
    
    //some systems don't remove the \r char as a part of the line change:
    if(line.size() > 0 &&  line[line.size() -1 ] == '\r' )
      line.resize(line.size() -1);

    if(line.size() > 0){
      sprintf(filepath, "buildings/%s", line.c_str() );
      WriteErr("Reading %s...\t\t", filepath);
      bool result = addSingleConfig( filepath, knownBuildings );
      if (result)
      	WriteErr("ok\n");
      else
        WriteErr("Unable to load building config %s\n(Will ignore building and continue)\n", filepath);
    }
  }
  myfile.close();

  BuildingNamesTranslatedFromGame = false;

  return true;
}

