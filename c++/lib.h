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

void communicate(int proxyOut, int proxyIn);    // TODO
int setupProxy(const std::string *p_ip, int port);
int acceptClient(int proxySocket);
bool endsWith(std::vector<char> const& fullString, std::vector<char> const& ending);
void handleConn(int clientSocket);

class Connection
{
public:
    int _clientSocket;
    std::vector<char> *_pHeader;
    std::vector<std::string> *_pHeaderList;
    std::vector<char> _hostIp;
    std::vector<char> _hostPort;
    
    Connection(int clientSocket);
    char *getHostIp();  // TODO
    int getHostPort();  // TODO
    bool isSSL();       // TODO
};

#endif