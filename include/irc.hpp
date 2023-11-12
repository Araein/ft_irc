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
#include <map>
#include <iterator>
#include <errno.h>



#define bufferSize 1024
#define commandSize 60
#define minPort 49152
#define maxPort 65535
#define maxFD 2

class server;
class client;
class channel;

typedef struct infoSocket{
	socklen_t sizeAddr;
	struct sockaddr_in Addr;
}infoSocket;


#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"

