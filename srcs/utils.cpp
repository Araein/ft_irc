#include "irc.hpp"


std::string server::startServer(void) const
{
	std::time_t tt = std::time(0);
	std::tm* fr = std::localtime(&tt);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%a %b %d %Y at %H:%M:%S UTC", fr);
	return buffer;
}

void server::closeAll(void)
{
	for (int i = 0; i < maxFD; i++)
	{
		shutdown(_fds[i].fd, SHUT_RDWR);
		if (_fds[i].fd > -1)
		{
			close(_fds[i].fd);
		}
	}
	std::cout << "[SERVER: DISCONNECTED]" << std::endl;
}

void server::closeOne(int fd)
{
	for (std::map<int, client>::iterator it = mapUser.begin(); it != mapUser.end(); it++)
	{
		if (it->second.getFD() == fd)
		{
			mapUser.erase(it);
			shutdown(it->second.getFD(), SHUT_RDWR);
			break;
		}
	}
	for (int i = 1; i < maxFD + 1; i++)
	{
		if (_fds[i].fd > 0 && _fds[i].fd == fd)
		{
			close(_fds[i].fd);
			_fds[i].fd = -1;
			if (i == maxFD)
				_fds[i].fd = -2;
			_fds[i].revents = 0;
			break;
		}
	}
	_totalPlace--;
}

void server::clearFDStemp(void)
{
	if (_fds[maxFD].fd > 0)
	{
		close(_fds[maxFD].fd);
		_fds[maxFD].fd = -2;
		_fds[maxFD].revents = 0;
	}
}

int server::findPlace(void) const
{
	for (int i = 1; i < maxFD; i++)
	{
		if (_fds[i].fd == -1)
			return i;
	}
	return maxFD;
}

bool server::nameChar(std::string name, int index) const
{
	for (size_t i = 0; i < name.size(); i++)
	{
		if (isalnum(name[i]) == 0)
		{
			if (index == 0 && name[i] != '_')
				return false;
			if (index == 1 && name[i] != '_' && name[i] != ' ')
				return false;
		}
	}
	return true;
}

bool server::nameExist(std::string name)
{
	for (std::map<int, client>::iterator it = mapUser.begin(); it != mapUser.end(); it++)
	{
		if (it->second.getNickname().compare(name) == 0)
			return false;
	}
	return true;
}

bool server::checkChannelName(std::string name)
{
	if (name[0] != '#')
		return false;
	if (name.size() == 0 || name.size() > 128)
		return false;
	return true;
}

std::vector<channel>::iterator server::selectChannel(std::string name)
{
	std::vector<channel>::iterator it;
	for (it = channelList.begin(); it != channelList.end();  it++)
	{
		if (it->getChannelName() == name)
			return it;
	}
	return it;
}

bool server::checkNickname(std::string nick, int fd)
{
	std::string msg = "";
	if (nick.size() == 0)
		msg = "  \nYour nickname is empty\n";
	else if (nick.size() > 30)
		msg = "  \nYour nickname is too long\n";
	else if (nameChar(nick, 0) == false)
		msg = "  \nYour nickname contains invalid character\n";
	else if (nameExist(nick) == false)
		msg = "  \nYour nickname already exist\n";
	if (msg.size() > 0)
	{
		msg += "Your name is unchanged\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return false;
	}
	return true;
}




