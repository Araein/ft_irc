#include "irc.hpp"

client::~client(void) {}
client::client(int id, int fd): _fd(fd), _id(id), _log(0), fileId(100), _netcat(0)
{
	std::ostringstream oss;
	oss << _id;
	_nickname = "user_" + oss.str();
	_username = "user" + oss.str();
	if (_id == 0)
	{
		_nickname = "chanOp_42stud";
		_username = "chanOp_42stud";
		_netcat = -2;
		_log = 2;
	}
}

//**********************************//GETTER//**********************************//

int client::getID(void) const { return _id; }
int client::getFD(void) const { return _fd; }
int client::getLog(void) const { return _log; }
int client::getNetcat(void) const { return _netcat; }
size_t client::getHowManyChannel(void) const { return channelConnected.size(); }
std::string client::getPassword(void) const { return _password;}
std::string client::getNickname(void) const { return _nickname;}
std::string client::getUsername(void) const { return _username;}
std::vector<channel>::iterator client::getConnectBegin() { return channelConnected.begin(); }
std::vector<channel>::iterator client::getConnectEnd() { return channelConnected.end(); }
int client::getHowManyFile() const { return fileList.size(); }

int client::getFileDest(std::string id) const
{
	for (std::vector<transferFile>::const_iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		if (it->id == id)
			return it->fdDest;
	}
	return -1;
}

bool client::getIsChanOp(void)
{
	for (std::vector<channel>::iterator it = channelConnected.begin(); it != channelConnected.end(); it++)
	{
		if (it->getIsChanOp(_id) == true)
			return true;
	}
	return false;
}

bool client::getNumFileExist(std::string id) const
{
	for (std::vector<transferFile>::const_iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		if (it->id == id)
			return true;
	}
	return false;
}

std::vector<transferFile>::iterator client::getTrf(std::string id)
{
	for (std::vector<transferFile>::iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		if (it->id == id)
			return it;
	}
	return fileList.end();
}

//**********************************//SETTER//**********************************//

void client::setPassword(std::string pass) { _password = pass; }
void client::setUsername(std::string username) { _username = username; }
void client::setLog(void) { _log++; }
void client::setNetcat(int value) { _netcat = value; }
void client::setNickname(std::string nickname) { _nickname = nickname; }
void client::setNetcat(void)
{
	if (_netcat > -1)
		_netcat++;
}

//**********************************//FONCTIONS//**********************************//

void client::addChannel(channel *chan) { channelConnected.push_back(*chan); }

void client::deleteChannel(channel const &chan)
{
	for (std::vector<channel>::iterator it = channelConnected.begin(); it != channelConnected.end(); it++)
	{
		if (it->getChannelName() == chan.getChannelName())
		{
			channelConnected.erase(it);
			break;
		}	
	}
}

void client::exitUser(void)
{
	for (std::vector<channel>::iterator it = channelConnected.begin(); it != channelConnected.end(); it++)
		it->setUserShutdown(this);
}

std::string client::setFileList(transferFile &trf)
{
	std::ostringstream oss;
	oss << ++fileId;
	trf.id = oss.str();
	fileList.push_back(trf);
	return trf.id;
}

void client::delFileList(std::string id)
{
	for (std::vector<transferFile>::iterator it = fileList.begin(); it != fileList.end(); it++)
	{
		if (it->id == id)
		{
			fileList.erase(it);
			return;
		}
	}
}



