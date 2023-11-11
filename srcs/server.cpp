#include "server.hpp"

// CONSTRUCTORS
server::server(int port, std::string password): _id(1000), _curFD(0), _bytesRead(0)
{
	setupPoll();
	initStruct(&_server);
	memset(&_buffer, 0, bufferSize);
	_server.id = _id;
	_server.port = port;
	_server.password = password;
	_id = 0;
	std::cout << "[SERVER: INITIALISATION]" << std::endl;
}

server::~server() {}

//GETTER

// std::string getName(int fd) {}



//SETTER

//FONCTIONS INITIALISATION
void server::setupPoll(void)
{
	for (int i = 0; i < maxFD; i++){
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLPRI;
		_fds[i].revents = 0;
	}
}

void server::initStruct(infoConnect *info)
{
	info->id = 0;
	info->port = 0;
	info->fds.fd = 0;
	info->fds.events = POLLIN | POLLPRI;
	info->sizeAddr = sizeof(info->Addr);
	memset(&info->Addr, 0, info->sizeAddr);
}

bool server::initServerSocket(void)
{
	_fds[0].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_fds[0].fd < 0){
		std::cout << "[SERVER: CONNECTION FAILLED]" << std::endl;
		return false;
	}
	_server.Addr.sin_family = AF_INET;
	_server.Addr.sin_addr.s_addr = INADDR_ANY;
	_server.Addr.sin_port = htons(_server.port);
	_server.fds.fd = _fds[0].fd;
	std::cout << "[SERVER: CONNECTED]" << std::endl;
	return true;
}

bool server::bindServerSocket(void)
{
	if (bind(_fds[0].fd, (sockaddr *)&_server.Addr, sizeof(_server.Addr)) < 0){
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
	std::cout << "[SERVER: LISTENING ON PORT " << _server.port << "]" << std::endl;
	_vect.push_back(_server);
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

bool server::verify_Pwd(infoConnect user){
	std::stringstream tries;
	std::string tmp;
	send(user.fds.fd, "ENTER PASSWORD\n", 16, 0);
	for (int i = 0; i < 3; i++){
		int bytesRead = recv(user.fds.fd, _buffer, bufferSize, 0);
		_buffer[bytesRead - 1] = '\0';
		if (_server.password.compare(_buffer) == 0)
			return (true);
		else{
			if (3 - (i + 1) == 0)
				break;
			send(user.fds.fd, "INCORRECT PASSWORD, TRY AGAIN\n", 31, 0);
			tries.str("");
			tries << (3 - (i + 1));
			tmp = tries.str();
			send(user.fds.fd, "YOU HAVE ", 10, 0);
			send(user.fds.fd, tmp.c_str(), 1, 0);
			send(user.fds.fd, " TRIES REMAINING\n", 18, 0);
		}
	}
	send(user.fds.fd, "INVALID PASSWORD, CONNECTION DENIED\n", 37, 0);
	return (false);
}

//BOUCLE

void server::sendMsgToClients(char *buffer, int n){
	std::stringstream tmp;
	std::string str = "Client ";
	tmp << _vect[n].id << " dit: "; //a changé vers nickname dit
	str.append(tmp.str()); 
	str.append(buffer);
	for (int i = 1; i <= _curFD; i++){
		if (i != n)
			send(_fds[i].fd, str.c_str(), str.length(),0);
	}
}

void server::accept_newUser(void)
{
	infoConnect user;
	initStruct(&user);
	user.fds.fd = accept(_fds[0].fd, (sockaddr*)&user.Addr, &user.sizeAddr);
	if (user.fds.fd > 0){
		user.id = _id++;
		_fds[++_curFD].fd = user.fds.fd;
		_fds[_curFD].events = POLLIN | POLLPRI;;
		std::cout << "[SERVER: SUCCESS CONNECTION FROM : " << inet_ntoa(user.Addr.sin_addr) << "]" << std::endl;
		//obtenir les info du user avant de stocker dans vector
		_vect.push_back(user);
		if (verify_Pwd(user))
			send(user.fds.fd, "|---------- WELCOME IN 42 IRC ----------|\n", 42, 0); //gestion erreur send
		else{
			_fds[_curFD].fd = -1;
			_curFD--;
			_vect.pop_back();
			_id--;
		}
	}
}

void server::mainloop()
{
	while (true)
	{
		_pollResult = poll(_fds, _curFD + 1, 1000);
		if (_pollResult < 0){
			std::cout << "[SERVER: POLL CALLING FAILED]" << std::endl;
			stopServer();
			return;
		}
		accept_newUser();
		for (int i = 1; i <= _curFD; i++){
			if (_fds[i].revents & POLLIN){
				std::cout << "[CLIENT " << _vect[i].id << "]: ";
				while (true){
					_bytesRead = recv(_fds[i].fd, _buffer, bufferSize - 1, MSG_DONTWAIT);
					if (_bytesRead == -1){
						if (errno != EAGAIN && errno != EWOULDBLOCK)
							std::cout << "Incomming message failed";
						break;
					}
					else if (_bytesRead > 0){
						_buffer[_bytesRead] = '\0';
						std::cout << _buffer;
						sendMsgToClients(_buffer, i);
						memset(_buffer, 0, bufferSize);
					}
					else if (_bytesRead == 0){
						std::cout << "DISCONNECTED" << std::endl;
						_fds[i].fd = -1;
						break;
					}
				}
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


