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
		if (it->second.getFD() > 0)
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
			it->second.exitUser();
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

std::vector<channel>::iterator server::selectChannel(std::string name)
{
	std::vector<channel>::iterator it;
	if (name.size() == 0)
		return channelList.end();
	for (it = channelList.begin(); it != channelList.end();  it++)
	{
		if (it->getChannelName() == name)
			return it;
	}
	return channelList.end();
}

std::map<std::string, std::string> server::splitCommandJoin(std::string buff)
{
	std::istringstream iss(buff);
	std::string str;
	std::vector<std::string> vec;
	std::vector<std::string> chan;
	std::vector<std::string> pass;
	std::map<std::string, std::string> chanPass;
	std::vector<std::string>::iterator it1 = vec.begin() + 1;
	std::vector<std::string>::iterator it2;

	while (std::getline(iss, str, ' '))
	{
		str = deleteCRLF(str);
		vec.push_back(str);
	}
	if (vec.size() == 1)
		return chanPass;
	iss.clear();
	iss.str(vec[1]);
	while (std::getline(iss, str, ','))
		chan.push_back(str);
	iss.clear();
	if (vec.size() > 2)
		iss.str(vec[2]);
	while (std::getline(iss, str, ','))
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
		str = deleteCRLF(str);
		vec.push_back(str);
	}
	return vec;
}

std::vector<std::string> server::splitCommandPrivmsg(std::string buff)
{
	std::istringstream iss(buff);
	std::string str;
	std::string tp = "";
	std::vector<std::string> vec;
	int indice = 0;
	while (std::getline(iss, str, ' '))
	{
		indice++;
		str = deleteCRLF(str);
		if (indice < 3)
			vec.push_back(str);
		else
			tp += str + " ";
	}
	if (tp.size() > 0)
	{
		if (tp[0] == ':')
			vec.push_back(&tp[1]);
		else
			vec.push_back(tp);
	}
	return vec;
}

void server::userUpDate(client *user, std::string newNick)
{
	std::string msg = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost NICK " + newNick + "\n";
	send(user->getFD(), msg.c_str(), msg.size(), 0);
	for (std::vector<channel>::iterator it = user->getConnectBegin(); it != user->getConnectEnd(); it++)
	{
		for (std::vector<channel>::iterator itchan = channelList.begin(); itchan != channelList.end(); itchan++)
		{
			if (itchan->getChannelName() == it->getChannelName())
			{

				itchan->switchUser(user);
				itchan->sendInfoToChannel(*user, " is now know as " + newNick);
				break;
			}
		}
	}
}

std::string server::deleteCRLF(std::string str)
{
	if (str[str.size() - 1] == '\n' && str[str.size() - 2] == '\r')
		str = str.substr(0, str.size() - 2);
	else if (str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r')
		str = str.substr(0, str.size() - 1);
	return str;
}

int server::jumpToNextMode(std::string::iterator it){
	int i = 0;
	while ((*it != '+' || *it != '-') && *it != '\0'){
		it++;
		i++;
	}
	return i - 1;
}

bool server::findKey(std::vector<std::string> vec, std::string key){
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++){
		if (*it == key)
			return true;
	}
	return false;
}

std::map<int, client>::iterator server::selectUser(std::string name)
{
	std::map<int, client>::iterator it;
	if (name.size() == 0)
		return mapUser.end();
	for (it = mapUser.begin(); it != mapUser.end();  it++)
	{
		if (it->second.getNickname() == name)
			return it;
	}
	return mapUser.end();
}




