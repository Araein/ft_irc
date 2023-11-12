#pragma once

#include "irc.hpp"

class client
{

	int _id;
	int _level;
	std::string _password;
	std::string _nickname;
	std::string _username;
	std::string _identity; 

	std::map<std::string, std::string> _profil;

	void fillProfil();


	public:
		client(pollfd *fds);
		~client();



		void setLevel(int level);
		void setPassword(const std::string& password);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setIdentity(const std::string& identity);
	
		int getLevel();
		std::string getPassword();
		std::string getNickname();
		std::string getUsername();
		std::string getIdentity();

};