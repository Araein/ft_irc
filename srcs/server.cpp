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
server::~server(void) {}

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
	std::cout << "[42_IRC:  NEW CONNECTION]" << std::endl;
	_curPlace = findPlace();
	_fds[_curPlace].fd = tmp.fd;
	_fds[_curPlace].events = tmp.events;
	_fds[_curPlace].revents = tmp.revents;
	if (_curPlace == maxFD)
	{
		std::string msg = RED;
		msg += "Too many connections in progress. Please try again later";
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
		if (it->second.getNetcat() == 0)
		{
			it->second.setNetcat(-1);
			msg = "Please enter your password to connect to 42IRC\n";
			msg += "Use the command : PASS (or QUIT to quit the server)\n";
			send(it->second.getFD(), msg.c_str(), msg.size(), 0);
		}
	}
}

void server::createChannel(void)
{
	client admin(-1, -1);
	channel chan0("#Libft");
	chan0.setUserChanOp(admin);
	chan0.setTopic("Discute around the Libft and how to create your first library");
	channel chan1("#get_next_line");
	chan1.setUserChanOp(admin);
	chan1.setTopic("Discute around get_next_line and how to read a file");
	channel chan2("#BornToBeRoot");
	chan2.setUserChanOp(admin);
	chan2.setTopic("Discute around the virtual Machine and Linux");
	channel chan3("#soLong");
	chan3.setUserChanOp(admin);
	chan3.setTopic("Discute around the minilibx");
	channel chan4("#minishell");
	chan4.setUserChanOp(admin);
	chan4.setTopic("Discute around the Shell");
	channel chan5("#philosopher");
	chan5.setUserChanOp(admin);
	chan5.setTopic("Discute around the threads");
	channel chan6("#Evaluation");
	chan6.setUserChanOp(admin);
	chan6.setMaxConnectedUser(2);
	chan6.setTopic("Evaluate or get evaluated");
	chan6.setPassword("stud42");
	channel chan7("#Promo");
	chan7.setUserChanOp(admin);
	chan7.setTopic("Discuss with students from your promo");
	chan7.setPassword("stud42");
	channel chan8("#Staff");
	chan8.setUserChanOp(admin);
	chan8.setMaxConnectedUser(5);
	chan8.setTopic("Participate in the development of future student projects. On invitation by a staff member\n");
	chan8.setMode('i', true);
	chan8.setUserInvited(admin);
	chan8.setPassword("stud42");
	channel chan9("#API");
	chan9.setUserChanOp(admin);
	chan9.setTopic("Welcome to the discussion forum reserved for tutors. On invitation by a tutor or staff\n");
	chan9.setPassword("stud42");
	chan9.setMode('i', true);
	chan9.setUserInvited(admin);
	channelList.push_back(chan0);
	channelList.push_back(chan1);
	channelList.push_back(chan2);
	channelList.push_back(chan3);
	channelList.push_back(chan4);
	channelList.push_back(chan5);
	channelList.push_back(chan6);
	channelList.push_back(chan7);
	channelList.push_back(chan8);
	channelList.push_back(chan9);
}

