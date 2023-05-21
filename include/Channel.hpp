#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

//#define MODE_I 0x01
//#define MODE_T 0x02
//#define MODE_K 0x04
//#define MODE_O 0x08
//#define MODE_L 0x10

class Client;
class ClientMode;
class ChannelMode;

class Channel
{
private:
	std::map<Client*, ClientMode*> client_map;
	ChannelMode	*mode;
	std::string	name;
	std::string ch_topic;
	std::string	password;

	void addClient(Client *client, std::string &password);
	void broadcast(Client *client, const std::string &msg);
public:
	Channel(Client *client, std::string name, std::string password);

	void subClient(Client *client, std::string &reason);

	void	invite(Client *oper, Client *invitee);
	void	part(Client* client);
	void	kick(Client *oper, Client *kicked, std::string &comments);
	void	topic(Client *client, std::string &topic);
	void	mode(Client *oper, std::string &mode);
	void	privmsg(Client *client, std::string &msg);

	std::string	getName(void) const;
	void	setName(std::string name);

	std::string getToic(void) const;
	int	getClientSize();
};

#endif