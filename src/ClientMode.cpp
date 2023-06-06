#include "ft_irc.hpp"

ClientMode::ClientMode(const unsigned int type) : _type(type) {}

bool ClientMode::isInvited() const
{
	return (_type & INVITED);
}

bool ClientMode::isJoined() const
{
	return (_type & JOINED);
}

bool ClientMode::isOperate() const
{
	return (_type & OPERATE);
}

void ClientMode::setClientMode(const unsigned int type)
{
	_type = type;
}

void ClientMode::addClientMode(const unsigned int type)
{
	_type |= type;
}

void ClientMode::subClientMode(const unsigned int type)
{
	_type &= ~type;
}

unsigned int ClientMode::getClientMode() const
{
	return (_type);
}
