#include <vector>
#include <iostream>

#define MODE_I 1
#define MODE_T (1 << 2)
#define MODE_K (1 << 3)
#define MODE_O (1 << 4)
#define MODE_L (1 << 5)

class Client;

class Channel
{
private:
	std::string	name;
	unsigned int mode;
	std::string	password;
	std::vector<Client *> clients;
	Client *channel_operator;
public:
	Channel(std::string name, std::string password, Client *client);
	void join_channel(Client *client);
	void leave_channel(Client *client);
	bool authenticatePassword(std::string password);

	//<channel> <user> [<comment>(==reason)]
	void kick(std::string username, std::string comments);
	//<nickname> <channel>
	void invite(std::string nickname);
	//<channel> [<topic>]
	void change_topic(std::string topic);
	//<channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]
	void change_mode(std::string mode);

	std::string getName(void) const;
	void setName(std::string name);

};