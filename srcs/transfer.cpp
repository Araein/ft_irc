#include "irc.hpp"

void server::trfSend(int fd, std::string txt, std::string channelName)
{
	std::vector<std::string> vec = splitCommand(txt);
	std::vector<channel>::iterator it_chan = selectChannel(channelName);
	std::string rep;
	std::string line;
	transferFile trf;
	if (vec.size() == 1)
		rep = "User is missing";
	else if (selectUser(vec[1]) == mapUser.end())
		rep = "User does not exist";
	else if (vec.size() == 2)
		rep = "File path is missing";
	else if (it_chan == channelList.end())
		rep = "No such channel";
	else if (it_chan->getIsConnected(selectUser(vec[1])->second.getID()) == false)
		rep = vec[1] + " has not joined " + channelName;
	else if (it_chan->getIsConnected(mapUser.find(fd)->second.getID()) == false)
		rep = "Your has not joined " + channelName;
	else
	{
		std::ifstream userFile(vec[2].c_str());
		if (!userFile.is_open())
			rep = "File cannot be opened";
		else
		{
			while (std::getline(userFile, line))
			{
				trf.textFile += line + "\n";
				if (trf.textFile.size() > 100000)
				{
					rep = "File is too large and cannot be sent";
					break;
				}
			}
			if (trf.textFile.size() == 0)
				rep = "File is empty";
			userFile.close();
		}
	}
	if (mapUser.find(fd)->second.getHowManyFile() >= maxSEND)
	{
		selectChannel(channelName)->sendToOne(mapUser.find(fd)->second, "You have too many undelivered files. Tape '!trf' for info");
		std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << " Failed to sent a file" << NONE << std::endl;
	}
	else if (rep.size() == 0)
	{
		trf.fdExp = fd;
		trf.fdDest = selectUser(vec[1])->second.getFD();
		trf.pathFile = vec[2];
		trf.filename = extractFilename(trf.pathFile);
		trf.id = mapUser.find(fd)->second.setFileList(trf);
		rep = "Download: '!trf get " + mapUser.find(fd)->second.getNickname() + " " + trf.id + "'"; 
		selectChannel(channelName)->sendToOne(selectUser(vec[1])->second, rep);
		rep = "Successfull sent file " + trf.id; 
		selectChannel(channelName)->sendToOne(mapUser.find(fd)->second, rep);
		std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << " Sent a file" << NONE << std::endl;
	}
	else
	{
		selectChannel(channelName)->sendToOne(mapUser.find(fd)->second, rep);
		std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << " Failed to sent a file" << NONE << std::endl;
	}
}

void server::trfGet(int fd, std::string txt, std::string channelName)
{
	std::vector<std::string> vec = splitCommand(txt);
	std::vector<channel>::iterator it = selectChannel(channelName);
	std::string rep;
	transferFile trf;
	if (vec.size() == 1)
		rep = "User is missing";
	else if (selectUser(vec[1]) == mapUser.end())
		rep = "User does not exist or is no more connected on 42IRC";
	else if (vec.size() == 2)
		rep = "File number is missing";
	else if (selectUser(vec[1])->second.getNumFileExist(vec[2]) == false)
		rep = "File number is invalid";
	else if (selectUser(vec[1])->second.getFileDest(vec[2]) != fd)
		rep = "You are not allowed to download this file";
	else if (it == channelList.end())
		rep = "No such channel";
	else if (it->getIsConnected(mapUser.find(fd)->second.getID()) == false)
		rep = "You has not joined " + channelName;
	else
	{
		trf = *(selectUser(vec[1])->second.getTrf(vec[2]));
		std::string path = "files/42irc_" + trf.filename;
		std::ofstream fileOut(path.c_str(), std::ofstream::trunc);
		if (!fileOut.is_open())
			rep = "Failed to download the file";
		else
		{
			fileOut << trf.textFile;
			fileOut.close();
			selectUser(vec[1])->second.delFileList(vec[2]);
			rep = "Successful download";
		}
		std::cout << CYAN << "[42_IRC: <<<< " << selectUser(vec[1])->second.getNickname() << ":" << " get a file" << NONE << std::endl;
	}
	selectChannel(channelName)->sendToOne(mapUser.find(fd)->second, rep);

}

void server::trfDel(int fd, std::string txt, std::string channelName)
{
	std::vector<std::string> vec = splitCommand(txt);
	std::string rep;
	transferFile trf;
	if (vec.size() == 1)
		rep = "File number is missing";
	else if (mapUser.find(fd)->second.getNumFileExist(vec[1]) == false)
		rep = "File number is invalid";
	else
	{
		rep = "Successfully deleted file";
		mapUser.find(fd)->second.delFileList(vec[1]);
		std::cout << CYAN << "[42_IRC: <<<< " << mapUser.find(fd)->second.getNickname() << ":" << " delete a file" << NONE << std::endl;
	}
	selectChannel(channelName)->sendToOne(mapUser.find(fd)->second, rep);
}

void server::trfHelp(int fd, std::string channelName)
{
	std::string msg;
	std::string CLIENT = ":" + mapUser.find(fd)->second.getNickname() + "!" + mapUser.find(fd)->second.getUsername() +  "@localhost ";
	msg = CLIENT + "372 : <!trf send> <recipient> <path source>\n";
	send(fd, msg.c_str(), msg.size(), 0);
	msg = CLIENT + "372 : <!trf get> <sender> <serial number> [path destination]\n";
	send(fd, msg.c_str(), msg.size(), 0);
	msg = CLIENT + "372 : <!trf del> <serial number>\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

std::string server::extractFilename(std::string filename)
{
	std::istringstream iss(filename);
	std::string str;
	std::vector<std::string> vec;
	while (std::getline(iss, str, '/'))
	{
		str = deleteCRLF(str);
		vec.push_back(str);
	}
	return vec[vec.size() - 1];
}


