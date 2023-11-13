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
	for (int i = 0; i < maxFD; i++)
	{
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLPRI; // voir comment gerer POLLPRI
		_fds[i].revents = 0;
	}
}

bool server::initServerSocket(void)
{
	_fds[0].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_fds[0].fd < 0)
	{
		std::cout << "[SERVER: CONNECTION FAILLED]" << std::endl;
		return false;
	}
	_sock.Addr.sin_family = AF_INET;
	_sock.Addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
	_sock.Addr.sin_port = htons(_port);
	std::cout << "[SERVER: CONNECTED]" << std::endl;
	return true;
}

bool server::bindServerSocket(void)
{
	if (bind(_fds[0].fd, (sockaddr *)&_sock.Addr, sizeof(_sock.Addr)) < 0)
	{
		std::cout << "[SERVER: FAILED TO BIND]" << std::endl;
		return false;
	}
	std::cout << "[SERVER: BINDING]" << std::endl;
	return true;
}

bool server::listenServerSocket(void)
{
	if (listen(_fds[0].fd, SOMAXCONN) < 0)
	{
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
	if (bindServerSocket() == false || listenServerSocket() == false)
	{
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
	for (int i = 1; i < maxFD; i++)
	{
		if (_fds[i].fd == -1)
			return i;
	}
	return maxFD;
}

void server::cleanFDS(int i)
{
	std::cout << "[SERVER:] Disconnected: " << getUserName(_fds[i].fd) << std::endl;
	mapUser.erase(mapUser.find(_fds[i].fd));
	close(_fds[i].fd);
	_fds[i].fd = -1;
	_fds[i].revents = 0;
	_totalFD--;
}

bool IsNotSpace(int ch)
{
    return !std::isspace(ch);
}

std::string ltrim(const std::string& str)
{
    std::string::const_iterator it = std::find_if(str.begin(), str.end(), IsNotSpace);
    return std::string(it, str.end());
}

std::string rtrim(const std::string& str)
{
    std::string::const_reverse_iterator it = std::find_if(str.rbegin(), str.rend(), IsNotSpace);
    return std::string(str.begin(), it.base());
}

const std::string extract(const std::string& message, const std::string& start, const std::string& end)
{
    size_t startPos = message.find(start);
    size_t endPos = message.find(end, startPos + start.length());

    if (startPos != std::string::npos && endPos != std::string::npos)
    {
        return rtrim(ltrim(message.substr(startPos + start.length(), endPos - startPos - start.length())));
    }

    return "";
}


bool server::firstMsg(std::string message, int fd)
{
	//std::cout << ":::::" << message << "::::::" << std::endl;

	std::map<int, client>::iterator it = mapUser.find(fd);

	if (it != mapUser.end())
	{

		client& clientFound = it->second;

		clientFound.setPassword(extract(message, "PASS ", "\n"));
		std::cout << "Password: ." << clientFound.getPassword() << "." << std::endl;

		clientFound.setNickname(extract(message, "NICK ", "\n"));
		std::cout << "Nickname: ." << clientFound.getNickname() << "." << std::endl;

		clientFound.setUsername(extract(message, "USER ", " "));
		std::cout << "Username: ." << clientFound.getUsername() << "." << std::endl;

		clientFound.setIdentity(extract(message, ":", "\n"));
		std::cout << "Identity: ." << clientFound.getIdentity() << "." << std::endl;

	}
	else
		std::cout << "Descripteur de fichier non trouvé dans la map." << std::endl;

	return true;
}

bool server::selectCommand(std::string message, int i)
{
	std::istringstream iss(message);
	std::string command;
	iss >> command;
	if (command == "KICK" || command == "kick"){
		send(_fds[i].fd, "commande KICK\n", 14, 0);
	}
	else if (command == "JOIN" || command == "join"){
		send(_fds[i].fd, "commande JOIN\n", 14, 0);
	}
	else if (command == "INVITE" || command == "invite"){
		send(_fds[i].fd, "commande INVITE\n", 16, 0);
	}
	else if (command == "TOPIC" || command == "topic"){
		send(_fds[i].fd, "commande TOPIC\n", 15, 0);
	}
	else if (command == "MODE" || command == "mode"){
		send(_fds[i].fd, "commande MODE\n", 14, 0);
	}
	else if (command == "QUIT" || command == "quit"){ //test
		send(_fds[i].fd, "Thanks to use 42_IRC.\n", 22, 0);
		cleanFDS(i);
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

void server::sendWelcomeMsgs(client user)
{

	std::string msg;
	msg = "001 " + user.getNickname() + " :Welcome to 42 IRC!\n";
	send(_fds[_curFD - 1].fd, msg.c_str(), msg.length(), 0);
	msg = "002 RPL_YOURHOST :Your host is ircserv running version 0.1\n";
	send(_fds[_curFD - 1].fd, msg.c_str(), msg.length(), 0);
	msg = "003 RPL_CREATED :The server was created god knows when\n";
	send(_fds[_curFD - 1].fd, msg.c_str(), msg.length(), 0);
	msg = "004 RPL_MYINFO :ircserv 0.1 level0 chan_modeballecouille\n";
	send(_fds[_curFD - 1].fd, msg.c_str(), msg.length(), 0);
}

void server::accept_newUser(void)
{
	int fd;
	_curFD = findCurFD();
	if (_curFD == maxFD){
		fd = accept(_fds[0].fd, (sockaddr*)&_sock.Addr, &_sock.sizeAddr);
		if (fd > 0){
			send(fd, "42_IRC is full please try again later", 37, 0);
			close(fd);
			return;
		}
	}
	_fds[_curFD].fd = accept(_fds[0].fd, (sockaddr*)&_sock.Addr, &_sock.sizeAddr);
	if (_fds[_curFD].fd > 0){
		_totalFD++;
		send(_fds[_curFD].fd, "|---------- WELCOME IN 42_IRC ----------|\n", 42, 0);
		client user(_fds);
		mapUser.insert(std::make_pair(_fds[_curFD].fd, user));
		std::cout << "[SERVER: SUCCESS CONNECTION FROM : " << inet_ntoa(_sock.Addr.sin_addr) << "]" << std::endl;
	}
}

void server::mainloop()
{
	while (true)
	{
		_pollResult = poll(_fds, maxFD, 1000);
		if (_pollResult < 0)
		{
			std::cout << "[SERVER: POLL CALLING FAILED]" << std::endl;
			stopServer();
			return;
		}
		else if (_pollResult == 0) // si timeout
			;
		accept_newUser();
		for (int i = 1; i < _totalFD + 1; i++)
		{
			if (_fds[i].revents & (POLLIN))
			{
				_bytesRead = recv(_fds[i].fd, _buffer, bufferSize - 1, MSG_DONTWAIT);
				if (_bytesRead == -1)
				{
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						;// std::cout << "[CLIENT " <<getUserName(_fds[i].fd) << "]: Incomming message failed" << std::endl; //mettre nickname au lieu de fds
				}
				else if (_bytesRead == 0)
					cleanFDS(i);
				else if (_bytesRead > 0)
				{
					_buffer[_bytesRead] = '\0';
					if (getUserLevel(_fds[i].fd) == 0)
					{
						if (firstMsg(_buffer, _fds[i].fd) == false)
						{
							// impossible de traiter le message
							// ou mot de passe incorrect
							// deconnecter le client
							return;
						}
						else
							sendWelcomeMsgs(mapUser.find(_fds[i].fd)->second);
						// debug();
						setUserLevel(_fds[i].fd, 1); // choisir quel niveau pour bannir
					}
					else if (getUserLevel(_fds[i].fd) == 1)
					{
						if (_bytesRead > commandSize)
							send(_fds[i].fd, "You can't exceeded 60 characters.Please try again\n", 50, 0); // gerer les eventuelles erreur de send
						else if (_buffer[0] != '/' || selectCommand(&_buffer[1], i) == false)
							send(_fds[i].fd, "Wrong command.Please try again\n", 31, 0);
					}
					else if (getUserLevel(_fds[i].fd) == 2)
					{
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