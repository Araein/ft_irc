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
#include <algorithm>
#include <cctype>

#define bufferSize 4096
#define minPort 49152
#define maxPort 65535
#define maxFD 2


class server;
class client;
class channel;

#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"


bool IsNotSpace(int ch);


