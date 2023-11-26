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
	for (std::map<int, client>::iterator it = mapUser.begin(); it != mapUser.end(); it++)
	{
		shutdown(it->second.getFD() , SHUT_RDWR);
		close(it->second.getFD());
	}
	std::cout << std::endl << RED << BOLD << "[42_IRC:  DISCONNECTED] " << NONE << "Hope you enjoyed it"  << std::endl;
}

void server::closeOne(int fd)
{
	for (std::map<int, client>::iterator it = mapUser.begin(); it != mapUser.end(); it++)
	{
		if (it->second.getFD() == fd)
		{
			shutdown(it->second.getFD(), SHUT_RDWR);
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

int server::findPlace(void) const
{
	for (int i = 1; i < maxFD; i++)
	{
		if (_fds[i].fd == -1)
			return i;
	}
	return maxFD;
}

bool server::nameUserCheck(std::string name) const
{
	if (isalpha(name[0]) == 0)
		return false;
	for (size_t i = 0; i < name.size(); i++)
	{
		if (isascii(name[i]) == 0)
			return false;
		if (name[i] == ':' || name[i] == ',' || name[i] == '!' || name[i] == '@' || name[i] == '#' || name[i] == ' ')
			return false;
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
	if (name.size() == 0)
		return channelList.end();
	std::vector<channel>::iterator it;
	for (it = channelList.begin(); it != channelList.end();  it++)
	{
		if (it->getChannelName().compare(name) == 0)
			return it;
	}
	return it;
}

std::map<std::string, std::string> server::splitCommandJoin(std::string buff)
{
	std::istringstream iss(buff);
	std::string str;
	std::vector<std::string> vec;
	std::vector<std::string> chan;
	std::vector<std::string> pass;
	std::map<std::string, std::string> chanPass;
	std::vector<std::string>::iterator it1 = vec.begin() + 1;//sans join
	std::vector<std::string>::iterator it2;

	while (std::getline(iss, str, ' '))//JOIN CHANNELS PASS
	{
		if (str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r')
			str = str.substr(0, str.size() - 2);
		else if (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r')
			str = str.substr(0, str.size() - 1);
		vec.push_back(str);
	}
	if (vec.size() == 1)
	{
		chanPass.insert(std::make_pair("", ""));
		return chanPass;
	}
	iss.clear();
	iss.str(vec[1]);
	while (std::getline(iss, str, ','))//SEPARE LES CHANNELS
		chan.push_back(str);
	iss.clear();
	if (vec.size() > 2)
		iss.str(vec[2]);
	while (std::getline(iss, str, ','))//SEPARE LES PASS
		pass.push_back(str);
	if (pass.size() < chan.size())
	{
		while (pass.size() < chan.size())
			pass.push_back("");
	}
	it1 = chan.begin();
	it2 = pass.begin();
	while (it1 != chan.end())
	{
		chanPass.insert(std::make_pair(*it1, *it2));
		it1++;
		it2++;
	}
	return chanPass;
}

std::vector<std::string> server::splitCommandNick(std::string buff)
{
	std::istringstream iss(buff);
	std::string str;
	std::vector<std::string> vec;
	while (std::getline(iss, str, ' '))
	{
		if (str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r')
			str = str.substr(0, str.size() - 2);
		else if (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r')
			str = str.substr(0, str.size() - 1);
		vec.push_back(str);
	}
	return vec;
}

void server::userUpDate(client *user)
{
	for (std::vector<channel>::iterator it = user->getConnectBegin(); it != user->getConnectEnd(); it++)
	{
		for (std::vector<channel>::iterator itchan = channelList.begin(); itchan != channelList.end(); itchan++)
		{
			if (itchan->getChannelName() == it->getChannelName())
			{
				itchan->switchUser(user);
				break;
			}
		}
	}
}

