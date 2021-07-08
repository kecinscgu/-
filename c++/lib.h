#ifndef LIB_H
#define LIB_H

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


// Maintain the dataflow from socketIn to socketOut
// TODO
void communicate(int socketOut, int socketIn);

// Initialize the listening socket for the proxy servers
int setupProxy(const std::string *p_ip, int port);

// Accept the connection from the client socket
int acceptClient(int proxySocket);

// Determine whether the first vector agrees with the second char at its ending positions
bool endsWith(std::vector<char> const& fullString, std::vector<char> const& ending);

// Handle the connection from some client socket
void handleConn(int clientSocket);

class Connection
{
public:
    int _clientSocket;
    std::vector<char> *_pHeader;
    std::vector<std::string> *_pHeaderList;
    std::string _method;
    char *_pHostName;
    char *_pHostPort;
    
    Connection(int clientSocket);
    ~Connection();
    bool isHTTPS();       // TODO
    int connectHost(int proxySocket);
};

#endif