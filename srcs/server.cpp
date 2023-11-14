#include "irc.hpp"

server::~server(void) {}
server::server(int fd, int port, std::string password)
{
	_port = port;
	_password = password;
	_curPlace = 0;
	_totalPlace = 0;
	for (int i = 0; i < maxFD; i++)
	{
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLHUP | POLLERR;
		_fds[i].revents = 0;
	}
	_fds[0].fd = fd;
}

bool server::initSocket(void)
{
	int opt = 1;
	sockaddr_in sock;
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port = htons(_port);

	std::cout << "[SERVER: INITIALISATION]" << std::endl;
	_fds[0].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_fds[0].fd < 0)
	{
		std::cerr << "Failed to open socket" << std::endl;
		return false;
	}
	if (setsockopt(_fds[0].fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))){
		std::cerr << "Failed to config socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	std::cout << "[SERVER: CONNECTED]" << std::endl;
	if (bind(_fds[0].fd, (sockaddr *)&sock, sizeof(sock)) < 0)
	{
		std::cerr << "Failed to bind to socket" << std::endl;
		return false;
	}
	std::cout << "[SERVER: BINDING]" << std::endl;
	if (listen(_fds[0].fd, SOMAXCONN) < 0)
	{
		std::cerr << "Failed to listen to socket" << std::endl;
		return false;
	}
	client us;
	mapUser.insert(std::make_pair(_fds[0].fd, us));
	std::cout << "[SERVER: LISTENING ON PORT " << _port << "]" << std::endl;
	_totalPlace++;
	return true;
}

void server::mainLoop(void)
{
	int ret;
	while (1)
	{
		ret = poll(_fds, maxFD, 1000);
		if (ret < 0)
		{
			std::cerr << "Failed to call poll()" << std::endl;
			closeAll();
			return;
		}
		acceptNewUser();
		if (ret == 0)
			continue;
		for (int i = 1; i < maxFD; i++)
		{
			if (_fds[i].revents & (POLLIN | POLLHUP | POLLERR))
			{
				if (_fds[i].revents & POLLIN)//surveille les messages recus des clients
					userMessage(_fds[i].fd);
				else if (_fds[i].revents & POLLERR)//surveille les erreurs venant d'une fonction qui crash entre autre
					errMessage(_fds[i].fd);
				else if (_fds[i].revents & POLLHUP)//surveille les deconnexion
					disconnectMessage(_fds[i].fd);
			}
		}
	}
}

void server::acceptNewUser(void)
{
	socklen_t sizeSock;
	sockaddr_in sock;
	size_t size = sizeof(sock);
	memset(&sock, 0, size);
	
	pollfd tempfds;
	tempfds.events = POLLIN | POLLHUP | POLLERR;
	tempfds.revents = 0;
	tempfds.fd = accept(_fds[0].fd, (sockaddr *)&sock, &sizeSock);

	sendWelcomeMsgs(tempfds.fd);

	if (tempfds.fd < 0)
		return;
	_curPlace = findPlace();
	if (_curPlace == maxFD)
	{
		printFullUser(tempfds.fd);
		close(tempfds.fd);
		return;
	}
	_totalPlace++;
	_fds[_curPlace].fd = tempfds.fd;
	_fds[_curPlace].revents = tempfds.revents;
	client us;
	mapUser.insert(std::make_pair(tempfds.fd, us));
}

