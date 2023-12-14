#include "irc.hpp"

void server::parseCommand(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string param;
	std::string txt;
	std::string msg;
	iss >> command >> param >> txt;
	std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << command << NONE << std::endl;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	if (command == "kick" || command == "KICK")
	{
		if (param.size() == 0)
		{
			msg = CLIENT + "461 " + mapUser.find(fd)->second.getNickname() + " :Need more parameters\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		cmdKick(fd, buff);
	}
	else if (command == "nick" || command == "NICK")
	{
		cmdNick(fd, buff);
	}
	else if (command == "privmsg" || command == "PRIVMSG" || command == "msg" || command == "MSG")
	{
		cmdPrivmsg(fd, buff);
	}
	else if (command == "notice" || command == "NOTICE")
	{
		cmdNotice(fd, buff);
	}
	else if (command == "join" || command == "JOIN")
	{
		cmdJoin(buff, fd);
	}
	else if (command == "invite" || command == "INVITE")
	{
		cmdInvite(fd, buff);
	}
	else if (command == "topic" || command == "TOPIC")
	{
		cmdTopic(fd, buff);
	}
	else if (command == "mode" || command == "MODE")
	{
		if (param.size() == 0)
		{
			msg = CLIENT + "461 " + mapUser.find(fd)->second.getNickname() + " :Need more parameters\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		if (selectUser(param) != mapUser.end())
		{
			if (txt == "+i")
				msg = CLIENT + "324 " + mapUser.find(fd)->second.getNickname() + " :" + param + " +i\r\n";
			else
				msg = CLIENT + "472 " + mapUser.find(fd)->second.getNickname() + " :" + txt + " is not implemented in 42_IRC\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		cmdMode(fd, buff);
	}
	else if (command == "part" || command == "PART")
	{
		if (param.size() == 0)
		{
			msg = CLIENT + "461 " + mapUser.find(fd)->second.getNickname() + " :Need more parameters\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		else
			cmdPart(fd, buff);
	}
	else if (command == "ping" || command == "PING")
	{
		cmdPing(buff, fd);
	}
	else if (command == "pass" || command == "PASS")
	{
		cmdPass(param, fd);
	}
	else if (command == "quit" || command == "QUIT")
	{
		std::istringstream iss2(buff);
		std::string cmd;
		std::string mess;
		iss2 >> cmd >> mess ;
		std::cout << GREEN << "[42_IRC:  USER DISCONNECTED] " << mapUser.find(fd)->second.getNickname() << " " << mess << NONE << std::endl;
		closeOne(fd);
	}
	else
	{
		std::string temp;
		if (buff[buff.size() - 1] == '\n' && selectTrunc(fd) == truncCmd.end())
		{
			msg = CLIENT + "421 " + mapUser.find(fd)->second.getNickname() + " :" + command + " not found in 42_IRC\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			std::cout << BLUE << "[42_IRC:  COMMAND NOT FOUND] " << command << NONE << std::endl;
			return;	
		}
		if (buff[buff.size() - 1] == '\n' && selectTrunc(fd) != truncCmd.end())
		{
			selectTrunc(fd)->second += buff;
			temp = selectTrunc(fd)->second;
			truncCmd.erase(selectTrunc(fd));
			parseCommand(temp, fd);
			return;
		}
		if (selectTrunc(fd) != truncCmd.end())
		{
			if (buff[buff.size() - 1] == '\r')
				selectTrunc(fd)->second += buff.substr(0, buff.size() - 1);
			else
				selectTrunc(fd)->second += buff;
		}
		else
		{
			if (buff[buff.size() - 1] == '\r')
				truncCmd.insert(std::make_pair(fd, buff.substr(0, buff.size() - 1)));
			else
				truncCmd.insert(std::make_pair(fd, buff));
		}
	}
}

void server::cmdKick(int fd, std::string buff)
{
	std::istringstream iss1(buff);
	std::string buff2;
	std::map<int, client>::iterator it = mapUser.find(fd);
	unsigned int countchannel = 0;
	if (it != mapUser.end())
	{
		client &kicker = it->second;
		while (std::getline(iss1, buff2))
		{
			buff2 = buff2 + "\n";
			std::istringstream iss(buff2);
			std::string message, kickCommand, mychannel, nicks, reason;
			std::vector<channel>::iterator channelIt;
			iss >> kickCommand;
			iss >> mychannel;
			std::vector<std::string> channels;
			std::istringstream iss3(mychannel);
			std::string channel;
			while (std::getline(iss3, channel, ','))
				channels.push_back(channel);
			iss >> nicks;
			std::getline(iss, reason);
			if (reason == " :\r")
				reason = " :" + kicker.getNickname() + "\r";
			std::vector<std::string>::iterator actualChannel;
			for (actualChannel = channels.begin(); actualChannel != channels.end(); ++actualChannel)
			{
				bool notadmin = false;
				bool nochannel = false;
				bool notmember = false;	
				mychannel = *actualChannel;

				for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
				{
					if (channelIt->getChannelName() == mychannel)
					{
						nochannel = true;
						if (channelIt->getIsConnected(kicker.getID()))
						{
							notmember = true;
							if (channelIt->getIsChanOp(kicker.getID()))
								notadmin = true;
						}
						break;
					}
				}
				if (!nochannel)
				{
					if  (countchannel < channels.size())
					{
						message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 403 " + kicker.getUsername() + " " + mychannel + " :No such channel\r\n";
						send(kicker.getFD(), message.c_str(), message.size(), 0);
					}
					countchannel++;
					continue ;
				}
				else if (!notmember)
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 442 " + kicker.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
					countchannel++;
					continue ;
				}
				else if (!notadmin)
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 482 " + kicker.getUsername() + " " + mychannel + " :You're not channel operator\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
					countchannel++;
					continue ;
				}
				if (channelIt->getConnectedFromString(nicks))
				{
					client *targetClient = channelIt->getClient(nicks);
					if (!targetClient)
					{
						countchannel++;
						continue;
					}
					channelIt->unsetUserInvited(*targetClient);
					channelIt->setUserDisconnect(*targetClient);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					send(targetClient->getFD(), message.c_str(), message.size(), 0);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					channelIt->sendToChannelnoPRIVMSG(*targetClient, message);
				}
				else
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 401 " + kicker.getUsername() + " " + nicks + " :No such nick/channel\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
				}
				countchannel++;
			}
		}
	}
}

void server::cmdNick(int fd, std::string buff)
{
	std::vector<std::string> vec = splitCommand(buff);
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	if (vec.size() == 1 || vec[1].size() == 0)
	{
		msg = CLIENT + "431 " + mapUser.find(fd)->second.getNickname() + " :Your new nickname is empty and can't be applied\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameUserCheck(vec[1]) == false)
	{
		msg = CLIENT + "432 :Nick " + vec[1] + " is invalid and can't be applied\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameExist(vec[1]) == false)
	{
		msg = CLIENT + "433 " + mapUser.find(fd)->second.getNickname() + " :can't be applied " + vec[1] + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	std::string newNick;
	if (vec[1].size() > 20)
		newNick = vec[1].substr(0, 19) + "_";
	else
		newNick = vec[1];
	userUpDate(mapUser.find(fd)->second, newNick);
	mapUser.find(fd)->second.setNickname(newNick);
	CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	msg = CLIENT + "001 " + mapUser.find(fd)->second.getNickname() + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

void server::cmdPrivmsg(int fd, std::string buff)
{
	std::vector<channel>::iterator it;
	std::string msg;
	std::vector<std::string> vec = splitCommandPrivmsg(buff);
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	if (vec.size() == 1)
	{
		msg = CLIENT + "411 " + mapUser.find(fd)->second.getNickname() + " :No recipient specified\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (vec.size() == 2)
	{
		msg = CLIENT + "412 " + mapUser.find(fd)->second.getNickname() + " :No message received\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if ((it = selectChannel(vec[1])) == channelList.end())
	{
		if (selectUser(vec[1]) == mapUser.end())
		{
			msg = CLIENT + "401 " + mapUser.find(fd)->second.getNickname() + " " + vec[1] + " :\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		cmdPrivateMsg(fd, vec);
		return;
	}
	if (it->userCanWrite(mapUser.find(fd)->second) == true) 
	{
		std::string str = vec[2].substr(0, 5);
		if (str == "!bot ")
			mybot(fd, vec[2], vec[1]);
		else if (str == "!trf ")
		{
			std::istringstream iss2(&vec[2][5]);
			std::string cmd;
			iss2 >> cmd;
			if (cmd == "send")
				trfSend(fd, &vec[2][5], vec[1]);
			else if (cmd == "get")
				trfGet(fd, &vec[2][5], vec[1]);
			else if (cmd == "del")
				trfDel(fd, &vec[2][5], vec[1]);
			else
				trfHelp(fd);
		}
		else
			it->sendToChannel(mapUser.find(fd)->second, vec[2]);
		std::cout << YELLOW << vec[1] << ": <" << mapUser.find(fd)->second.getNickname() << "> " << vec[2] << NONE << std::endl;
	}
}

void server::cmdJoin(std::string buff, int fd)	
{
	std::string msg;
	std::map<std::string, std::string> chanPass = splitCommandJoin(buff);
	std::map<std::string, std::string>::iterator it_chanPass = chanPass.begin();
	std::vector<channel>::iterator it_channelList;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	int nbChan = mapUser.find(fd)->second.getHowManyChannel();

	if (chanPass.size() == 0)
	{
		msg = CLIENT + "461 " + mapUser.find(fd)->second.getNickname() + " :Need more parameters\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	while (it_chanPass != chanPass.end())
	{
		nbChan++;
		if (nbChan > maxChannelConnect)
		{
			msg = CLIENT + "405 " + mapUser.find(fd)->second.getNickname() + " " + it_chanPass->first + " :\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		if ((it_channelList = selectChannel(it_chanPass->first)) != channelList.end())
		{
			if (it_channelList->userCanJoin(mapUser.find(fd)->second, it_chanPass->second) == true)
			{
				it_channelList->setUserConnect(mapUser.find(fd)->second);
			}
		}
		else
		{
			std::string channelName; 
			if (it_chanPass->first[0] != '#' || it_chanPass->first.size() == 1 || it_chanPass->first[1] == ' ')
			{
				std::string msg = CLIENT + "476 " + mapUser.find(fd)->second.getUsername() + " :" + it_chanPass->first + "\r\n";
				send(fd, msg.c_str(), msg.size(), 0);
			}
			else
			{
				if (it_chanPass->first.size() > 50)
					channelName = it_chanPass->first.substr(0, 49) + "_";
				else
					channelName = it_chanPass->first;
				channel temp(channelName);
				temp.setUserConnect(mapUser.find(fd)->second);
				channelList.push_back(temp);
			}
		}
		it_chanPass++;
	}
}

void server::cmdInvite(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::map<int, client>::iterator it = mapUser.find(fd);
	if (it != mapUser.end())
	{
		client &inviter = it->second;
		std::string message, inviteCommand, mychannel, user;
		std::vector<channel>::iterator channelIt;
		std::map<int, client>::iterator it2;
		iss >> inviteCommand;
		iss >> user;
		iss >> mychannel;
		bool nochannel = false;
		bool notmember = false;
		bool alreadymember = false;
		for (it2 = mapUser.begin(); it2 != mapUser.end(); ++it2) 
		{
			if (it2->second.getNickname() == user) 
				break;
		}
		if (it2 == mapUser.end())
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 401 " + inviter.getNickname() + " " + user + " :No such nick/channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
		{
			if (channelIt->getChannelName() == mychannel)
			{
				nochannel = true;
				if (channelIt->getIsConnected(inviter.getID()))
					notmember = true;
				if (channelIt->getConnectedFromString(user))
					alreadymember = true;	
				break;
			}
		}
		if (!nochannel)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 403 " + inviter.getNickname() + " " + mychannel + " :No such channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (!notmember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 442 " + inviter.getNickname() + " " + mychannel + " :You're not on that channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (alreadymember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 443 " + inviter.getNickname() + " " + user + " " + mychannel + " :is already on channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;	
		}
		if (channelIt->getMode('i') && !(channelIt->getIsChanOp(inviter.getID())))
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 482 " + inviter.getNickname() + " " + mychannel + " :You're not channel operator\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		channelIt->setUserInvited(it2->second);
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 341 " + inviter.getNickname() + " " + user + " " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getNickname() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		message = ":" + it2->second.getNickname() + "!" + it2->second.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getNickname() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(it2->second.getFD() , message.c_str(), message.size(), 0);
	}
}

void server::cmdTopic(int fd, std::string buff)
{
	std::string tmp;
	std::string channelName;
	std::istringstream iss(buff);

	iss >> tmp;
	iss >> channelName;
	std::vector<channel>::iterator it_channelList = selectChannel(channelName);
	if (it_channelList == channelList.end()){
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 403" + channelName + ":no such channel\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 403 " + channelName + ":no such channel\r\n").length(), 0);
		return ;
	}
	tmp = iss.str().substr(6 + channelName.length() + 1, buff.length() - (6 + channelName.length() + 1));
	if (it_channelList->getIsConnected(mapUser.find(fd)->second.getID()) == true){
		if (tmp.empty() == true && it_channelList->getTopic().empty() == true){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :No topic is set\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 331 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :No topic is set\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == true && it_channelList->getTopic().empty() == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :" + it_channelList->getTopic() + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 332 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :" + it_channelList->getTopic() + "\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == true && it_channelList->getIsChanOp(mapUser.find(fd)->second.getID()) == false){
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :You do not have permission to change the topic\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + " :You do not have permission to change the topic\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == true && it_channelList->getIsChanOp(mapUser.find(fd)->second.getID()) == true){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannelnoPRIVMSG(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == false){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannelnoPRIVMSG(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
			return ;
		}
	}
	else{
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channelName + " :You are not part of this channel\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channelName + " :You are not part of this channel\n").length(), 0);
		return ;
	}
}

void server::cmdMode(int fd, std::string buff)
{
	std::istringstream iss(buff);
	int mode = -1;
	std::string str(iss.str());
	std::string cmd;
	std::string chan;
	std::string arg;
	std::string msg;
	std::string tmp;
	std::vector<std::string> keys;
	std::vector<std::string> values;
	client user = mapUser.find(fd)->second;
	std::vector<channel>::iterator itchan = channelList.begin();

	iss >> cmd;
	iss >> chan;
	iss >> arg;
	if (arg == "b")
		return ;
	if (chan[0] != '#')
		return;
	while (itchan != channelList.end() && itchan->getChannelName() != chan){
		itchan++;
	}
	if (itchan == channelList.end()){
		msg = ":localhost 403 " + user.getNickname() + " " + chan + " :No such channel\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
		return ;
	}
	if (arg.empty()){
		msg = ":localhost 324 " + user.getNickname() + " " + chan + " +" + itchan->getAllMode() + "\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (itchan->getIsChanOp(user.getID()) == false){
		std::string error = ":localhost 482 " + user.getNickname() + " " + chan + " " + ":You're not channel operator\r\n";
		send(fd, error.c_str(), error.length(), 0);
		return ;
	}
	std::string argmsg = "";
	if (arg[0] == '+' || arg[0] == '-'){
		msg = ":" + user.getNickname() + "!" + user.getUsername() + "@localhost MODE " + chan + " ";
		for (std::string::iterator it = str.begin() + (cmd.length() + chan.length() + 2); it != str.end(); it++){
			if (*it == '+' || mode == 1){
				if (mode == 0 || mode == -1){
					mode = 1;
					argmsg += "+";
				}
				if (*it == 'i' && argmsg.find('i') == std::string::npos && arg.find('i') != std::string::npos){
					itchan->setMode('i', true);
					itchan->setAllInvited();
					argmsg += "i";
				}
				if (*it == 't' && argmsg.find('t') == std::string::npos && arg.find('t') != std::string::npos){
					itchan->setMode('t', true);
					argmsg += "t";
				}
				if (*it == 'k' && argmsg.find('k') == std::string::npos && arg.find('k') != std::string::npos){
					iss >> tmp;
					if (tmp.empty() == false){
						itchan->setPassword(tmp);
						itchan->setMode('k', true);
						itchan->setNeedPass(true);
						argmsg += "k";
						keys.push_back("password");
						values.push_back(tmp);
					}
				}
				if (*it == 'o' && arg.find('o') != std::string::npos){
					iss >> tmp;
					if (tmp.empty() == true && argmsg.find('o') != std::string::npos)
						continue ;
					else if (tmp.empty() == false){
						if (itchan->getClient(tmp)){
							itchan->setUserChanOp(*itchan->getClient(tmp));
							itchan->setMode('o', true);
							argmsg += "o";
							keys.push_back("chanops");
							values.push_back(tmp);
						}
						else{
							std::string error = ":localhost 401 " + user.getNickname() + " " + tmp + " :no such nick\r\n";
							send(fd, error.c_str(), error.length(), 0);
							error = ":localhost 441 " + user.getNickname() + " " + tmp + " " + itchan->getChannelName() + " :user not on channel\r\n";
							send(fd, error.c_str(), error.length(), 0);
						}
					}
				}
				if (*it == 'l' && arg.find('l') != std::string::npos){
					iss >> tmp;
					if (tmp.empty() == false && std::atoi(tmp.c_str()) > 0){
						itchan->setMaxConnectedUser(std::atoi(tmp.c_str()));
						itchan->setMode('l',true);
						argmsg += "l";
						keys.push_back("limit");
						values.push_back(tmp);
					}
					else if (tmp.empty()){
						std::string error = ":localhost 461 " + user.getNickname() + " MODE +l :Not enough parameters\r\n";
						send(fd, error.c_str(), error.length(), 0);
					}
				}
				if (*it == '-'){
					mode = 0;
					argmsg += "-";
				}
				if (std::string("+-ilokt\r\n ").find(*it) == std::string::npos){
					std::string error = ":localhost 472 " + user.getNickname() + *it + " :is unknown mode char to me\r\n";
					send(fd, error.c_str(), error.length(), 0);
				}
			}
			if (*it == '-' || mode == 0){
				if (mode == 1 || mode == -1){
					mode = 0;
					argmsg += "-";
				}
				if (*it == 'i' && argmsg.find('i') == std::string::npos && arg.find('i') != std::string::npos){
					itchan->setMode('i', false);
					argmsg += "i";
				}
				if (*it == 't'  && argmsg.find('t') == std::string::npos && arg.find('t') != std::string::npos){
					itchan->setMode('t', false);
					argmsg += "t";
				}
				if (*it == 'k' && arg.find('k') != std::string::npos){
					if (argmsg.find('k') != std::string::npos && findKey(keys, "password") == true)
					{
						if (itchan->getMode('k') == true){
							itchan->setMode('k', false);
							itchan->setNeedPass(false);
							itchan->setPassword("");
							argmsg += "k";
						}
					}
					else if (argmsg.find('k') == std::string::npos && findKey(keys, "password") == false){
						if (itchan->getMode('k') == true){
							itchan->setMode('k', false);
							itchan->setNeedPass(false);
							itchan->setPassword("");
							argmsg += "k";
						}
					}
				}
				if (*it == 'o' && arg.find('o') != std::string::npos){
					iss >> tmp;
					if (tmp.empty() == true && argmsg.find('o') != std::string::npos)
						continue ;
					if (tmp.empty() == false ){
						if (itchan->getClient(tmp)){
							itchan->undoUserChanOp(*itchan->getClient(tmp));
							argmsg += 'o';
							keys.push_back("chanopsremove");
							values.push_back(tmp);
						}
						else{
							std::string error = ":localhost 401 " + user.getNickname() + " " + tmp + " :no such nick\r\n";
							send(fd, error.c_str(), error.length(), 0);
							error = ":localhost 441 " + user.getNickname() + " " + tmp + " " + itchan->getChannelName() + " :user not on channel\r\n";
							send(fd, error.c_str(), error.length(), 0);
						}
					}
				}
				if (*it == 'l' && arg.find('l') != std::string::npos){
					itchan->setMode('l', false);
					itchan->setMaxConnectedUser(1000);
					argmsg += "l";
				}
				if (std::string("+-ilokt\r\n ").find(*it) == std::string::npos){
					std::string error = ":localhost 472 " + user.getNickname() + *it + " :is unknown mode char to me\r\n";
					send(fd, error.c_str(), error.length(), 0);
				}
			}
			if (*it == ' '){
				it += jumpToNextMode(it);
			}
		}
	}
	std::string finalmsg(msg + argmsg);
	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++){
		finalmsg.append(" " + *it);
	}
	finalmsg += "\r\n";
	send(fd, finalmsg.c_str(), finalmsg.length(), 0);
	std::vector<client> vec = itchan->getConnectedVector();
	for (std::vector<client>::iterator it = vec.begin(); it != vec.end(); it++){
		if (fd != it->getFD())
		send(it->getFD(), finalmsg.c_str(), finalmsg.length(), 0);
	}
}

void server::cmdPing(std::string buff, int fd)
{
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	std::istringstream iss(buff);
	std::string str1;
	std::string str2;
	iss >> str1 >> str2;
	if (str2.size() == 0)
	{
		std::string msg = CLIENT + "409 " + mapUser.find(fd)->second.getNickname() + " :No origin specified\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	std::string pongMsg = "PONG " + str2 + "\r\n";
	send(fd, pongMsg.c_str(), pongMsg.size(), 0);
}

void server::cmdPart(int fd, std::string buff)
{
	std::istringstream iss(buff);
	std::map<int, client>::iterator it = mapUser.find(fd);
	if (it != mapUser.end())
	{
		client &parter = it->second;
		std::string message, partCommand, mychannel, reason;
		std::vector<channel>::iterator channelIt;
		iss >> partCommand;
		iss >> mychannel;
		std::vector<std::string> channels;
		std::istringstream iss3(mychannel);
		std::string channel;
		while (std::getline(iss3, channel, ','))
			channels.push_back(channel);
		std::getline(iss, reason);
		if (reason == " :\r")
			reason = " :" + parter.getNickname() + "\r";
		std::vector<std::string>::iterator actualChannel;
		for (actualChannel = channels.begin(); actualChannel != channels.end(); ++actualChannel)
		{
			bool nochannel = false;
			bool notmember = false;	
			mychannel = *actualChannel;
			for (channelIt = channelList.begin(); channelIt != channelList.end(); ++channelIt)
			{
				if (channelIt->getChannelName() == mychannel)
				{
					nochannel = true;
					if (channelIt->getIsConnected(parter.getID()))
						notmember = true;
					break;
				}
			}
			if (!nochannel)
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 403 " + parter.getUsername() + " " + mychannel + " :No such channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
			else if (!notmember)
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 442 " + parter.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
		
			channelIt->unsetUserInvited(parter);
			channelIt->setUserDisconnect(parter);
			message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost PART " + mychannel + reason + "\n";
			channelIt->sendToChannelnoPRIVMSG(parter, message);
			send(fd, message.c_str(), message.size(), 0);
			
			if (channelIt->getNbConnectedUser() == 0)
			{
				if (channelIt->getConnectedFromString("MrRobot"))
					channelIt->setUserDisconnect(*MrRobot);
				channelList.erase(channelIt);
			} 
		}
	}
}

void server::cmdPass(std::string password, int fd)
{
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	if (mapUser.find(fd)->second.getLog() > 0)
	{
		msg = CLIENT + "462 " + mapUser.find(fd)->second.getNickname() + " :You have already logged in\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (password.size() == 0)
	{
		msg = CLIENT + "464 " + mapUser.find(fd)->second.getNickname() + " :Your password is empty. Please try again" + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (_password.compare(password) == 0)
	{
		mapUser.find(fd)->second.setLog();
		msg = CLIENT + "372 " + mapUser.find(fd)->second.getNickname() + " :" + GREEN + BOLD + "You have successfully logged in" + NONE + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		sendWelcomMsgs(fd);
		return;
	}
	msg = CLIENT + "464 "+ mapUser.find(fd)->second.getNickname() + " :" + RED + BOLD + "Invalid password, you will be disconnected" + NONE + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
	std::cout << RED << BOLD << "[42_IRC:  USER FAILED TO LOG IN] "<< mapUser.find(fd)->second.getNickname() << NONE << std::endl;
	closeOne(fd);
}

void server::cmdPrivateMsg(int fd, std::vector<std::string> vec)
{
	std::map<int, client>::iterator it_recip = selectUser(vec[1]);
	std::map<int, client>::iterator it_sender = selectUser(mapUser.find(fd)->second.getNickname());
	std::vector<privChannel>::const_iterator it_privlist = selectPrivChan(it_sender->second.getNickname(), it_recip->second.getNickname());
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	if (it_recip->second.getNickname() == "server" || it_recip->second.getNickname() == "MrRobot" || it_recip->second.getNickname() == "chanOp42")
	{
		std::string msg = CLIENT + "401 " + mapUser.find(fd)->second.getNickname() + " :\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (it_privlist == privateList.end())
	{
		privChannel pc;
		pc.name1 = it_sender->second.getNickname();
		pc.name2 = it_recip->second.getNickname();
	}
	std::string str = vec[2].substr(0, 5);
	if (str == "!bot " || str == "!trf ")
	{
		std::string msg = CLIENT + "372 " + mapUser.find(fd)->second.getNickname() + " :To use !bot or !trf join a channel (not private channel)\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
	}
	std::string msg = CLIENT + "PRIVMSG " + it_recip->second.getNickname() + " " + vec[1] + " :" + vec[2] + "\r\n";
	send(it_recip->first, msg.c_str(), msg.size(), 0);
	std::cout << YELLOW << vec[1] << ": <" << mapUser.find(fd)->second.getNickname() << "> " << vec[2] << NONE << std::endl;
}

void server::cmdNotice(int fd, std::string buff)
{
	std::vector<std::string> vec = splitCommandPrivmsg(buff);
	if (vec.size() < 3)
		return;
	std::vector<channel>::iterator it_chan = selectChannel(vec[1]);
	std::map<int, client>::iterator it_user = selectUser(vec[1]);
	if (it_chan != channelList.end())
		it_chan->sendToChannelNotice(mapUser.find(fd)->second, vec[2]);
	else if (it_user != mapUser.end())
	{
		std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost NOTICE ";
		std::string  msg = CLIENT + mapUser.find(fd)->second.getNickname() + " :" + vec[2] + "\r\n";
		send(it_user->first, msg.c_str(), msg.size(), 0);
	}
}

