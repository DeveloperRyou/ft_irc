#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Client;
class ClientMode;
class ChannelMode;

class Channel
{
private:
	Server *server;
	std::map<Client*, ClientMode*> client_map;
	ChannelMode	*ch_mode;
	std::string	name;
	std::string ch_topic;
	int	client_size;

	void addClient(Client *client, ClientMode *mode);
	void broadcast(const std::string &msg);
	void broadcast(Client *client, const std::string &msg);
	ClientMode* findClient(std::string nickname);

public:
	Channel(Server *server, Client *client, std::string name, std::string spassword);
	~Channel();

	void	invite(Client *oper, Client *invitee);
	void	join(Client *client, std::string &password);
	void	part(Client* client);
	void	kick(Client *oper, Client *kicked, std::string &comments);
	void	topic(Client *client, std::string &topic);
	void	mode(Client *oper, std::vector<std::string>mode_vect);
	void	privmsg(Client *client, const std::string &msg);

	void	changeOper(std::string nickname, bool oper);
	void	subClient(Client *client);

	std::string	getName(void) const;
	void	setName(std::string name);

	std::string getToic(void) const;
	int	getClientSize();

	std::string getClientNameList(void) const;
};

#endif