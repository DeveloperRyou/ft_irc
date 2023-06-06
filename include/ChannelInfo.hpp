#ifndef CHANNELINFO_HPP
# define CHANNELINFO_HPP

# include "ft_irc.hpp"
# define DEFAULT_LIMIT 10

class ChannelInfo
{
private:
	std::string	name;
	std::string topic;
	std::string password;
	int limit;
	int	client_size;
	time_t	create_time;
public:
	ChannelInfo(std::string name);
	~ChannelInfo();

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

	std::string	getCreateTime(void) const;
};

#endif