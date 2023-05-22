#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "ft_irc.hpp"

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

	void addClient(Client *client, ClientMode *mode);
	void broadcast(std::string &msg);
	void broadcast(Client *client, std::string msg);
public:
	Channel(Client *client, std::string name, std::string spassword);

	void	invite(Client *oper, Client *invitee);
	void	join(Client *client, std::string &password);
	void	part(Client* client); //reason?
	void	kick(Client *oper, Client *kicked, std::string &comments);
	void	topic(Client *client, std::string &topic);
	void	mode(Client *oper, std::string &mode);
	void	privmsg(Client *client, std::string &msg);

	void subClient(Client *client);

	std::string	getName(void) const;
	void	setName(std::string name);

	std::string getToic(void) const;
	int	getClientSize();
};

#endif