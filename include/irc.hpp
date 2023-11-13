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
#define maxFD 5 //inclure le server


# define RPL_WELCOME(nick) (":Welcome to the Internet Relay Network " + nick + "\r\n")
# define RPL_YOURHOST(servername, version) (":Your host is ircserv running version 0.1\r\n")
# define RPL_MYINFO() (":ircserv 0.1 level0 chan_modeballecouille\r\n")
# define RPL_CREATED() (":This server was created god knows when\r\n");

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

