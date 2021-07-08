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

    // Extract method from the header
    std::string firstLine = (*_pHeaderList)[0];
    size_t firstSpacePos = firstLine.find_first_of(" ");
    std::string method = firstLine.substr(0, firstSpacePos);
    _method = method;

    // Extract hostname and port number from the header
    // if (_method == "CONNECT")
    // {
    startPos = firstLine.find_first_of(" ");
    size_t midPos = firstLine.find_first_of(":");
    size_t endPos = firstLine.find_last_of(" ");

    std::string hostUrl = firstLine.substr(startPos + 1, midPos - startPos - 1);
    std::string hostPort = firstLine.substr(midPos+1, endPos - midPos - 1);

    size_t len = hostUrl.length();
    char *pHostName = new char[len + 1];
    memset(pHostName, 0, len + 1);
    for (size_t i = 0; i < len; i++)
    {
        pHostName[i] = hostUrl.at(i);
    }
    _pHostName = pHostName;

    size_t lenP = hostPort.length();
    char *pHostPort = new char[lenP + 1];
    memset(pHostPort, 0, lenP + 1);
    for (size_t i = 0; i < lenP; i++)
    {
        pHostPort[i] = hostPort.at(i);
    }
    _pHostPort = pHostPort;
    
    // }
    // else
    // {
    //     for (size_t i = 0; i < _pHeaderList->size(); i++)
    //     {
    //         std::string line = _pHeaderList->at(i);
    //         if (line.find_first_of("Host:") != std::string::npos)
    //         {
    //             if (line.find_first_of(" ") != std::string::npos)
    //             {
                    
    //             }
    //         }
    //     }
    // }
}


bool Connection::isHTTPS()
{
    if (_method == "CONNECT")
    {
        return true;
    }
    return false;
}


int Connection::connectHost(int proxySocket)
{
    struct addrinfo hints, *results, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if (getaddrinfo(_pHostName, _pHostPort, &hints, &results) != 0)
    {
        return -1; // Cannot resolve host name or port
    }

    for (rp = results; rp != NULL; rp = rp->ai_next)
    {
        if (connect(proxySocket, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            int hostSocket = socket(AF_INET, SOCK_STREAM, 0);
            bind(hostSocket, rp->ai_addr, rp->ai_addrlen);
            
            return hostSocket;
        }
    }

    return -1; // Cannot resolve host name or port
}


Connection::~Connection()
{
    delete _pHeader;
    delete _pHeaderList;
    delete[] _pHostName;
    delete[] _pHostPort;
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


bool endsWith(std::vector<char> const& fullString, std::vector<char> const& ending) 
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


void handleConn(int clientSocket, int proxySocket)
{
    Connection conn(clientSocket);
    if (!(conn._pHeader)->size())
    {
        close(clientSocket);
    }
    
    int hostSocket = conn.connectHost(proxySocket);
    try
    {
        if (hostSocket == -1)
        {
            throw -1; // Cannot connect to host socket
        }
        
        // Setup message transmission from client to remote in case of HTTPS protocal
        if (conn.isHTTPS())
        {
            std::string message = "HTTP/1.0 200 Connection Established\r\n\r\n";
            if (send(clientSocket, message.c_str(), message.size() + 1, 0) == -1)
            {
                throw -2; // Cannot send message to client socket
            }
            std::thread c2h(communicate, hostSocket, clientSocket);
        }
        std::thread h2c(communicate, clientSocket, hostSocket);
    }
    catch(int e)
    {
        close(clientSocket);
        close(hostSocket);
        if (e == -1)
        {
            std::cout << "Cannot connect to host\n";
        }
        else if (e == -2)
        {
            std::cout << "Cannot echo message back to client\n";
        }
    }
}


void communicate(int socketOut, int socketIn)
{
    char buf[4096];
    memset(buf, 0, 4096);
    
    while (true)
    {
        int nBytesRecv = recv(socketIn, buf, 4096, 0);
        if (nBytesRecv)
        {
            return;
        }
        send(socketOut, buf, 4096, 0);
    }

    // Something bad happened ...
    return;
}