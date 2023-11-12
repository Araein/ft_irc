#include "server.hpp"

// CONSTRUCTORS
server::server(int port, std::string password): 
_id(1000), _port(port), _totalFD(1), _pollResult(0), _bytesRead(0)
{
	(void)password;
	setupPoll();
	_sock.sizeAddr = sizeof(_sock.Addr);
	memset(&_sock.Addr, 0, _sock.sizeAddr);
	memset(&_buffer, 0, bufferSize);
	std::cout << "[SERVER: INITIALISATION]" << std::endl;
}

server::~server() {}

//GETTER

std::string server::getUserName(int fd) { return (mapUser.find(fd))->second.getNickname(); }
int server::getUserLevel(int fd) { return (mapUser.find(fd))->second.getLevel(); }

//SETTER

void server::setUserLevel(int fd, int level) { (mapUser.find(fd))->second.setLevel(level); }

//FONCTIONS INITIALISATION
void server::setupPoll(void)
{
	for (int i = 0; i < maxFD; i++){
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLPRI; // voir comment gerer POLLPRI
		_fds[i].revents = 0;
	}
}

bool server::initServerSocket(void)
{
	_fds[0].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_fds[0].fd < 0){
		std::cout << "[SERVER: CONNECTION FAILLED]" << std::endl;
		return false;
	}
	_sock.Addr.sin_family = AF_INET;
	_sock.Addr.sin_addr.s_addr = INADDR_ANY;
	_sock.Addr.sin_port = htons(_port);
	std::cout << "[SERVER: CONNECTED]" << std::endl;
	return true;
}

bool server::bindServerSocket(void)
{
	if (bind(_fds[0].fd, (sockaddr *)&_sock.Addr, sizeof(_sock.Addr)) < 0){
		std::cout << "[SERVER: FAILED TO BIND]" << std::endl;
		return false;
	}
	std::cout << "[SERVER: BINDING]" << std::endl;
	return true;
}

bool server::listenServerSocket(void)
{
	if (listen(_fds[0].fd, SOMAXCONN) < 0){
		std::cout << "[SERVER: FAILED TO LISTENING]" << std::endl;
		return false;
	}
	std::cout << "[SERVER: LISTENING ON PORT " << _port << "]" << std::endl;
	return true;
}

bool server::initServer()
{
	if (initServerSocket() == false)
		return false;
	if (bindServerSocket() == false || listenServerSocket() == false){
		stopServer();
		return false;
	}
	return true;
}

// bool server::verify_Pwd(infoConnect user){// voir ou le placer car bloque tant que pas de reponse.
// 	std::stringstream tries;
// 	std::string tmp;
// 	send(user.fds.fd, "ENTER PASSWORD\n", 16, 0);
// 	for (int i = 0; i < 3; i++){
// 		int bytesRead = recv(user.fds.fd, _buffer, bufferSize, 0);
// 		if (bytesRead > 0)
// 			_buffer[bytesRead - 1] = '\0';
// 		if (_sock.password.compare(_buffer) == 0)
// 			return (true);
// 		else{
// 			if (3 - (i + 1) == 0)
// 				break;
// 			send(user.fds.fd, "INCORRECT PASSWORD, TRY AGAIN\n", 31, 0);
// 			tries.str("");
// 			tries << (3 - (i + 1));
// 			tmp = tries.str();
// 			send(user.fds.fd, "YOU HAVE ", 10, 0);
// 			send(user.fds.fd, tmp.c_str(), 1, 0);
// 			send(user.fds.fd, " TRIES REMAINING\n", 18, 0);
// 		}
// 	}
// 	send(user.fds.fd, "INVALID PASSWORD, CONNECTION DENIED\n", 37, 0);
// 	return (false);
// }

int server::findCurFD(void)
{
	for (int i = 1; i < _totalFD; i++){
		if (_fds[i].fd == -1)
			return i;
	}
	return _totalFD;
}

void server::cleanFDS(int i)
{
	mapUser.erase(mapUser.find(_fds[i].fd)); 
	if (_fds[i].fd > -1)
		close(_fds[i].fd);
	_fds[i].fd = -1;
	_fds[i].revents = 0;
	_totalFD--;
}

bool server::firstMsg(std::string message)// permettra de gerer le 1er message envoyer par irssi ou le client qu'on choisira
{
	std::cout << message << std::endl;
	return true;
}

