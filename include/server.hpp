#pragma once

#include "irc.hpp"

class server 
{
	private:
		int _id;
		int _curFD;
		int _pollResult;
		int _bytesRead;
		char _buffer[bufferSize];
		std::vector<infoConnect> _vect;
		infoConnect _server;
		struct pollfd _fds[maxFD];

		void setupPoll();
		void initStruct(infoConnect *info);
		bool initServerSocket();
		bool bindServerSocket();
		bool listenServerSocket();

		void accept_newUser();
		void sendMsgToClients(char *buffer, int n);
		int findFdById(int id);
		// bool deleteUser();


	public:
		server(int port, std::string password);
		~server();

		std::string getName(int fd);

		void stopServer(void);
		bool initServer();
		void mainloop();
};
