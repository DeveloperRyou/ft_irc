#ifndef CHANNELINFO_HPP
# define CHANNELINFO_HPP

# include "ft_irc.hpp"
# define DEFAULT_LIMIT 10

class ChannelInfo
{
private:
	Client* oper;
	std::string	name;
	std::string topic;
	std::string password;
	int limit;
	int	client_size;
public:
	ChannelInfo(Client* client, std::string &name, std::string &password);
	~ChannelInfo();

	Client*	getOper(void) const;
	void	setOper(Client *oper);

	std::string	getName(void) const;
	void	setName(std::string &name);

	std::string getTopic(void) const;
	void	setTopic(std::string &topic);

	std::string getPassword(void) const;
	void	setPassword(std::string &password);
	bool	isPassword(const std::string &password) const;

	int	getLimit(void) const;
	void	setLimit(int limit);

	int	getClientSize(void) const;
	void	setClientSize(int client_size);
	bool	isFull(void) const;
};

#endif