void server::userMessage(int fd)// si  POLLIN
{
	int size;
	char buff[bufferSize];
	memset(&buff, 0, bufferSize - 1);

	size = recv(fd, buff, bufferSize - 2, MSG_DONTWAIT);
	if (size == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			send(fd, "An error has occurred\nYour message could not be sent./nPlease try again\n", 72, 0);
	}
	else if (size > 0)
	{
		if ((mapUser.find(fd))->second.getPWD() == false)
		{//si PWD du client est false alors le mot de passe n est pas encore valider donc le message recu est forcement les info irssi
			(mapUser.find(fd))->second.firstMessage(buff);//parsing du message dans la class client
			if (((mapUser.find(fd))->second.getPassword()).compare(_password) != 0)
			{//compare les mot de passe
				send(fd, "INCORRECT PASSWORD\n You will be disconnected\n", 45, 0);
				closeOne(fd);
			}
			else
			{
				(mapUser.find(fd))->second.setPWD();//passe PWD a true
				send(fd, "Password validate\n", 18, 0);
				printNewUser(fd);
			}
		}
		else
			parseMessage(buff, fd);
	}
}


void server::parseMessage(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	if (command == "KICK" || command == "kick")
	{
		std::cout << "commande recu en a traiter: KICK\n" << std::endl; 
	}
	else if (command == "JOIN" || command == "join")
	{
		std::cout << "commande recu en a traiter: JOIN\n" << std::endl;
	}
	else if (command == "INVITE" || command == "invite")
	{
		std::cout << "commande recu en a traiter: INVITE\n" << std::endl;
	}
	else if (command == "TOPIC" || command == "topic")
	{
		std::cout << "commande recu en a traiter: TOPIC\n" << std::endl;
	}
	else if (command == "MODE" || command == "mode")
	{
		std::cout << "commande recu en a traiter: MODE\n" << std::endl;
	}
	else if (command == "QUIT" || command == "quit")
	{
		std::cout << (mapUser.find(fd))->second.getNickname() << " Thanks to use 42_IRC.\n" << std::endl;
		//closeOne();
	}
	else
		std::cout << "Message en attente de parsing\n" << buff << std::endl;


}

void server::errMessage(int fd)// si POLLERR
{
	(void)fd;
	std::cout << " errMessage en cours" << std::endl;
}

void server::disconnectMessage(int fd)//si POLLHUP
{
	(void)fd;
	std::cout << " disconnectMessage en cours" << std::endl;
}


void server::closeOne(int fd)
{
	mapUser.erase(mapUser.find(fd));
	for(int i = 1; i < maxFD; i++)
	{
		if (_fds[i].fd == fd){
			close(_fds[i].fd);
			_fds[i].fd = -1;
			_fds[i].revents = 0;
		}
	}
	_totalPlace--;
}

void server::closeAll(void)
{
	for (int i = 0; i < maxFD; i++)
	{
		if (_fds[i].fd > -1)
			close(_fds[i].fd);
	}
}

void server::sendWelcomeMsgs(int fd)
{
	if (fd == -1)
		return;
	std::string nickname = (mapUser.find(fd))->second.getNickname();
	std::string msg;
	msg = "001 " + nickname + " :Welcome to 42 IRC!\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "002 RPL_YOURHOST :Your host is ircserv running version 0.1\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "003 RPL_CREATED :The server was created god knows when\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "004 RPL_MYINFO :ircserv 0.1 level0 chan_modeballecouille\n";
	send(fd, msg.c_str(), msg.length(), 0);
}


void server::printNewUser(int fd)
{

}

int server::findPlace(void)
{
	for (int i = 1; i < maxFD; i++)
	{
		if (_fds[i].fd == -1)
			return i;
	}
	return maxFD;
}

void server::printFullUser(int fd)
{
	send(fd, "|-------------------------------------------------------|\n", 58, 0);
	send(fd, "|-------------------------------------------------------|\n", 58, 0);
	send(fd, "|------------------ 42_IRC is full ---------------------|\n", 58, 0);
	send(fd, "|--------------- Please try again later ----------------|\n", 58, 0);
	send(fd, "|-------------------------------------------------------|\n", 58, 0);
	send(fd, "|-------------------------------------------------------|\n", 58, 0);
}




int server::getFD(int i) const { return _fds[i].fd; }
int server::getPort(void) const { return _port; }
std::string server::getPassword(void) const { return _password;}

void server::setFD(int i, int val) { _fds[i].fd = val; }






