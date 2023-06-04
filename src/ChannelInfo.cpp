#include "ft_irc.hpp"

ChannelInfo::ChannelInfo(std::string name) : 
	name(name), password(""), topic(""), limit(DEFAULT_LIMIT), client_size(0)
{}

ChannelInfo::~ChannelInfo()
{}

std::string	ChannelInfo::getName(void) const
{
	return (name);
}

void	ChannelInfo::setName(std::string &name)
{
	this->name = name;
}

std::string	ChannelInfo::getTopic(void) const
{
	return (topic);
}

void	ChannelInfo::setTopic(std::string &topic)
{
	this->topic = topic;
}

std::string	ChannelInfo::getPassword(void) const
{
	return (password);
}

void	ChannelInfo::setPassword(std::string &password)
{
	this->password = password;
}

int	ChannelInfo::getLimit(void) const
{
	return (limit);
}

void	ChannelInfo::setLimit(int limit)
{
	this->limit = limit;
}

int	ChannelInfo::getClientSize(void) const
{
	return (client_size);
}

void	ChannelInfo::setClientSize(int client_size)
{
	this->client_size = client_size;
}

// TODO: Implement this function
bool	ChannelInfo::isPassword(const std::string &password) const
{
	if (this->password == password)
		return true;
	return false;
}

bool	ChannelInfo::isFull() const
{
	if (limit == 0 || client_size < limit)
		return false;
	return true;
}
