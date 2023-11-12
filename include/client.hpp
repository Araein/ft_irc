#pragma once

#include "irc.hpp"

class client
{



	std::map<std::string, std::string> _profil;

	void fillProfil();


	public:
		client(pollfd *fds);
		~client();

		int getLevel();
		std::string getNickname();

		void setLevel(int level);

	/*a remettre en prive plus tard?*/
		int _id;
		int _level;
		std::string _password;
		std::string _nickname;
		std::string _username;
		std::string _identity; //(utile?)
};