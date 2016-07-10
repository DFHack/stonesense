
#include "RemoteClient.h"
#include "RemoteFortressReader.pb.h"

class DFConnection
{
	~DFConnection();

	////RPC Functions
	//Unchanging Stuff
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::MaterialList>* materialListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::PlantRawList>* plantRawListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::CreatureRawList>* creatureRawListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::MaterialList>* itemListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::TiletypeList>* tiletypeListCall;

	//Regular Updates
	DFHack::RemoteFunction<RemoteFortressReader::BlockRequest, RemoteFortressReader::BlockList>* blockListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::UnitList>* unitListCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::ViewInfo>* viewInfoCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::MapInfo>* mapInfoCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage>* mapResetCall;
	DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::BuildingList>* buildingListCall;
	DFHack::RemoteClient networkClient;

	////Variable Storage

};