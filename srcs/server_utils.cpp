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
		if (_fds[i].fd > -1)
		{
			shutdown(_fds[i].fd, SHUT_RDWR);
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



