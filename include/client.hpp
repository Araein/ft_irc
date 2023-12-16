#pragma once

#include "irc.hpp"

class client
{

	std::string rtrim(const std::string& str);
	std::string ltrim(const std::string& str);
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
	bool getIsChanOp() const;
	bool getNumFileExist(std::string id) const;
	size_t getHowManyChannel() const;
	std::string getPassword() const;
	std::string getUsername() const;
	std::string getNickname() const;
	int getFileDest(std::string id) const;
	std::vector<channel>::iterator getConnectBegin();
	std::vector<channel>::iterator getConnectEnd();
	std::vector<transferFile>::const_iterator getTrf(std::string const &id) const;


//**********************************/SETTER//**********************************/
	void setPassword(std::string const &pass);
	void setUsername(std::string const &username);
	void setNickname(std::string const &nickname);
	void setLog();
	void setNetcat(int value);
	void setNetcat();
	std::string setFileList(transferFile &trf);

//**********************************/FUNCTION//**********************************/
	void delFileList(std::string const &id);
	void addChannel(channel const &chan);
	void deleteChannel(channel const &chan);
	void exitUser();

};