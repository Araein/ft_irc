#pragma once

#include "irc.hpp"


class server 
{
	private:
		int _id;
		int _port;
		int _totalFD;
		int _curFD;
		int _pollResult;
		int _bytesRead;
		infoSocket _sock;
		pollfd _fds[maxFD];
		char _buffer[bufferSize];
		std::map<int, client> mapUser;
		std::vector<channel> chan;

		void setupPoll();
		bool initServerSocket();
		bool bindServerSocket();
		bool listenServerSocket();

		void accept_newUser();
		int findCurFD();
		void cleanFDS(int i);
		bool firstMsg(std::string message, int fd);
		bool selectCommand(std::string message, int i);
		void sendWelcomeMsgs(client user);
		// void sendMsgToClients(char *buffer, int n);
		// bool verify_Pwd(infoConnect user);
		// bool deleteUser();


	public:
		server(int port, std::string password);
		~server();

		std::string getUserName(int fd);
		int getUserLevel(int fd);
		
		void setUserLevel(int fd, int level);

		void stopServer(void);
		bool initServer();
		void mainloop();
};
