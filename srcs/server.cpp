#include "irc.hpp"

server::~server(void) {}
server::server(int fd, int port, std::string password)
{
	_port = port;
	_password = password;
	_curPlace = 0;
	_totalPlace = 0;
	for (int i = 0; i < maxFD; i++)//+1 pour un fd temporaire pour eviter les BROKE PIPE
	{
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLERR;
		_fds[i].revents = 0;
	}
	_fds[maxFD].fd = -2;
	_fds[maxFD].events = POLLIN | POLLERR;
	_fds[maxFD].revents = 0;
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
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
	sock.sin_port = htons(_port);
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
	client us(_id, _fds[0].fd);//**********************************************A COMPLETER
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
		ret = poll(_fds, maxFD + 1, 1000);
		if (ret < 0)
		{
			std::cerr << "Failed to call poll()" << std::endl;
			closeAll();
			return;
		}
		acceptNewUser();
		if (ret == 0)
			continue;
		for (int i = 1; i < maxFD + 1; i++)
		{
			if (_fds[i].revents & (POLLIN | POLLERR))
			{
				if (_fds[i].revents & POLLIN)//surveille les messages recus des clients
					userMessage(_fds[i].fd);//**********************************************A FINIR
				else if (_fds[i].revents & POLLERR)//surveille les erreurs venant d'une fonction qui crash entre autre
					errMessage(_fds[i].fd);//**********************************************A FAIRE
			}
		}
	}
}

void server::acceptNewUser(void)
{
	if (_fds[maxFD].fd > 0){
		close(_fds[maxFD].fd);
		_fds[maxFD].fd = -2;
		_fds[maxFD].revents = 0;
	}
	sockaddr_in sock;
	socklen_t sizeSock = sizeof(sock);
	memset(&sock, 0, sizeSock);
	pollfd tempfds;
	tempfds.events = POLLIN | POLLERR;
	tempfds.revents = 0;
	tempfds.fd = accept(_fds[0].fd, (sockaddr *)&sock, &sizeSock);
	if (tempfds.fd < 0)
		return;
	_curPlace = findPlace();
	_fds[_curPlace].fd = tempfds.fd;
	_fds[_curPlace].revents = tempfds.revents;
	if (_curPlace == maxFD)
		printFullUser(_fds[_curPlace].fd);
	else
	{
		_totalPlace++;
		client us(++_id, _fds[_curPlace].fd);
		mapUser.insert(std::make_pair(_fds[_curPlace].fd, us));
	}
}

void server::userMessage(int fd)// si POLLIN
{
	int size;
	char buff[bufferSize];
	memset(&buff, 0, bufferSize);

	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	if (size == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			send(fd, "An error has occurred\nYour message could not be sent./nPlease try again\n", 72, 0);
	}
	else if (size == 0)
	{
		std::cout << "Bye " << (mapUser.find(fd))->second.getNickname() << ". Thanks to use 42_IRC." << std::endl;
		closeOne(fd);
	}
	else if (size > 0)
	{
		if ((mapUser.find(fd))->second.getPWD() == false)// password non encore valide
		{
			(mapUser.find(fd))->second.firstMessage(buff);//parsing du message dans la class client
			mapUser.find(fd)->second.setFD(fd); // a verifier
			if (((mapUser.find(fd))->second.getPassword()).compare(_password) != 0)//compare les mot de passe
			{
				send(fd, "INCORRECT PASSWORD\n You will be disconnected\n", 45, 0);
				closeOne(fd);
			}
			else
			{
				(mapUser.find(fd))->second.setPWD();//definie passeword validate
				sendWelcomeMsgs(fd);//**********************************************A REGLER (affichae nickname)
				send(fd, "Password validate\n", 18, 0);
				// printNewUser(fd);//**********************************************A FAIRE
			}
		}
		else
			parseMessage(buff, fd);//**********************************************A COMPLETER
	}
}

int server::findChanbyName(std::string chan) const{
	int i = 0;
	for (std::vector<channel>::const_iterator it = vecChannel.begin(); it != vecChannel.end(); it++){
		if (it->getChannelName() == chan)
			return (i);
		i++;
	}
	return (-1);
}

