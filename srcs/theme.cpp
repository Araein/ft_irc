#include "irc.hpp"

void server::sendWelcomMsgs(int fd) const
{
	std::string msg;
	std::string start = startServer();
	msg = "001 " + mapUser.find(fd)->second.getNickname() + " :Welcome to 42 IRC!\n";
	msg += "002 RPL_YOURHOST :Your host is ircserv running version 0.1\n";
	msg += "003 RPL_CREATED :The server was created " + start + "\n";
	msg += "004 RPL_MYINFO :ircserv 0.1 level0 +itkol\n";
	msg += ":ircserv CAP * LS : \r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void server::printServerHeader(void) const
{
	std::cout << BLUE << BOLD << "________________________________________________________________" << NONE << std::endl;
	std::cout << RED << "                                                                " << NONE << std::endl;
	std::cout << RED << " TTTTTTTT  EEEEEE  AAAAA  MM    MM                              " << NONE << std::endl;
	std::cout << RED << "    TT    EE      AA   AA M M  M M                              " << NONE << std::endl;
	std::cout << RED << "    TT    EEEEE   AAAAAAA M  MM  M                              " << NONE << std::endl;
	std::cout << RED << "    TT    EE      A     A M      M                              " << NONE << std::endl;
	std::cout << YELLOW << "    TT     EEEEEE A     A M      M                              " << NONE << std::endl;
	std::cout << YELLOW << "                                                                " << NONE << std::endl;
	std::cout << YELLOW << "              CCCCCC  X    EEEEEE  SSSSSS TTTTTTTT              " << NONE << std::endl;
	std::cout << YELLOW << "             C         X  E       S         TT                  " << NONE << std::endl;
	std::cout << YELLOW << "             C            EEEEE    SSSSSS   TT                  " << NONE << std::endl;
	std::cout << GREEN << "             C            E              S  TT                  " << NONE << std::endl;
	std::cout << GREEN << "              CCCCCC       EEEEEE  SSSSSS   TT                  " << NONE << std::endl;
	std::cout << GREEN << "                                                                " << NONE << std::endl;
	std::cout << GREEN << "                       SSSSSS  U     U PPPPPP   EEEEEE RRRRR    " << NONE << std::endl;
	std::cout << GREEN << "                      S        U     U P     P E       R    R   " << NONE << std::endl;
	std::cout << PINK << "                       SSSSSS  U     U PPPPPP  EEEEE   RRRRR    " << NONE << std::endl;
	std::cout << PINK << "                             S U     U P       E       R   R    " << NONE << std::endl;
	std::cout << PINK << "                       SSSSSS   UUUUU  P        EEEEEE R    R   " << NONE << std::endl;
	std::cout << BLUE << BOLD << "________________________________________________________________" << NONE << std::endl;
	std::cout << BLUE << "                                          Create by : 'dcyprien'" << NONE << std::endl;
	std::cout << BLUE << "                                                      'sadorlin'" << NONE << std::endl;
	std::cout << BLUE << "                                                      'tlebouvi'" << NONE << std::endl;
}

void server::printHome(int fd) const
{
	std::string msg = "372 " + mapUser.find(fd)->second.getNickname() + " :";
	msg += "  \n";
	msg += " \n";
	msg += "****************************************************************\n";
	msg += "   **     **  *********    *********   *********      ********* \n";
	msg += "   **     **         **       **       **      **   **          \n";
	msg += "   **     **         **       **       **       ** **           \n";
	msg += "   *********  *********       **       **********  **           \n";
	msg += "          **  **              **       **    **    **           \n";
	msg += "          **  **              **       **      **   **          \n";
	msg += "          **  *********    *********   **       **    ********* \n";
	msg += "****************************************************************\n";
	msg += " \n";
	msg += printBonus();
	msg += " \n";
	msg += "AVAILABLE    COMMANDS\n";
	msg += "   -PASS:    <password>\n";
	msg += "   -NICK:    <nickname> \n";
	msg += "   -JOIN:    <channel1>,<channel2> [<key>],[<key>]\n";
	msg += "   -PART:    <channel> [ <Part Message> ] \n";
	msg += "   -QUIT:    [<Quit Message>]\n";
	msg += "   -KICK:    <channel> <user> [<comment>]\n";
	msg += "   -MODE:    <channel> <-/+ modes> <modeparams>\n";
	msg += "   -INVITE:  <nickname> <channel>\n";
	msg += "   -TOPIC:   <channel> [<topic>]\n";
	msg += "   -PRIVMSG: <!bot>\n";
	msg += "             <!bot help>\n";
	msg += "             <!bot time>\n";
	msg += "             <!bot trivia>\n";
	msg += "             <!bot coinflip>\n";
	msg += "             <!bot thanks>\n";
	msg += "             <!trf send> <recipient> <path source>\n";
	msg += "             <!trf get> <sender> <serial number> [path destination]\n";
	msg += "             <!trf del> <serial number>\n";
	msg += " \n";
	msg += printChannel();
	msg += " \n";
	send(fd, msg.c_str(), msg.size(), 0);
}

std::string server::printChannel(void) const
{
	int i = 0;
	std::string msg = "AVAILABLE CHANNELS\n";
	msg += "*Free access\n";
	for (std::vector<channel>::const_iterator it = channelList.begin(); it != channelList.end(); it++)
	{
		i++;
		if (i == 5)
		{
			msg += " \n";
			i = 0;
		}
		if (it->getMode('i') == false && it->getNeedPass() == false)
			msg += "  " + it->getChannelName();
	}
	msg += " \n";
	msg += "*With password and/or invitation\n";
	for (std::vector<channel>::const_iterator it = channelList.begin(); it != channelList.end(); it++)
	{
		if (it->getMode('i') == true || it->getNeedPass() == true)
			msg += "  " + it->getChannelName();
	}
	msg += " \n";
	return msg;
}

std::string server::printBonus(void) const
{
	std::string msg = "You can call";
	msg += GREEN;
	msg += BOLD;
	msg += "  MrRobot ";
	msg += NONE;
	msg += "(in channel send !bot)\n";
	msg += "\n";
	msg += "You can ";
	msg += GREEN;
	msg += BOLD;
	msg += "      Transfer File ";
	msg += NONE;
	msg += " (in channel send !trf)\n";
	return msg;
}
