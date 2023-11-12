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
		_fds[i].events = POLLIN | POLLPRI; // voir comment gerer POLLPRI
		_fds[i].revents = 0; //a mettre sinon leaks
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

bool server::verify_Pwd(infoConnect user){// voir ou le placer car bloque tant que pas de reponse.
	std::stringstream tries;
	std::string tmp;
	send(user.fds.fd, "ENTER PASSWORD\n", 16, 0);
	for (int i = 0; i < 3; i++){
		int bytesRead = recv(user.fds.fd, _buffer, bufferSize, 0);
		if (bytesRead > 0)
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

bool server::firstMsg(std::string message)// permettra de gerer le 1er message envoyer par irssi ou le client qu'on choisira
{
	(void)message;
	std::cout << "message" << std::endl;
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

void server::sendMsgToClients(char *buffer, int n){ // a placer dans la partie gerant les channels
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
		send(user.fds.fd, "|---------- WELCOME IN 42_IRC ----------|\n", 42, 0);
		if (_curFD >= maxFD){
			send(user.fds.fd, "42_IRC is full please try again later\n", 38, 0);
			close(user.fds.fd);
			return;
		}
		user.id = ++_id;
		_curFD++;
		_fds[_curFD].fd = user.fds.fd;
		_fds[_curFD].events = POLLIN | POLLPRI;;
		client newUser(&user); //creer une instance pour chaque nouveau user class client
		user.infoUser = &newUser;
		std::cout << "[SERVER: SUCCESS CONNECTION FROM : " << inet_ntoa(user.Addr.sin_addr) << "]" << std::endl;
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
		_pollResult = poll(_fds, _curFD + 1, 1000);
		if (_pollResult < 0){
			std::cout << "[SERVER: POLL CALLING FAILED]" << std::endl;
			stopServer();
			return;
		}
		accept_newUser();
		for (int i = 1; i <= _curFD; i++){
			if (_fds[i].revents & POLLIN){
				_bytesRead = recv(_fds[i].fd, _buffer, bufferSize - 1, MSG_DONTWAIT);
				if (_bytesRead == -1){
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						std::cout << "[CLIENT " << _vect[i].id << "]: Incomming message failed" << std::endl;
				}
				else if (_bytesRead == 0){
					std::cout << "DISCONNECTED" << std::endl;
					_fds[i].fd = -1; //deconnecter proprement
				}
				else if (_bytesRead > 0){
					_buffer[_bytesRead] = '\0';
//il y a un segFault ici car je ne peux pas semble t il acceder a _vect[i].infoUser->getLevel() a la place de
// infoConnect je vais directement creer une instance et mettre toutes les info de la struct infoConnect dans l'instance directement
// et a la place de vector faire une map<pollfd, class client>
					if (_vect[i].infoUser->getLevel() == 0){// niveau 0 c'est donc le 1er mess avec les infos de connexion
						if (firstMsg(_buffer) == false){// traiter les info
							// impossible de traiter le message
							// deconnecter le client
							return;
						}
						_vect[i].infoUser->setLevel(1);
					}
					else if (_vect[i].infoUser->getLevel() == 1){ //en attente d'une commande uniquement
						if (_bytesRead > bufferSize - 4)
							send(_fds[i].fd, "You can't exceeded 60 characters\nPlease try again.", 50, 0); //gerer les eventuelles erreur de send
						else if (_buffer[0] != '/' || selectCommand(&_buffer[1], i) == false)
							std::cout << "Wrong command\nPlease try again" << std::endl;
					}
					else if (_vect[i].infoUser->getLevel() == 2){ //pourrait etre destine aux messages vers le chanel
						// a faire
						// je pense que les mess channel ne devraient pas s afficher dans le serveur mais plutot
						// dans les client connectes au channel a discuter
					}
				}
					memset(_buffer, 0, bufferSize);
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