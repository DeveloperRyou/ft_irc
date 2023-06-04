#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "ft_irc.hpp"

class Client;
class ClientMode;
class ChannelInfo;
class ChannelMode;

class Channel
{
private:
	ChannelInfo	*ch_info;
	ChannelMode	*ch_mode;
	std::map<Client*, ClientMode*> client_map;

	void addClient(Client *client, ClientMode *mode);
	Client* getClient(std::string &nickname);
	void broadcast(const std::string &msg);
	void broadcast(Client *client, const std::string &msg);

public:
	Channel(Client *client, std::string name, std::string password);
	~Channel();

	void 	subClient(Client *client);
	
	void	invite(Client *oper, Client *invitee);
	void	join(Client *client, std::string &password);
	void	part(Client* client, std::string &reason);
	void	kick(Client *oper, Client *kicked, std::string &comments);
	void	topic(Client *client, std::string &topic);
	void	mode(Client *oper, std::vector<std::string>mode_vect);
	void	privmsg(Client *client, const std::string &msg);

	void	changeOperateClient(std::string &nickname, bool oper);
	void	changeOper(std::string nickname, bool oper);

	std::string getName(void) const;
	std::string getClientNameList(void) const;
	int getClientSize(void) const;
};

#endif