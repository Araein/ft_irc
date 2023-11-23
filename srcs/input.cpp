#include "irc.hpp"

void server::receivMessage(void)//********** VERIFIE SI UN CLIENT A RECU UN MESSAGE
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

void server::inputError(int fd)//********** RECEPTION D'UN MESSAGE D'ERREUR
{
	(void)fd;
	std::cout << "DEBUG errMessage en cours" << std::endl;//********** A SUPPRIMER
}

void server::inputMessage(int fd)//********** RECEPTION D'UN MESSAGE CLIENT
{
	std::string msg;
	int size;
	char buff[bufferSize];
	memset(&buff, 0, bufferSize);
	size = recv(fd, buff, bufferSize - 1, MSG_DONTWAIT);
	
	std::cout << "DEBUG : " << fd << " " << mapUser.find(fd)->second.getNickname() << " BUFF: " << buff;//********** A SUPPRIMER
	if (size < 0)//********** MESSAGE VIDE OU MAL RECEPTIONNE
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			msg = "An error has occurred\nYour message could not be sent./nPlease try again\n";
			send(fd, msg.c_str(), msg.size(), 0);
		}
	}
	else if (size == 0)//********** MESSAGE INDIQUANT LA DECONNEXION D'UN CLIENT
	{
		std::cout << "[SERVER: USER DISCONNECTED]: " << mapUser.find(fd)->second.getNickname() << std::endl;
		closeOne(fd);
	}
	else
	{
		std::istringstream iss(buff);
		std::string command;
		std::string msg;
		iss >> command;
		if (mapUser.find(fd)->second.getStatus() > 0 || command == "QUIT" || command == "PING")
			parseCommand(buff, fd);
		if (mapUser.find(fd)->second.getStatus() == 0)
		{
	std::cout << "DEBUG password " << fd << " " << mapUser.find(fd)->second.getNickname() << std::endl;//********** A SUPPRIMER
			msg = ":ircserv 393 " + mapUser.find(fd)->second.getNickname() + " :Your username is validated\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			std::string password = extract(buff, "PASS ", "\n");
			checkPassword(password, fd);
			std::cout << "[SERVER: USER CONNECTED]:" << mapUser.find(fd)->second.getNickname() << std::endl;
			if (mapUser.find(fd)->second.getStatus() == 1)
				printHome(fd);
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







