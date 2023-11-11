#include "irc.hpp"

server *srv = NULL;

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

// static bool parsePass(std::string pass)
// {
// 	if (pass.size() == 0){
// 		std::cout << "Error\nPassword is empty" << std::endl;
// 		return false;
// 	}
// 	return true;
// }

void sig_int(int signum)
{
	(void)signum;
	srv->stopServer();
	delete srv;
	exit(0);
}

int main (int ac, char **av)
{
	signal(SIGINT, sig_int);
	signal(SIGQUIT, SIG_IGN);

	if (ac != 3 || (av[2] && !av[2][0])){
		std::cerr << "Error. Argument invalid or missing" << std::endl;
		return 1;
	}
	int port = parsePort(av[1]);
	if (!port){
		std::cerr << "Error. Invalid port number" << std::endl;
		return 1;
	}
	srv = new server(port, av[2]);
	if (srv->initServer() == true)
		srv->mainloop();
	else
		std::cout << "[SERVER: DISCONNECTED]" << std::endl;
	delete srv;
}

