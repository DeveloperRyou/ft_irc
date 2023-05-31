#ifndef CHANNELMODE_HPP
# define CHANNELMODE_HPP

# include "ft_irc.hpp"

class ChannelMode
{
private:
	Channel *channel;
	unsigned int mode;
	std::map<char, void (ChannelMode::*)(char , std::string argv)> changer;

	void checkValidMode(std::vector<std::string> mode);
	void changeInviteMode(char sign, std::string none);
	void changeTopicMode(char sign, std::string none);
	void changeKeyMode(char sign, std::string password);
	void changeLimitMode(char sign, std::string limit);
	void changeOperMode(char sign, std::string nickname);

public:
	static const unsigned int INVITE = 0x01;
	static const unsigned int TOPIC = 0x02;
	static const unsigned int KEY = 0x04;
	static const unsigned int LIMIT = 0x08;

	ChannelMode(Channel *channel);

	void changeMode(std::vector<std::string> mode);
	std::string getMode(bool isJoin);
	bool isMode(const unsigned int mode);
};

#endif