bool server::selectCommand(std::string message, int i)
{
	(void)i;
	std::istringstream iss(message);
	std::string command;
	iss >> command;
	if (command == "KICK"){
		//fonction KICK 
		std::cout << command << std::endl;
	}
	else if (command == "INVITE"){
		std::cout << command << std::endl;
	}
	else if (command == "TOPIC"){
		std::cout << command << std::endl;
	}
	else if (command == "MODE"){
		std::cout << command << std::endl;
	}
	else
		return false;
	return true;
}

// void server::sendMsgToClients(char *buffer, int n){ // a placer dans la partie gerant les channels
// 	std::stringstream tmp;
// 	std::string str = "Client ";
// 	tmp << _vect[n].id << " dit: "; //a changé vers nickname dit
// 	str.append(tmp.str()); 
// 	str.append(buffer);
// 	for (int i = 1; i <= _curFD; i++){
// 		if (i != n)
// 			send(_fds[i].fd, str.c_str(), str.length(),0);
// 	}
// }

void server::accept_newUser(void)
{
	_curFD = findCurFD();
	std::cout << _curFD << std::endl;
	_fds[_curFD].fd = accept(_fds[0].fd, (sockaddr*)&_sock.Addr, &_sock.sizeAddr);
	if (_fds[_curFD].fd > 0){
		send(_fds[_curFD].fd, "|---------- WELCOME IN 42_IRC ----------|\n", 42, 0);
		if (_curFD > maxFD){
			send(_fds[_curFD].fd, "42_IRC is full please try again later\n", 38, 0);
			close(_fds[_curFD].fd);
			return;
		}
		_totalFD++;
		client user(_fds);
		mapUser.insert(std::make_pair(_fds[_curFD].fd, user));
		std::cout << "[SERVER: SUCCESS CONNECTION FROM : " << inet_ntoa(_sock.Addr.sin_addr) << "]" << std::endl;
		//obtenir les info du user avant de stocker dans vector
		// if (verify_Pwd(user))
		// 	_vect.push_back(user);
		// else{
		// 	_fds[_curFD].fd = -1;
		// 	_curFD--;
		// 	_id--;
		// }
	}
}

void server::mainloop()
{
	while (true)
	{
		_pollResult = poll(_fds, _totalFD, 1000);
		if (_pollResult < 0){
			std::cout << "[SERVER: POLL CALLING FAILED]" << std::endl;
			stopServer();
			return;
		}
		else if (_pollResult == 0) // voir si necessaire
			continue;
		accept_newUser();
		for (int i = 1; i < _totalFD; i++){
			if (_fds[i].revents & (POLLIN | POLLPRI)){
				_bytesRead = recv(_fds[i].fd, _buffer, bufferSize - 1, MSG_DONTWAIT);
				if (_bytesRead == -1){
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						std::cout << "[CLIENT " <<getUserName(_fds[i].fd) << "]: Incomming message failed" << std::endl; //mettre nickname au lieu de fds
				}
				else if (_bytesRead == 0){
					std::cout << "DISCONNECTED" << std::endl;
					cleanFDS(i);
				}
				else if (_bytesRead > 0){
					_buffer[_bytesRead] = '\0';
					if (getUserLevel(_fds[i].fd) == 0){// niveau 0 c'est donc le 1er mess avec les infos de connexion
						if (firstMsg(_buffer) == false){// traiter les info
							// impossible de traiter le message
							// deconnecter le client
							return;
						}
						setUserLevel(_fds[i].fd, 1); //choisir quel niveau pour bannir
					}
					else if (getUserLevel(_fds[i].fd) == 1){ //level en attente d'une commande uniquement
						if (_bytesRead > commandSize)
							send(_fds[i].fd, "You can't exceeded 60 characters\nPlease try again.", 50, 0); //gerer les eventuelles erreur de send
						else if (_buffer[0] != '/' || selectCommand(&_buffer[1], i) == false)
							std::cout << "Wrong command\nPlease try again" << std::endl;
					}
					else if (getUserLevel(_fds[i].fd) == 2){ //pourrait etre destine aux messages vers le chanel
						// a faire
						// je pense que les mess channel ne devraient pas s afficher dans le serveur mais plutot
						// dans les client connectes au channel a discuter
					}
				}
				memset(_buffer, 0, bufferSize);
				_fds[i].revents = 0;
			}
		}
	}
}


//FONCTION EXIT

void server::stopServer(void)
{
	for (int i = 0; i < maxFD; i++){
		if (_fds[i].fd > -1)
			close(_fds[i].fd);
	}
	std::cout << "[SERVER: DISCONNECTED]" << std::endl;
}