#include "lib.h"


int main()
{
    // setup a listening socket for the proxy server
    std::string p = "127.0.0.1";
    int port = 3000;
    int proxySocket = setupProxy(&p, port);
    
    // wait for incoming client connectioins
    while (true)
    {
        int clientSocket = acceptClient(proxySocket);
        std::thread conn(handleConn, clientSocket);
    }

    return 0;
}
