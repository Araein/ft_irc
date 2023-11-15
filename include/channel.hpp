#pragma once

#include "irc.hpp"

class channel
{
	private:
	std::string _pwd;
	std::vector<client> _admins;
	

	public:
	channel();
	~channel();
};
