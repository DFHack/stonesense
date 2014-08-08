#include "ConnectionState.h"
#include <iostream>

ConnectionState::ConnectionState() {
    df_network_out = new DFHack::color_ostream_wrapper(std::cout);
    network_client = new DFHack::RemoteClient(df_network_out);
    is_connected = network_client->connect();
    if (!is_connected) return;
    MaterialListCall.bind(network_client, "GetMaterialList", "RemoteFortressReader");
    GrowthListCall.bind(network_client, "GetGrowthList", "RemoteFortressReader");
    BlockListCall.bind(network_client, "GetBlockList", "RemoteFortressReader");
    HashCheckCall.bind(network_client, "CheckHashes", "RemoteFortressReader");
    TiletypeListCall.bind(network_client, "GetTiletypeList", "RemoteFortressReader");
}

ConnectionState::~ConnectionState() {
    network_client->disconnect();
    delete network_client;
    delete df_network_out;
}

void ConnectionState::Connect()
{
    connection_state = new ConnectionState;
    if (!connection_state->is_connected)
    {
        delete connection_state;
        connection_state = NULL;
    }
}

void ConnectionState::Disconnect()
{
    if (connection_state)
    {
        delete connection_state;
        connection_state = NULL;
    }
}

ConnectionState *connection_state = NULL;
