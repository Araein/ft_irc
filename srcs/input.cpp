#include "irc.hpp"

void server::receivMessage(void)
{
	for (int i = 1; i < maxFD + 1; i++)
	{
		if (_fds[i].revents & (POLLIN | POLLERR))
		{
			if (_fds[i].revents & POLLIN)
				inputMessage(_fds[i].fd);
			else if (_fds[i].revents & POLLERR)
				inputError(_fds[i].fd);
			_fds[i].revents = 0;
		}
	}
}

void server::inputError(int fd)// si POLLERR
{
	(void)fd;
	std::cout << " errMessage en cours" << std::endl;
}

void server::inputMessage(int fd)
{
	std::string msg;
	int size;
	char buff[bufferSize];
	memset(&buff, 0, bufferSize);
	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	std::cout << "BUFF: " << buff << std::endl;//********************************a supprimer
	if (size < 0)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			msg = "An error has occurred\nYour message could not be sent./nPlease try again\n";
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
	else if (size == 0)
	{
		std::cout << "[SERVER: USER DISCONNECTED]: " << mapUser.find(fd)->second.getNickname() << std::endl;
		closeOne(fd);
	}
	else
	{
		std::istringstream iss(buff);
		std::string command;
		iss >> command;
		if (mapUser.find(fd)->second.getStatus() > 1 || command == "QUIT" || command == "PING")
			parseCommand(buff, fd);
		if (mapUser.find(fd)->second.getStatus() == 0)
		{
			std::string msg;
			if (command == "CAP")
			{
				msg = ":ircserv CAP * LS :Soon available service\r\n";
				send(fd, msg.c_str(), msg.size(), 0);
			}
			std::string password = extract(buff, "PASS ", "\n");
			checkPassword(password, fd);
		}
		if (mapUser.find(fd)->second.getStatus() == 1 || mapUser.find(fd)->second.getStatus() == 2)
		{
			// std::string nickname = extract(buff, "NICK ", "\n");
			checkNickname(mapUser.find(fd)->second.getNickname(), fd);
		}
	}
}

void server::checkPassword(std::string pass, int fd)
{
	std::string msg;
	if (_password.compare(pass) == 0)
	{
		mapUser.find(fd)->second.setStatus();
		msg = GREEN;
		msg += BOLD;
		msg +=  "You have successfully logged in\n";
		msg += NONE;
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	msg = RED;
	msg += BOLD;
	msg += "Invalid password, you will be disconnected\n";
	msg += NONE;
	_fds[maxFD].fd = fd;
	send(_fds[maxFD].fd, msg.c_str(), msg.size(), 0);
}

void server::checkNickname(std::string nick, int fd)
{
	std::string msg = "";
	if (nick.size() == 0)
		msg = "  \nYour nickname is empty\n";
	else if (nick.size() > 30)
		msg = "  \nYour nickname is too long\n";
	else if (nameChar(nick, 0) == false)
		msg = "  \nYour nickname contains invalid character\n";
	// else if (nameExist(nick) == false)
	// 	msg = "  \nYour nickname already exist\n";
	if (msg.size() > 0)
	{
		msg += "Please set a new nickname with the command: NICK <nickname>\n";
		send(fd, msg.c_str(), msg.size(), 0);
	}
	else
	{
		mapUser.find(fd)->second.setStatus();
		std::string mess = "NICK " + nick;
		cmdNick(fd, mess);
		if (mapUser.find(fd)->second.getUsername().size() == 0)
			mapUser.find(fd)->second.setUsername(nick);
		std::cout << "[SERVER: USER CONNECTED]:" << mapUser.find(fd)->second.getNickname() << std::endl;
		printHome(fd);
	}
}




