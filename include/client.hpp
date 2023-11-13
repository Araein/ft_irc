#pragma once

#include "irc.hpp"

class client
{
<<<<<<< HEAD
	int _id;
	int _level;
	std::string _nickname;
	std::string _password;
=======

	int _id;
	int _level;
	std::string _password;
	std::string _nickname;
	std::string _username;
	std::string _identity; 

>>>>>>> 1bf8c0a73195a7ebcdbc30524917c3eb388e1c59
	std::map<std::string, std::string> _profil;

	void fillProfil();


	public:
		client(pollfd *fds);
		~client();

<<<<<<< HEAD
		int getLevel();
		std::string getNickname();

		void setLevel(int level);

=======


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
>>>>>>> 1bf8c0a73195a7ebcdbc30524917c3eb388e1c59

};