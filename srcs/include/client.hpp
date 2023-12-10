#pragma once

#include "irc.hpp"

class client
{

	int  _fd;
	int  _id;
	int _log;
	int fileId;
	clock_t _netcat;
	std::string _nickname;
	std::string _username;
	std::string _password;
	std::vector<channel> channelConnected;
	std::vector<transferFile> fileList;

	public:
	client(int id, int fd);
	~client();

//**********************************/GETTER//**********************************/
	int getID() const;
	int getFD() const;
	int getLog() const;
	int getNetcat() const;
	int getHowManyFile() const;
	bool getIsChanOp();
	bool getNumFileExist(std::string id) const;
	size_t getHowManyChannel() const;
	std::string getPassword() const;
	std::string getUsername() const;
	std::string getNickname() const;
	int getFileDest(std::string id) const;
	std::vector<channel>::iterator getConnectBegin();
	std::vector<channel>::iterator getConnectEnd();
	std::vector<transferFile>::iterator getTrf(std::string id);


//**********************************/SETTER//**********************************/
	void setPassword(std::string pass);
	void setUsername(std::string username);
	void setNickname(std::string nickname);
	void setLog();
	void setNetcat(int value);
	void setNetcat();
	std::string setFileList(transferFile &trf);

//**********************************/FUNCTION//**********************************/
	void delFileList(std::string id);
	void addChannel(channel *chan);
	void deleteChannel(channel const &chan);
	void exitUser();


};