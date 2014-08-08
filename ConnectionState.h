
#include <stdint.h>
#include "RemoteFortressReader.pb.h"
#include "RemoteClient.h"

class ConnectionState {
public:
    bool is_connected;
    RemoteFortressReader::MaterialList net_material_list;
    RemoteFortressReader::BlockList net_block_list;
    RemoteFortressReader::BlockRequest net_block_request;
    RemoteFortressReader::TiletypeList net_tiletype_list;
    dfproto::EmptyMessage empty_message;
    DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::MaterialList> MaterialListCall;
    DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::MaterialList> GrowthListCall;
    DFHack::RemoteFunction<RemoteFortressReader::BlockRequest, RemoteFortressReader::BlockList> BlockListCall;
    DFHack::RemoteFunction<dfproto::EmptyMessage> HashCheckCall;
    DFHack::RemoteFunction<dfproto::EmptyMessage, RemoteFortressReader::TiletypeList> TiletypeListCall;
    DFHack::color_ostream * df_network_out;
    DFHack::RemoteClient * network_client;

    ConnectionState();
    ~ConnectionState();

    static void Connect();
    static void Disconnect();
};

extern ConnectionState *connection_state;
