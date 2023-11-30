#include "irc.hpp"

void server::parseCommand(std::string buff, int fd)
{
	std::istringstream iss(buff);
	std::string command;
	std::string param;
	std::string msg;
	iss >> command >> param;
	std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << command << NONE << std::endl;
	if (command == "kick" || command == "KICK")
	{
		cmdKick(fd, buff);
	}
	else if (command == "nick" || command == "NICK")
	{
		cmdNick(fd, buff);
	}
	else if (command == "privmsg" || command == "PRIVMSG")
	{
		cmdPrivmsg(fd, buff);
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
		cmdMode(fd, buff);
		
		std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
		msg = CLIENT + "324 " + mapUser.find(fd)->second.getNickname() + " :" + param + " +i\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
	}
	else if (command == "part" || command == "PART")
	{
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
	else if (command == "who" || command == "WHO")
	{
		std::cout << "DEBUG WHO:" << buff << std::endl;
		// cmdWho(param, fd);
	}
	else if (command == "whois" || command == "WHOIS")
	{
		std::cout << "DEBUG WHOIS:" << buff << std::endl;
		// cmdWhois(param, fd);
	}
	else if (command == "quit" || command == "QUIT")
	{
		std::cout << GREEN << "[42_IRC:  USER DISCONNECTED] " << mapUser.find(fd)->second.getNickname() << NONE << std::endl;
		closeOne(fd);
	}
	else
	{
		std::string temp;
		if (buff[buff.size() - 1] == '\n' && _partCommand.size() == 0)
		{
			std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
			msg = CLIENT + "421 " + mapUser.find(fd)->second.getNickname() + " :" + command + " unknown to 42_IRC\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			std::cout << BLUE << "[42_IRC:  COMMAND NOT SUPPORTED] " << command << NONE << std::endl;
			return;	
		}
		if (buff[buff.size() - 1] == '\n' && _partCommand.size() > 0)
		{
			_partCommand += buff;
			temp = _partCommand;
			_partCommand = "";
			parseCommand(temp, fd);
			return;
		}
		if (buff[buff.size() - 1] == '\r')
			_partCommand += buff.substr(0, buff.size() - 1);
		else
			_partCommand += buff;
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
				if (!nochannel) // si channel n'existe pas
				{
					if  (countchannel < channels.size())
					{
						message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 403 " + kicker.getUsername() + " " + mychannel + " :No such channel\r\n";
						send(kicker.getFD(), message.c_str(), message.size(), 0);
					}
					countchannel++;
					continue ;
				}
				else if (!notmember) //si kicker pas membre
				{
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost 442 " + kicker.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
					send(kicker.getFD(), message.c_str(), message.size(), 0);
					countchannel++;
					continue ;
				}
				else if (!notadmin) //si kicker pas admin
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
					channelIt->setUserDisconnect(targetClient);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					send(targetClient->getFD(), message.c_str(), message.size(), 0);
					message = ":" + kicker.getNickname() + "!" + kicker.getUsername() + "@localhost KICK " + mychannel + " " + nicks + reason + "\n";
					channelIt->sendToChannelnoPRIVMSG(*targetClient, message);
				}
				else //si le membre n'est pas sur le channel
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
	std::vector<std::string> vec = splitCommandNick(buff);
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	if (vec.size() == 1 || vec[1].size() == 0)
	{
		msg = CLIENT + "431 " + mapUser.find(fd)->second.getNickname() + " :Your new nickname is empty and has not been changed\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameUserCheck(vec[1]) == false)
	{
		msg = CLIENT + "432 " + mapUser.find(fd)->second.getNickname() + " :Nick " + vec[1] + " is invalid and has not been changed\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	else if (nameExist(vec[1]) == false)
	{
		msg = CLIENT + "433 " + mapUser.find(fd)->second.getNickname() + " :has not been changed " + vec[1] + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	std::string newNick;
	if (vec[1].size() > 20)
		newNick = vec[1].substr(0, 19);
	else
		newNick = vec[1];
	userUpDate(&mapUser.find(fd)->second, newNick);
	mapUser.find(fd)->second.setNickname(newNick);
	//Gestion du username
	if (vec.size() >= 3)
	{
		if (vec[2].size() > 0 || nameUserCheck(vec[2]) == true)
		{
			if (vec[2].size() > 20)
				mapUser.find(fd)->second.setUsername(vec[2].substr(0, 19));
			else
				mapUser.find(fd)->second.setUsername(vec[2]);
		}
	}
	CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	msg = CLIENT + "001 " + mapUser.find(fd)->second.getNickname() + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
}


// void server::cmdJoin(std::string buff, int fd)
// {
// 	std::istringstream iss(buff);
// 	std::string cmd;
// 	std::string name;
// 	iss >> cmd >> name;
// 	int join = 0;
	
// 	std::map<int, client>::iterator it = mapUser.find(fd);
//     client& myclient = it->second;

// 	for (size_t i = 0; i < channelList.size(); i++)
// 	{
// 		if (channelList[i].getChannelName().compare(name) == 0) // verifie si le channel existe deja
// 		{
// 			join = 1;
// 			channelList[i].setNbUserUp();//incremente le nombre de connectes
// 			channelList[i].setConnect((mapUser.find(fd))->second);//insert le nouveau clien client dans un vector dans channel
// 			if (channelList[i].getNbUser() == 1)//si c est le 1er connecte il devient admin
// 				channelList[i].setAdminTrue((mapUser.find(fd))->second);
// 			std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + channelList[i].getChannelName() + "\r\n";		
// 			send(fd, message.c_str(), message.size(), 0);
// 		}
// 	}
// 	if (join == 0)//si le channel n existe pas on le cree
// 	{
// 		channel temp(name);
// 		temp.setNbUserUp();
// 		temp.setConnect((mapUser.find(fd))->second);
// 		temp.setAdminTrue((mapUser.find(fd))->second);
// 		channelList.push_back(temp);
// 		std::string message = ":" + myclient.getNickname() + "!" + myclient.getUsername() + "@localhost JOIN " + temp.getChannelName() + "\r\n";		
// 		send(fd, message.c_str(), message.size(), 0);
// 	}
// }
	
void server::cmdPrivmsg(int fd, std::string buff)
{
	std::vector<channel>::iterator it;
	std::string msg;
	std::vector<std::string> vec = splitCommandPrivmsg(buff);
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost ";
	if (vec.size() == 1)
	{
		msg = CLIENT + "411 " + mapUser.find(fd)->second.getNickname() + " :No recipient given\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (vec.size() == 2)
	{
		msg = CLIENT + "412 " + mapUser.find(fd)->second.getNickname() + " :No message given\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if ((it = selectChannel(vec[1])) == channelList.end())
	{
		msg = CLIENT + "403 " + mapUser.find(fd)->second.getNickname() + " :'" + vec[1] + "'\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		return;
	}
	if (it->userCanWrite(&mapUser.find(fd)->second, vec[1]) == true) 
	{
		std::string txt;
		if (vec[2][0] == ':')
			txt = vec[2].substr(1, vec[2].size());
		else
			txt = vec[2];
		std::cout << YELLOW << vec[1] << ": <" << mapUser.find(fd)->second.getNickname() << "> " << txt << NONE << std::endl;
		it->sendToChannel(mapUser.find(fd)->second, txt);
 		if (txt.length() >= 5)
		{
			std::string checkbot = txt.substr(0, 5);
			if (checkbot == "!bot ")
				mybot(fd, txt, vec[1]);
		}		
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
		if (nbChan > 4)//mettre a 20
		{
			msg = CLIENT + "405 " + mapUser.find(fd)->second.getNickname() + " " + it_chanPass->first +  ":\r\n";
			send(fd, msg.c_str(), msg.size(), 0);
			return;
		}
		if ((it_channelList = selectChannel(it_chanPass->first)) != channelList.end())
		{
			if (it_channelList->userCanJoin(&mapUser.find(fd)->second, it_chanPass->second) == true)
				it_channelList->setUserConnect(&mapUser.find(fd)->second);
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
					channelName = it_chanPass->first.substr(0, 49);
				else
					channelName = it_chanPass->first;
				channel temp(channelName);
				temp.setUserConnect(&mapUser.find(fd)->second);
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
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 401 " + inviter.getUsername() + " " + user + " :No such nick/channel\r\n";
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
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 403 " + inviter.getUsername() + " " + mychannel + " :No such channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (!notmember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 442 " + inviter.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;
		}
		else if (alreadymember)
		{
			message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 443 " + inviter.getUsername() + " " + user + " " + mychannel + " :is already on channel\r\n";
			send(inviter.getFD(), message.c_str(), message.size(), 0);
			return ;	
		}
		// if (channelIt->private() && !(channelIt->getAdmin(inviter))) //-> si channel privé et user pas operator de ce channel
		// {
		// 	message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 482 " + inviter.getUsername() + " " + mychannel + " :You're not channel operator\r\n";
		// 	channelIt->sendToChannel(inviter, message);
		// 	return ;
		// }

		// ajouter le user a la liste des invités/admis du channel ici
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost 341 " + inviter.getUsername() + " " + user + " " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		std::cout << message << std::endl;
		message = ":" + inviter.getNickname() + "!" + inviter.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getUsername() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(inviter.getFD(), message.c_str(), message.size(), 0);
		message = ":" + it2->second.getNickname() + "!" + it2->second.getUsername() + "@localhost NOTICE @" + mychannel + " :" + inviter.getUsername() + " invited " + user + " into channel  " + mychannel + "\r\n";
		send(it2->second.getFD() , message.c_str(), message.size(), 0);
	}
}

void server::cmdTopic(int fd, std::string buff)
{
	std::string tmp;
	std::string channelName;
	std::istringstream iss(buff);
	// int index;

	iss >> tmp;
	iss >> channelName;
	std::vector<channel>::iterator it_channelList = selectChannel(channelName);
	if (it_channelList == channelList.end()){
		send(fd, std::string("403" + channelName + ":no such channel\r\n").c_str(), std::string("403 " + channelName + ":no such channel\r\n").length(), 0);
		return ;
	}
	// index = findChanbyName(channel);
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
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + ":You do not have permission to change the topic\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 482 " + mapUser.find(fd)->second.getNickname() + " " + channelName + ":You do not have permission to change the topic\r\n").length(), 0);
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == true && it_channelList->getIsChanOp(mapUser.find(fd)->second.getID()) == true){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
			return ;
		}
		else if (tmp.empty() == false && it_channelList->getMode('t') == false){
			it_channelList->setTopic(tmp);
			send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str(), std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").length(), 0);
			it_channelList->sendToChannel(mapUser.find(fd)->second, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost TOPIC " + channelName + " " + tmp + "\r\n").c_str());
			return ;
		}
	}
	else{
		send(fd, std::string(":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() + "@localhost 442 " + channelName + ":You are not part of this channel\n").c_str(), std::string("442 " + channelName + ":You are not part of this channel\n").length(), 0);
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
	if (chan[0] != '#') // mode is sent for user mode, ignoring (CLIENT SENDS MODE +i AT BEGINNING, MAKING A CRASH LATER IN THIS FUNCTION IF THIS LINE IS REMOVED AS THE CHANNELS ARE NOT SET YET)
		return;
	while (itchan->getChannelName() != chan && itchan != channelList.end()){ //find channel | IT CHAAAAN UwU
		itchan++;
	}
	if (itchan == channelList.end()){ // channel not found, does not output anything on client side, this is expected behavior
		msg = ":localhost 403 " + user.getNickname() + " " + chan + " :No such channel\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
		return ;
	}
	if (arg.empty()){ // list modes for channel even if no modes
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
				if (*it == 'i' && argmsg.find('i') == std::string::npos && arg.find('i') != std::string::npos){ // found i mode, setting up invite mode. Checks if there is already a i mode in the command wether it's suppresing or adding doesn't matter) if there is ignoring the i mode
					itchan->setMode('i', true);
					itchan->setAllInvited();
					argmsg += "i";
				}
				if (*it == 't' && argmsg.find('t') == std::string::npos && arg.find('t') != std::string::npos){ // found t mode, setting up restricted topic mode> Checks if there is already a t mode in the command wether it's suppresing or adding doesn't matter) if there is ignoring the t mode
					itchan->setMode('t', true);
					argmsg += "t";
				}
				if (*it == 'k' && argmsg.find('k') == std::string::npos && arg.find('k') != std::string::npos){ // found k mode, setting up password
					iss >> tmp;
					if (tmp.empty() == false){ // if password is not empty we set it up, else we ignore it. Checks if there is already a k mode in teh command (wether it's suppresing or adding doesn't matter) if there is ignoring the k mode
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
					else if (tmp.empty() == false){ // checking is arg is empty, if it is, we ignore o mode
						if (itchan->getClient(tmp)){ // checking if user provided exists, if it does setting it up as ops
							itchan->setUserChanOp(itchan->getClient(tmp));
							itchan->setMode('o', true);
							argmsg += "o";
							keys.push_back("chanops");
							values.push_back(tmp);
						}
						else{ // else return error and ignore o mode
							std::string error = ":localhost 401 " + user.getNickname() + " " + tmp + " :no such nick\r\n";
							send(fd, error.c_str(), error.length(), 0);
							error = ":localhost 441 " + user.getNickname() + " " + tmp + " " + itchan->getChannelName() + " :user not on channel\r\n";
							send(fd, error.c_str(), error.length(), 0);
						}
					}
				}
				if (*it == 'l' && arg.find('l') != std::string::npos){
					iss >> tmp;
					if (tmp.empty() == false && std::atoi(tmp.c_str()) > 0){ // checking if arg is empty, if it is, we ignore l mode and send error but if it isn't empty but invalid parameter (string or == 0) we ignore
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
				if (*it == 'i' && argmsg.find('i') == std::string::npos && arg.find('i') != std::string::npos){ // removing invite mode, if there is already a i in the command ignoring this one
					itchan->setMode('i', false);
					argmsg += "i";
				}
				if (*it == 't'  && argmsg.find('t') == std::string::npos && arg.find('t') != std::string::npos){ // removing restrictions on topic, if there is already a i in the command ignoring this one
					itchan->setMode('t', false);
					argmsg += "t";
				}
				if (*it == 'k' && arg.find('k') != std::string::npos){ // remove password if a password is set, if not ignoring
					if (argmsg.find('k') != std::string::npos && findKey(keys, "password") == true) // if there is a k before BUT the argument is not valid we ignore all the k's altogether
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
						if (itchan->getClient(tmp)){ // checking if user exists if it does remove it from admins
							itchan->undoUserChanOp(itchan->getClient(tmp));
							argmsg += 'o';
							keys.push_back("chanopsremove");
							values.push_back(tmp);
						}
						else{ // else return error and ignore o mode
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
	std::istringstream iss(buff);
	std::string pingMsg;
	iss >> pingMsg >> pingMsg; 
	std::string pongMsg = "PONG " + pingMsg + "\r\n";
	send(fd,pongMsg.c_str(), pongMsg.size(), 0);
}


//    Parameters: <channel> *( "," <channel> ) [ <Part Message> ]

//pas reussi a trigger ERR_NEEDMOREPARAMS
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
			if (!nochannel) // si channel n'existe pas
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 403 " + parter.getUsername() + " " + mychannel + " :No such channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
			else if (!notmember) //si  pas membre
			{
				message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost 442 " + parter.getUsername() + " " + mychannel + " :You're not on that channel\r\n";
				send(parter.getFD(), message.c_str(), message.size(), 0);
				continue ;
			}
			channelIt->setUserDisconnect(&parter);
			channelIt->unsetUserInvited(&parter);
			message = ":" + parter.getNickname() + "!" + parter.getUsername() + "@localhost PART " + mychannel + reason + "\n";
			channelIt->sendToChannelnoPRIVMSG(parter, message);
			send(fd, message.c_str(), message.size(), 0);
			if (channelIt->getNbConnectedUser() == 1 && channelIt->getConnectedFromString("MrRobot"))
				channelIt->setUserDisconnect(MrRobot);
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
		msg = CLIENT + "001 " + mapUser.find(fd)->second.getNickname() + " :" + GREEN + BOLD + "You have successfully logged in" + NONE + "\r\n";
		send(fd, msg.c_str(), msg.size(), 0);
		sendWelcomMsgs(fd);
		return;
	}
	msg = CLIENT + "464 "+ mapUser.find(fd)->second.getNickname() + " :" + RED + BOLD + "Invalid password, you will be disconnected" + NONE + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
	closeOne(fd);
}
