#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main()
{
    int proxySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxySocket == -1)
    {
        std::cerr << "Can't create a socket!";
        return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(proxySocket, AF_INET, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        std::cerr << "Can't bind to IP/port";
        return -2;
    }

    if (listen(proxySocket, SOMAXCONN) == -1)
    {
        std::cerr << "Can't listen!";
        return -3;
    }

    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST]{};
    char service[NI_MAXSERV];
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    int clientSocket = accept(proxySocket, (sockaddr*)&client, &clientSize)
    if (clientSocket == -1)
    {
        std::cerr << "Problem with client connecting";
        return -4;
    }

    char buf[4096];
    while (True)
    {
        memset(buf, 0, 4096);

        int nBytesRecv = recv(clientSocket, buf, 4096, 0);
        if (nBytesRecv == -1)
        {
            std::cerr << "There was a connection issue" << std::endl;
            break;
        }
        if (nBytesRecv == 0)
        {
            std::cout << "The client disconnected" << std::endl;
            break;
        }
        std::cout << "Received: " << std::string(buf, 0, nBytesRecv) << std::endl;

        send(clientSocket, buf, nBytesRecv + 1, 0);
    }
    
    close(proxySocket);


    return 0;
}