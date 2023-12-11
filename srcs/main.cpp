#include "irc.hpp"

server *srv = NULL;

void sig_int(int signum)
{
	(void)signum;
	srv->closeAll();
	delete srv;
	exit(0);
}

static int parsePort(std::string port)
{
	int val = 0;
	for (size_t i=0; i < port.size(); i++){
		if (isdigit(port[i]) == 0){
			std::cout << "Invalid port number" << std::endl;
			return 0;
		}
		val = (val * 10) + (port[i] - 48);
		if (val > maxPort){
			std::cout << "Port number is too high\n Between " << minPort << " and " << maxPort << std::endl;
			return 0;
		}
	}
	if (val < minPort){
		std::cout << "Port number is too low\n Between " << minPort << " and " << maxPort << std::endl;
		return 0;
	}
	return val;
}

int main (int ac, char **av)
{
	int port;

	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_int);
	if (ac != 3 || (port = parsePort(av[1])) == false){
		std::cerr << "Error: invalid argument.\nUsage <./ircserv> <port 49152:65535> <password>" << std::endl;
		return 1;
	}
	srv = new server(port, av[2]);
	if (srv->initSocket() == true){
		srv->mainLoop();
	}
	delete srv;
	return 0;
}


