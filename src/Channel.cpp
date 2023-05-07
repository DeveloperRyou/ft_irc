#include "Channel.hpp"

Channel::Channel(std::string name, std::string password, Client *client)
{
	this->name = name;
	mode = 0;
	password = this->password;
	clients.push_back(client);
	channel_operator = client;
}

void Channel::join_channel(Client *client)
{
	clients.push_back(client);
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
		it.send_to_client("[join the new user]");
}

void Channel::leave_channel(Client *client)
{
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		it.send_to_client("[leave the user]");
		if (*it == client) //포인터 변수라 상관없지 않을까...
			clients.erase(it);
	}
}

bool Channel::authenticatePassword(std::string password)
{
	if (this->password == password)
		return true;
	return false;
}

void Channel::kick(std::string username, std::string comments)
{
	std::vector<Client*>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if (it.getUsername() == username)
			break ;
	}
	if (it == clients.end())
	{
		//해당 username 가진 client 없음
		return ;
	}
	it = clients.begin();
	it.send_to_client("you are kicked: " + comments);
	clients.erase(it);
	for (; it != clients.end(); it++)
		it.send_to_client("[kick the user]" + comments);
}

void Channel::invite(std::string nickname)
{
	if (mode )
}

void Channel::change_topic(std::string topic)
{

}

void Channel::change_mode(int mode)
{
	
}

std::string Channel::getName(void) const
{
	return name;
}

void Channel::setName(std::string name)
{
	this->name = name;
}
