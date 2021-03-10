#include "ConnectionState.h"

#include <iostream>

#include "MiscUtils.h"

using namespace std;

ConnectionState::ConnectionState() {
    df_network_out = dts::make_unique<DFHack::color_ostream_wrapper>(std::cout);
    network_client = dts::make_unique<DFHack::RemoteClient>(df_network_out.get());
    is_connected = network_client->connect();
    if (!is_connected) return;

    MaterialListCall.bind(network_client.get(), "GetMaterialList", "RemoteFortressReader");
    BlockListCall.bind(network_client.get(), "GetBlockList", "RemoteFortressReader");
}

ConnectionState::~ConnectionState() {
    network_client->disconnect();
}

ConnectionState* ConnectionState::Connect()
{
    connection_state = dts::make_unique<ConnectionState>();
    if (!connection_state->is_connected)
    {
        connection_state.reset();
    }

    return connection_state.get();
}

void ConnectionState::Disconnect()
{
    connection_state.reset();
}

std::unique_ptr<ConnectionState> ConnectionState::connection_state;
