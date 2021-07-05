#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <vector>


class Connection
{
public:
    
    int _clientSocket;
    std::vector<char> *_pHeader;
    std::vector<char> _hostIp;
    std::vector<char> _hostPort;
    
    
    Connection(int clientSocket);
}


int setupProxy(const std::string *p_ip , int port)
{
    int proxySocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in proxyAddr;
    inet_pton(AF_INET, (*p_ip).c_str(), &proxyAddr.sin_addr);
    proxyAddr.sin_port = htons(port);
    proxyAddr.sin_family = AF_INET;
    
    bind(proxySocket, AF_INET, (struct sockaddr *)&proxyAddr, sizeof(proxyAddr));

    listen(proxySocket, SOMAXCONN);

    std::cout << "http proxy server running at " << *p_ip << ":" << port << std::endl;

    return proxySocket;
}


int acceptClient(int proxySocket)
{
    struct sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    
    int clientSocket = accept(proxySocket, (struct sockaddr *)&client, &clientSize);
    return 0;
}





bool endsWith (std::vector<char> const& fullString, std::vector<char> const& ending) {
    if (fullString.size() >= ending.size()) {
        size_t pos = fullString.size() - ending.size();
        for (size_t i{pos}; i < fullString.size(); i++)
        {
            if (fullString[i] != ending[i-pos])
            {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void handleConn(int clientSocket)
{
    Connection conn(clientSocket);
    if (!*(conn._pHeader).size())
    {
        close(clientSocket);
    }

    
    return;
}

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


Connection::Connection(int clientSocket):
_clientSocket{clientSocket}
{
    std::vector<char> *p_header = new std::vector<char>;
    char buf[4096];
    memset(buf, 0, 4096);
    while (true)
    {
        int nBytesRecv = recv(_clientSocket, buf, 4096, 0);
        for (int i{0}; i < nBytesRecv; i++)
        {
            (*p_header).push_back(buf[i]);
        }

        std::vector<char> ending{13, 10, 13, 10}; // "/r/n/r/n"
        if (endsWith(*p_header, ending) || nBytesRecv == 0)
        {
            break;
        }

        memset(buf, 0, 4096);
    }

    _pHeader = p_header;
    _hostIp{};
    _hostPort{};
}