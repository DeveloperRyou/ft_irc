#include <vector>
#include <iostream>
#include <Client.hpp>

#define MODE_I 0x01
#define MODE_T 0x02
#define MODE_K 0x04
#define MODE_O 0x08
#define MODE_L 0x10

class Client;
class Server;

class Channel
{
private:
	Server	*server;
	std::string	name;
	std::string topic;
	unsigned int mode;
	std::string	password;
	std::vector<Client *> clients;
	Client *channel_operator;
public:
	Channel(Server* server, Client *client, std::string &name, std::string &password);
	void join_channel(Client *client, std::string &password);
	void leave_channel(Client *client, std::string &reason);

	//<channel> <user> [<comment>(==reason)]
	void kick(Client *client, std::string &username, std::string &comments);
	//<nickname> <channel>
	void invite(Client *client, std::string &nickname);
	//<channel> [<topic>]
	void change_topic(Client *client, std::string &topic);
	//<channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]
	void change_mode(Client *client, std::string &mode);

	std::string getName(void) const;
	void setName(std::string name);

	void broadcast(Client *client, const std::string &msg);
};