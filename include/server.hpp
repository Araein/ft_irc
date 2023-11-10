#pragma once

#include "irc.hpp"

class server 
{
private:
    int _server;
    int _client[3]; // Tableau pour les descripteurs de fichiers des clients
    struct sockaddr_in _serverAddr;
    struct sockaddr_in _clientAddr[3]; // Tableau pour les adresses des clients
    
    /*variables de poll*/
    int _pollResult;
    struct pollfd _fds[4];

    socklen_t _sizeAddr;
    //char _buff[1024]; // faire un buff par client?
    int _port;
	std::string _pass;
	//ssize_t _sizeRead;
    void mainloop();
    void setupPoll();

public:
    server(void);
    ~server();
    void setPort(int port);
    void setPassword(std::string pass);
    int getServer(void);
    int getClient(int index);
    void stopServer(void);
    void initServer();
};
