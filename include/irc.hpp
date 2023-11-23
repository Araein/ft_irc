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
#include <ctime>
#include <iomanip>



#define bufferSize 4096
#define minPort 49152
#define maxPort 65535
#define maxFD 15

#define BLACK  "\033[30m"
#define RED  "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE  "\033[34m"
#define PINK  "\033[35m"
#define CYAN  "\033[36m"
#define GREY  "\033[37m"
#define BOLD  "\033[1m"
#define LINE  "\033[4m"
#define NONE  "\033[0m"

class client;
class server;
class channel;


#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"

bool IsNotSpace(int ch);
std::string extract(const std::string& message, const std::string& start, const std::string& end);
std::string rtrim(const std::string& str);
std::string ltrim(const std::string& str);

