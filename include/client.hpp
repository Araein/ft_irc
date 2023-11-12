#pragma once

#include "irc.hpp"

class client
{
	int _id;
	int _level;
	std::string _nickname;
	std::string _password;
	std::map<std::string, std::string> _profil;

	void fillProfil();


	public:
		client(pollfd *fds);
		~client();

		int getLevel();
		std::string getNickname();

		void setLevel(int level);


};