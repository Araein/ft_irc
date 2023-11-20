#include "irc.hpp"


void server::sendWelcomMsgs(int fd) const
{
	std::string msg;
	std::string start = startServer();
	msg = "001 " + (mapUser.find(fd))->second.getNickname() + " :Welcome to 42 IRC!\n";
	msg += "002 RPL_YOURHOST :Your host is ircserv running version 0.1\n";
	msg += "003 RPL_CREATED :The server was created ";
	msg += start;
	msg += "\n";
	msg += "004 RPL_MYINFO :ircserv 0.1 level0\n";
	msg += "...............................................................\n";
	msg += "Connexion via 'nc' : <PASS> <PASSWORD>\n";
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

void server::printFullUser(int fd) const
{
	std::string message = "";
	message += "|-------------------------------------------------------------|\n";
	message += "|--------------------- 42_IRC is full ------------------------|\n";
	message += "|------------------ Please try again later -------------------|\n";
	message += "|-------------------------------------------------------------|\n";
	send(fd, message.c_str(), message.size(), 0);
}

void server::printInfo(int fd) const
{
	std::string msg;
	msg = "...............................................................\n";
	msg += "Connexion via 'nc' : <PASS> <PASSWORD>\n";
	msg += "...............................................................\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

void server::printHome(int fd)
{
	std::string msg = "";
	msg += "________________________________________________________________\n";
	msg += "                                                                \n";
	msg += "   **     **  *********    *********   *********      ********* \n";
	msg += "   **     **         **       **       **      **   **          \n";
	msg += "   **     **         **       **       **       ** **           \n";
	msg += "   *********  *********       **       **********  **           \n";
	msg += "          **  **              **       **    **    **           \n";
	msg += "          **  **              **       **      **   **          \n";
	msg += "          **  *********    *********   **       **    ********* \n";
	msg += "________________________________________________________________\n";
	msg += "                                                                \n";
	msg += "\n";
	msg += "*** AVAILABLE COMMANDS\n";
	msg += "    > Users\n";
	msg += "    - PASS:   Set password\n";
	msg += "    - NICK:   Set nickname\n";
	msg += "    - JOIN:   Join a channel\n";
	msg += "    - QUIT:   Disconnect from the server\n";
	msg += "\n";
	msg += "    > Operators\n";
	msg += "    - KICK:   Ejecte un client d'un canal\n";
	msg += "    - MODE:   Changer le mode d'un canal\n";
	msg += "    - INVITE: Inviter un client sur un channel a acces sur invitation\n";
	msg += "    - TOPIC:  Change le titre d'un canal\n";
	msg += "\n";
	send(fd, msg.c_str(), msg.size(), 0);
	listChannel(fd);
}

void server::listChannel(int fd)
{
	
	std::string msg = "";
	msg += "\n";
	msg += "               \n";
	msg += "*** AVAILABLE CHANNELS\n";
	msg += "    > Public\n";
	for (std::vector<channel>::iterator it = channelList.begin(); it != channelList.end(); it++)
	{
		if (it->getMode('i') == false)
		{
			msg += "      ";
			msg += it->getChannelName();
			msg += ":  ";
			msg += it->getTopicMessage();
			msg += "\n";
		}
	}
	msg += "    > Private\n";
	for (std::vector<channel>::iterator it = channelList.begin(); it != channelList.end(); it++)
	{
		if (it->getMode('i') == true)
		{
			msg += "      ";
			msg += it->getChannelName();
			msg += ":  ";
			msg += it->getTopicMessage();
			msg += "\n";
		}
	}
	msg += "               \n";
	msg += "Enjoy...\n";
	msg += "               \n";
	msg += "               \n";
	send(fd, msg.c_str(), msg.size(), 0);
}