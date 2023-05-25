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
	unsigned int	*ch_mode;
	std::string	name;
	std::string ch_topic;
	std::string	password;
	int	client_size;
	int	limit = 0;

	void addClient(Client *client, ClientMode *mode);
	void broadcast(const std::string &msg);
	void broadcast(Client *client, const std::string &msg);

public:
	Channel(Client *client, std::string name, std::string spassword);

	void	invite(Client *oper, Client *invitee);
	void	join(Client *client, std::string &password);
	void	part(Client* client);
	void	kick(Client *oper, Client *kicked, std::string &comments);
	void	topic(Client *client, std::string &topic);
	void	mode(Client *oper, std::vector<std::string>mode_str);
	void	privmsg(Client *client, const std::string &msg);

	void subClient(Client *client);

	std::string	getName(void) const;
	void	setName(std::string name);

	std::string getToic(void) const;
	int	getClientSize();

	std::string getClientNameList(void) const;
};

#endif