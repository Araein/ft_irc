#include "irc.hpp"

channel::~channel(void) {}
channel::channel(std::string name): _nbUser(0), _name(name) {}


std::string channel::getChannelName() const
{ return _name; }

bool channel::getConnected(client const &user) const
{
	for (size_t i = 0; i < connected.size(); i++)
	{
		if (connected[i].getID() == user.getID())
			return true;
	}
	return false;
}

bool channel::getConnectedFromString(std::string const &user) const
{
	for (size_t i = 0; i < connected.size(); i++)
	{
		if (connected[i].getNickname() == user)
			return true;
	}
	return false;
}

client* channel::getClient(const std::string& user)
{
    for (size_t i = 0; i < connected.size(); i++)
    {
        if (connected[i].getNickname() == user)
            return &connected[i];
    }
    return NULL;
}


bool channel::getAdmin(client const &user) const
{
	for (size_t i = 0; i < admin.size(); i++)
	{
		if (admin[i].getID() == user.getID())
			return true;
	}
	return false;
}

bool channel::getIsBanned(client const &user) const
{
	for (size_t i = 0; i < banned.size(); i++)
	{
		if (banned[i].getID() == user.getID())
			return true;
	}
	return false;
}

int channel::getNbUser(void) const
{ return _nbUser; }


void channel::setConnect(client const &user)
{
	for (size_t i = 0; i < connected.size(); i++)
	{
		if (connected[i].getID() == user.getID())
			return;
	}
	connected.push_back(user);
}

void channel::setDisconnect(client const &user)
{
	for (size_t i = 0; i < connected.size(); i++)
	{
		if (connected[i].getID() == user.getID())
			connected.erase(connected.begin() + i);
	}
}


void channel::setAdminTrue(client const &user)
{
	for (size_t i = 0; i < admin.size(); i++)
	{
		if (admin[i].getID() == user.getID())
			return;
	}
	admin.push_back(user);
}

void channel::setAdminFalse(client const &user)
{
	for (size_t i = 0; i < admin.size(); i++)
	{
		if (admin[i].getID() == user.getID())
			admin.erase(admin.begin() + i);
	}
}

void channel::setBannedTrue(client const &user)
{
	for (size_t i = 0; i < banned.size(); i++)
	{
		if (banned[i].getID() == user.getID())
			return;
	}
	banned.push_back(user);
}

void channel::setBannedFalse(client const &user)
{
	for (size_t i = 0; i < banned.size(); i++)
	{
		if (banned[i].getID() == user.getID())
			banned.erase(banned.begin() + i);
	}
}

void channel::setNbUserUp(void) { _nbUser++; }
void channel::setNbUserDown(void) { _nbUser--; }

void channel::sendToChannel(client const &user, std::string message)//ne semble pas fonctionner
{
	for (int i = 0; i < _nbUser; i++)
	{
		if (connected[i].getID() != user.getID())
		{
			if (send(connected[i].getFD(), message.c_str(), message.size(), 0) == -1)
				std::cout << "erreur send" << std::endl;
		}
	}
}

