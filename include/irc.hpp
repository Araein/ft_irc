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
#define maxFD 5


class server;
class client;
class channel;

#include "server.hpp"
#include "client.hpp"
#include "channel.hpp"


bool IsNotSpace(int ch);
	std::string extract(const std::string& message, const std::string& start, const std::string& end);
	std::string rtrim(const std::string& str);
	std::string ltrim(const std::string& str);


