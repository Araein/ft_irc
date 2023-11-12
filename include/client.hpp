#pragma once

#include "irc.hpp"

class client
{
	int _id;
	int _level;
	std::string password;
	std::map<std::string, std::string> profil;

	void fillProfil();


	public:
		client(infoConnect *user);
		~client();

		int getLevel();

		void setLevel(int level);


};