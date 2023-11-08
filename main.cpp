#include "ircserv.hpp"

int main (int ac, char **av){
	(void)ac;
	(void)av;
    int sock_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int client_fd =  0;
	struct sockaddr *my_addr = new struct sockaddr;
	socklen_t len = sizeof(struct sockaddr);

    if (sock_tcp == -1)
    {
        std::cerr << "Error creating server socket." << std::endl;
        return -1;
    }
	my_addr->sa_family = AF_INET;
	if (bind(sock_tcp, my_addr, len) == -1){
		std::cerr << "Error binding socket."  << std::endl;
		return -1;
	}
	if (listen(sock_tcp, 50) == -1){
		std::cerr << "Error listen." << std::endl;
		return -1;
	}
	while (client_fd == 0){
		client_fd = accept(sock_tcp, my_addr, &len);
	}
	if (client_fd == -1)
	{
		std::cerr << "Error acceptin client" << std::endl;
		return -1;
	}
    return 0;
}