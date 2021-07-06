#include "lib.h"

Connection::Connection(int clientSocket):
_clientSocket(clientSocket)
{
    // Receive header bytes from client
    std::vector<char> *p_header = new std::vector<char>;
    char buf[4096];
    memset(buf, 0, 4096);
    while (true)
    {
        int nBytesRecv = recv(_clientSocket, buf, 4096, 0);
        for (int i = 0; i < nBytesRecv; i++)
        {
            (*p_header).push_back(buf[i]);
        }

        std::vector<char> ending{'\r', '\n', '\r', '\n'}; 
        if (endsWith(*p_header, ending) || nBytesRecv == 0)
        {
            break;
        }

        memset(buf, 0, 4096);
    }

    _pHeader = p_header;

    // Split header into header list (separated by '\r\n')
    size_t startPos = 0;
    std::vector<std::string> *pHeaderList = new std::vector<std::string>;
    for (size_t i = 0; i < p_header->size(); i++)
    {
        if (i == p_header->size() - 1)
        {
            break;
        }
        
        if ((*p_header)[i] == '\r' && (*p_header)[i+1] == '\n')
        {
            // combine the entries within the vector between startPos and i-1 into a string
            std::string headerEntry = "";
            for (size_t j = startPos; j < i; j++)
            {
                headerEntry.push_back((*p_header)[j]);
            }

            pHeaderList->push_back(headerEntry);
            startPos = i + 2;
        }
    }

    _pHeaderList = pHeaderList;
}


char *Connection::getHostIp()
{
    return NULL;
}


int Connection::getHostPort()
{
    return 0;
}


bool Connection::isSSL()
{
    return false;
}


int setupProxy(const std::string *p_ip , int port)
{
    int proxySocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in proxyAddr;
    inet_pton(AF_INET, p_ip->c_str(), &proxyAddr.sin_addr);
    proxyAddr.sin_port = htons(port);
    proxyAddr.sin_family = AF_INET;
    
    bind(proxySocket, (struct sockaddr *)&proxyAddr, sizeof(proxyAddr));

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


bool endsWith (std::vector<char> const& fullString, std::vector<char> const& ending) 
{
    if (fullString.size() >= ending.size()) {
        size_t pos = fullString.size() - ending.size();
        for (size_t i = pos; i < fullString.size(); i++)
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
    if (!(conn._pHeader)->size())
    {
        close(clientSocket);
    }

    int remoteSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in remoteAddr;
    inet_pton(AF_INET, conn.getHostIp(), &remoteAddr.sin_addr);
    remoteAddr.sin_port = htons(conn.getHostPort());
    remoteAddr.sin_family = AF_INET;
    try
    {
        if (connect(remoteSocket, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr)) == -1)
        {
            throw -1;
        }
        
        // Setup message transmission from client to remote in case of HTTPS protocal
        if (conn.isSSL())
        {
            std::string message = "HTTP/1.0 200 Connectioin Established\r\n\r\n";
            if (send(clientSocket, message.c_str(), message.size() + 1, 0) == -1)
            {
                throw -2;
            }
            std::thread c2r(communicate, clientSocket, remoteSocket);
        }

        std::thread r2c(communicate, remoteSocket, clientSocket);
    }
    catch(int e)
    {
        close(clientSocket);
        close(remoteSocket);
        if (e == -1)
        {
            std::cout << "Cannot connect to remote host\n";
        }
        else if (e == -2)
        {
            std::cout << "Cannot echo message back to client\n";
        }
    }

}


void communicate(int proxyOut, int proxyIn)
{
    return;
}