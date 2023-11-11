#pragma once

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <errno.h>



#define bufferSize 1024
#define minPort 49152
#define maxPort 65535
#define maxFD 100

typedef struct infoConnect{
	int id;
	int port;
	std::string nickname;
	std::string name;
	std::string password;
	socklen_t sizeAddr;
	struct pollfd fds;
	struct sockaddr_in Addr;
	char buffer[bufferSize];
}infoConnect;


#include "server.hpp"