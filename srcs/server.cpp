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

	std::cout << "[SERVER: INITIALISATION]" << std::endl;
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
	std::cout << "[SERVER: CONNECTED]" << std::endl;
	if (bind(_fds[0].fd, (sockaddr *)&sock, sizeof(sock)) < 0)
	{
		std::cerr << "Failed to bind to socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	std::cout << "[SERVER: BINDING]" << std::endl;
	if (listen(_fds[0].fd, SOMAXCONN) < 0)
	{
		std::cerr << "Failed to listen to socket" << std::endl;
		close(_fds[0].fd);
		return false;
	}
	client us(_id, _fds[0].fd);
	mapUser.insert(std::make_pair(_fds[0].fd, us));
	_totalPlace++;
	std::cout << "[SERVER: LISTENING ON PORT " << _port << "]" << std::endl;
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

void server::createChannel(void)
{
	channel chan0("#Minishell");
	channelList.push_back(chan0);
	channel chan1("#SoLong");
	channelList.push_back(chan1);
	channel chan2("#PushSwap");
	channelList.push_back(chan2);
	channel chan3("#Inception");
	channelList.push_back(chan3);
}

void server::mainLoop(void)
{
	int ret;
	while (1)
	{
		ret = poll(_fds, maxFD + 1, 1000);
		if (ret < 0)
		{
			std::cerr << "Failed to call poll()" << std::endl;
			closeAll();
			return;
		}
		clearFDStemp();
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
	_curPlace = findPlace();
	_fds[_curPlace].fd = tmp.fd;
	_fds[_curPlace].events = tmp.events;
	_fds[_curPlace].revents = tmp.revents;
	if (_curPlace == maxFD)
		printFullUser(_fds[_curPlace].fd);
	else
	{
		_totalPlace++;
		client us(++_id, _fds[_curPlace].fd);
		mapUser.insert(std::make_pair(_fds[_curPlace].fd, us));
		sendWelcomMsgs(_fds[_curPlace].fd);
	}
}
