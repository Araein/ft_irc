#include "irc.hpp"

server::server(int port, std::string password):
_id(100), _port(port), _curPlace(0), _totalPlace(0), _password(password)
{
	printServerHeader();
	createChannel();
	for (int i = 0; i < maxFD + 1; i++)
	{
		_fds[i].fd = -1;
		_fds[i].events = POLLIN | POLLERR;
		_fds[i].revents = 0;
	}
	_fds[maxFD].fd = -2;
}
server::~server(void)
{
	delete admin;
	delete MrRobot;
	delete [] chan;
}

//**********************************//SOCKET//**********************************//

bool server::initSocket(void)
{
	int opt = 1;
	sockaddr_in sock;
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port = htons(_port);

	std::cout << "[42_IRC:  INITIALISATION]" << std::endl;
	_fds[0].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_fds[0].fd < 0)
	{
		std::cerr << "Failed to open socket" << std::endl;
		return false;
	}
	if (setsockopt(_fds[0].fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))){
		std::cerr << "Failed to config socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	std::cout << "[42_IRC:  CONNECTED]" << std::endl;
	if (bind(_fds[0].fd, (sockaddr *)&sock, sizeof(sock)) < 0)
	{
		std::cerr << "Failed to bind to socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	std::cout << "[42_IRC:  BINDING]" << std::endl;
	if (listen(_fds[0].fd, SOMAXCONN) < 0)
	{
		std::cerr << "Failed to listen to socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	client us(_id, _fds[0].fd);
	us.setNetcat(-1);
	mapUser.insert(std::make_pair(_fds[0].fd, us));
	_totalPlace++;
	std::cout << "[42_IRC:  LISTENING ON PORT " << _port << "]" << std::endl;
	return true;
}

int server::findChanbyName(std::string chan) const{
	int i = 0;
	for (std::vector<channel>::const_iterator it = channelList.begin(); it != channelList.end(); it++)
	{
		if (chan == it->getChannelName())
			return i;
		i++;
	}
	return -1;
}

void server::mainLoop(void)
{
	int ret;
	while (1)
	{
		ret = poll(_fds, maxFD + 1, 1000);
		if (ret < 0)
		{
			std::cerr << "[42_IRC:  FAILED POLL() CALL]" << std::endl;
			closeAll();
			return;
		}
		closeOne(_fds[maxFD].fd);
		userNetcat();
		acceptNewUser();
		if (ret > 0)
			receivMessage();
	}
}

void server::acceptNewUser(void)
{
	sockaddr_in sock;
	socklen_t sizeSock = sizeof(sock);
	memset(&sock, 0, sizeSock);
	pollfd tmp;
	tmp.revents = 0;
	tmp.events = POLLIN | POLLERR;
	tmp.fd = accept(_fds[0].fd, (sockaddr *)&sock, &sizeSock);
	if (tmp.fd == -1)
		return;
	std::cout << "[42_IRC:  NEW CONNECTION REQUEST]" << std::endl;
	_curPlace = findPlace();
	_fds[_curPlace].fd = tmp.fd;
	_fds[_curPlace].events = tmp.events;
	_fds[_curPlace].revents = tmp.revents;
	if (_curPlace == maxFD)
	{
		std::string msg = "ircserv:";
		msg += RED;
		msg += " Too many connections in progress. Please try again later";
		msg += NONE;
		msg += "\r\n";
		send(tmp.fd, msg.c_str(), msg.size(), 0);
		closeOne(_fds[_curPlace].fd);
		return;
	}
	client user( ++_id, _fds[_curPlace].fd);
	mapUser.insert(std::make_pair(_fds[_curPlace].fd, user));
	_totalPlace++;
}

void server::userNetcat(void)
{
	for (std::map<int, client>::iterator it = mapUser.begin(); it != mapUser.end(); it++)
	{
		std::string msg;
		if (it->second.getLog() == 0)
			it->second.setNetcat();
		if (it->second.getNetcat() == 2 && it->second.getLog() == 0)
		{
			it->second.setNetcat(-1);
			msg = "Please enter your password to connect to 42IRC\n";
			msg += "Use the command : PASS (or QUIT to quit the server)\n";
			send(it->second.getFD(), msg.c_str(), msg.size(), 0);
		}
	}
	client user( ++_id, _fds[_curPlace].fd);
	mapUser.insert(std::make_pair(_fds[_curPlace].fd, user));
	_totalPlace++;
}


//**********************************//GESTION DE NETCAT//**********************************//


//**********************************//CANAUX DE BASE//**********************************//

void server::createChannel(void)
{
	admin = new client(0, 0);


	client user( -1, -1);
	MrRobot = new client( -1, -1);

	user.setNickname("MrRobot");
	user.setUsername("MrRobot");
	mapUser.insert(std::make_pair(-1, user));
	MrRobot->setNickname("MrRobot");
	MrRobot->setUsername("MrRobot");


	chan = new channel[10];
	chan[0].setChannelName("#Libft");
	chan[0].setUserChanOp(admin);
	chan[0].setTopic("Discuss around the Libft and how to create your first library");
	chan[1].setChannelName("#get_next_line");
	chan[1].setUserChanOp(admin);
	chan[1].setTopic("Discute around get_next_line and how to read a file");
	chan[2].setChannelName("#BornToBeRoot");
	chan[2].setUserChanOp(admin);
	chan[2].setTopic("Discuss around the virtual Machine and Linux");
	chan[3].setChannelName("#soLong");
	chan[3].setUserChanOp(admin);
	chan[3].setTopic("Discuss around the minilibx");
	chan[4].setChannelName("#minishell");
	chan[4].setUserChanOp(admin);
	chan[4].setTopic("Discuss around the Shell");
	chan[5].setChannelName("#philosopher");
	chan[5].setUserChanOp(admin);
	chan[5].setTopic("Discuss around the threads");
	chan[6].setChannelName("#Evaluation");
	chan[6].setUserChanOp(admin);
	chan[6].setMaxConnectedUser(2);
	chan[6].setTopic("Evaluate or get evaluated");
	chan[6].setPassword("stud42");
	chan[7].setChannelName("#Promo");
	chan[7].setUserChanOp(admin);
	chan[7].setNeedPass(true);
	chan[7].setTopic("Discuss with students from your promo");
	chan[7].setPassword("stud42");
	chan[8].setChannelName("#Staff");
	chan[8].setUserChanOp(admin);
	chan[8].setMaxConnectedUser(5);
	chan[8].setTopic("Participate in the development of future student projects. On invitation by a staff member\n");
	chan[8].setMode('i', true);
	chan[8].setUserInvited(admin);
	chan[8].setPassword("stud42");
	chan[9].setChannelName("#API");
	chan[9].setUserChanOp(admin);
	chan[9].setTopic("Welcome to the discussion forum reserved for tutors. On invitation by a tutor or staff\n");
	chan[9].setPassword("stud42");
	chan[9].setMode('i', true);
	chan[9].setUserInvited(admin);
	channelList.push_back(chan[0]);
	channelList.push_back(chan[1]);
	channelList.push_back(chan[2]);
	channelList.push_back(chan[3]);
	channelList.push_back(chan[4]);
	channelList.push_back(chan[5]);
	channelList.push_back(chan[6]);
	channelList.push_back(chan[7]);
	channelList.push_back(chan[8]);
	channelList.push_back(chan[9]);
}