void server::parseMessage(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	iss >> command;
	if (command == "KICK" || command == "kick")
	{
		std::cout << "commande recu a traiter: KICK" << std::endl; 
	}
	else if (command == "NICK" || command == "nick"){
		cmdNick(fd, buff);
	}
	else if (command == "JOIN" || command == "join")
	{
		cmdJoin(buff, fd);
	}
	else if (command == "INVITE" || command == "invite")
	{
		std::cout << "commande recu a traiter: INVITE" << std::endl;
	}
	else if (command == "TOPIC" || command == "topic")
	{
		cmdTopic(fd, buff);
	}
	else if (command == "MODE" || command == "mode")
	{
		std::cout << "commande recu a traiter: MODE" << std::endl;
	}
	else if (command == "PRIVMSG" || command == "privmsg")
	{
		cmdPrivmsg(fd, buff);
	}
	else if (command == "QUIT" || command == "quit")
	{
		std::cout << "Bye " << (mapUser.find(fd))->second.getNickname() << ". Thanks to use 42_IRC." << std::endl;
		closeOne(fd);
	}
	else if (command == "PING" || command == "ping"){
		std::istringstream iss(buff);
		std::string pingMsg;
		iss >> pingMsg >> pingMsg; 
		std::string pongMsg = "PONG " + pingMsg + "\r\n";
		send(fd,pongMsg.c_str(), pongMsg.size(), 0);
	}
	else
		std::cout << "Message en attente de parsing: " << buff << std::endl;
}

void server::errMessage(int fd)// si POLLERR
{
	(void)fd;
	std::cout << " errMessage en cours" << std::endl;
}

void server::closeOne(int fd)
{
	mapUser.erase(mapUser.find(fd));
	for(int i = 1; i < maxFD + 1; i++)
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
	std::string msg;
	msg = "001 " + (mapUser.find(fd))->second.getNickname() + " :Welcome to 42 IRC!\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "002 RPL_YOURHOST :Your host is ircserv running version 0.1\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "003 RPL_CREATED :The server was created god knows when\n";
	send(fd, msg.c_str(), msg.length(), 0);
	msg = "004 RPL_MYINFO :ircserv 0.1 level0 chan_modeballecouille\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void server::createChannel(void)
{
	channel chan0("#Minishell");
	vecChannel.push_back(chan0);
	channel chan1("#SoLong");
	vecChannel.push_back(chan1);
	channel chan2("#PushSwap");
	vecChannel.push_back(chan2);
	channel chan3("#Inception");
	vecChannel.push_back(chan3);
}

void server::printNewUser(int fd)
{
	(void)fd;
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


	/*********communication intra channel**********/


    void server::joinChannel(int fd, const std::string& channel)
	{
		std::map<int, client>::iterator it = mapUser.find(fd);
		if (it != mapUser.end())
        {
			client& myClient = it->second;
        	channels[channel].push_back(&myClient); // on remplit le map des channels
		} //message d'erreur si trouve pas?
    }



    void server::sendMessage(int fd, const std::string& channel, std::string& message)
	{
		std::map<int, client>::iterator it = mapUser.find(fd);
		if (it != mapUser.end())
		{
			client& sender = it->second;
		
			std::cout << "in " << channel << ": " << "[" << sender.getNickname() << "]: " << message << std::endl;

			const std::vector<client*>& clients = channels[channel];
			
			for (std::vector<client*>::const_iterator it = clients.begin(); it != clients.end(); ++it)
			{
				if (*it != NULL)
				{
					if (*it != &sender)
					{
       					 message = ":" + sender.getNickname() + " PRIVMSG " + channel + " :" + message + "\n";
						std::cout << "  [Message to " << (*it)->getNickname() << "]: " << message << std::endl;
						send((*it)->getFD(), message.c_str(), message.size(), 0);
					}
				}
			}
		}    
    }

	/**********************************************/


int server::getFD(int i) const { return _fds[i].fd; }
int server::getPort(void) const { return _port; }
std::string server::getPassword(void) const { return _password;}

void server::setFD(int i, int val) { _fds[i].fd = val; }






