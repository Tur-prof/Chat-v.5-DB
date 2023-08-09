#include "Client.h"

int main() {

    Client client;

    if (client.createSock())
    {
        client.connectServer();
    }
    client.~Client();
    return 0;
}
