#include "server.hpp"

server::server(void): _server(0)
{ 
	_client[0] = -1;
    _client[1] = -1;
    _client[2] = -1;

	std::cout << "[SERVER: INITIALISATION]" << std::endl; 
}

server::~server() {}
// server::server(server const &copy) {}
// server &server::operator=(server const &aff) {}

void server::setPort(int port) { _port = port; }
void server::setPassword(std::string pass) { _pass = pass; }

int server::getServer(void) { return _server; }
int server::getClient(int i) { return _client[i]; }

void server::stopServer(void)
{
	int i = 0;
	while (i < 3)
	{
		if (_client[i] > 0)
			close(_client[i]);
		_client[i] = -1; // -2?
		i++;
	}
	if (_server > 0)
		close(_server);
	_server = -1;
	std::cout << "[SERVER: DISCONNECTED]" << std::endl;
}

void server::initServer()
{

//Creation d'un socket
	_server = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_server < 0){
			std::cout << "[SERVER: CONNECTION FAILLED]" << std::endl;
			return;
	}
	std::cout << "[SERVER: CONNECTED]" << std::endl;


// Configuration de l'adresse du serveur
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY; // ou inet_addr("127.0.0.1");
	_serverAddr.sin_port = htons(_port);

// Liaison du socket avec le port et l'adresse IP
	if (bind(_server, (sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
	{
			std::cout << "[SERVER: FAILED TO BIND]" << std::endl;
			return;
	}
	std::cout << "[SERVER: BINDING]" << std::endl;

// Tentative d'ecoute des connexions entrantes
	if (listen(_server, SOMAXCONN) < 0)
	{
			std::cout << "[SERVER: FAILED TO LISTENING]" << std::endl;
			return;
	}
	std::cout << "[SERVER: LISTENING ON PORT " << _port << "]" << std::endl;
	setupPoll();
	mainloop();
}

void server::setupPoll()
{
    _fds[0].fd = _server;
    _fds[0].events = POLLIN;

    for (int i = 1; i <= 3; ++i) 
	{
        _client[i - 1] = -1;
        _fds[i].fd = _client[i - 1];
        _fds[i].events = POLLIN;
    }
	_pollResult = poll(_fds, 4, -1);
}

void server::sendMsgToClients(char *buffer, int n){
	std::stringstream tmp;
	std::string str = "Client ";
	tmp << n << " dit: "; //a changé vers nickname dit
	for (int i = 0; i <= 2; i++){
		str.append(tmp.str()); 
		str.append(buffer);
		if (i != n)
			send(_client[i], str.c_str(), str.length(),0);
	}
}

void server::mainloop()
{
	int i = 0;

    while (true) 
	{
        if (_pollResult == -1)
		{
            std::cerr << "Erreur lors de l'appel à poll" << std::endl;
            break;
        }
		/*recuperation des nouveaux clients*/
		int p = 0;
        if (p < 4 && _fds[0].revents & POLLIN)
		{
            while (i < 3) 
			{
                if (_client[i] == -1) 
				{
                    _sizeAddr = sizeof(_clientAddr[i]);	
                    _client[i] = accept(_server, (sockaddr*)&_clientAddr[i], &_sizeAddr);
                    // if (_client[i] == -1)
					// {
                    //     std::cerr << "Erreur lors de l'acceptation de la connexion" << std::endl;
                    // }
					if (_client[i] > 0)
                    	std::cout << "[SERVER: ACCEPTED CONNECTION FROM " << inet_ntoa(_clientAddr[i].sin_addr) << "]" << std::endl;
					send(_client[i], "Coucou bienvenue sur IRC\n", 25, 0);
					break;
                }
				i++;
            }
        }

		/*lecture des donnees des clients*/
		for (int j = 1; j <= 3; ++j) 
		{
			if (_client[j - 1] != -1 && (_fds[j].events & POLLIN))		
			{
				char buffer[bufferSize];
				ssize_t bytesRead = recv(_client[j - 1], buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

				// if (bytesRead <= 0)
				// {
				// 	// Gestion de la déconnexion du client à changer
				// 	//std::cout << "[CLIENT " << j << "BREAK " << std::endl;
				// 	// close(_client[j - 1]);
				// 	// _client[j - 1] = -1;
				// 	//break;
				// }
				if (bytesRead > 0)
				{
					buffer[bytesRead] = '\0';
					std::cout << "[CLIENT " << j << "]: " << buffer << std::endl;
					sendMsgToClients(buffer, j - 1);
				}
			}
		}
    }
}
