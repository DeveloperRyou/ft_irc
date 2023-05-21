#include "Channel.hpp"

Channel::Channel(Client *client, std::string name, std::string password)
{
	client_map[client] = new ClientMode(); //operator & join
	this->name = name;
	ch_topic = "";
	this->password = password;
}

void Channel::addClient(Client *client, std::string &password)
{
	if (!this->password.empty() && this->password != password)
		throw IRCException("incorrect pw");
	client_map[client] = new ClientMode(); //join
	broadcast(client, "[join the new user]");
}

void Channel::subClient(Client *client, std::string &reason)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
		if (it->first == client)
			break ;
	if (it == client_map.end())
		throw IRCException("not joind client");
	if (client_map.empty())
		throw IRCException("채널에 유저 없음");
}
void Channel::broadcast(Client *client, const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first == client)
			continue;
		it->first->send_to_Client(msg);
	}
}

void	Channel::invite(Client *oper, Client *invitee)
{
	if (client_map[client]->isOper())
		throw IRCException("is not operator");
	Client *invitee = server->getClient(nickname);
	if (invitee == NULL)
	{
		client->send_to_Client("일치하는 유저 없음");
		return ;
	}
	invitee->send_to_Client("invite this channel");
}

void Channel::kick(Client *client, std::string &username, std::string &comments)
{
	if (client != channel_operator)
	{
		client->send_to_Client("not channel operator");
		return;
	}
	std::vector<Client*>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if ((*it)->getUsername() == username)
			break ;
	}
	if (it == clients.end())
	{
		client->send_to_Client("일치하는 유저 없음");
		return ;
	}
	(*it)->send_to_Client("you are kicked: " + comments);
	broadcast(*it, "kick the user" + comments);
	clients.erase(it);
}

void Channel::change_topic(Client *client, std::string &topic)
{
	if (client != channel_operator)
	{
		client->send_to_Client("not channel operator");
		return;
	}
	this->topic = topic;
	broadcast(client, "topic changed");
}

void Channel::change_mode(Client *client, std::string &mode)
{
	if (client != channel_operator)
	{
		client->send_to_Client("not channel operator");
		return;
	}
	char op;
	std::string mode_str = "itkol";
	for (std::string::iterator it = mode.begin(); it != mode.end(); it++)
	{
		if (!((*it) == '+') || ((*it) == '-'))
			op = *it;
		else if (mode_str.find((*it)) == std::string::npos)
		{
			client->send_to_Client("unvalid mode");
			return;
		}
		if (op == '+')
		{
			if ((*it) == 'i')
				this->mode |= MODE_I;
			if ((*it) == 't')
				this->mode |= MODE_T;
			if ((*it) == 'k')
				this->mode |= MODE_K;
			if ((*it) == 'o')
				this->mode |= MODE_O;
			if ((*it) == 'l')
				this->mode |= MODE_L;
		}
		if (op == '-')
		{
			if ((*it) == 'i')
				this->mode &= ~MODE_I;
			if ((*it) == 't')
				this->mode &= ~MODE_T;
			if ((*it) == 'k')
				this->mode &= ~MODE_K;
			if ((*it) == 'o')
				this->mode &= ~MODE_O;
			if ((*it) == 'l')
				this->mode &= ~MODE_L;
		}
	}
}

std::string Channel::getName(void) const
{
	return name;
}

void Channel::setName(std::string name)
{
	this->name = name;
}

std::string Channel::getToic(void) const
{
	return ch_topic;
}

int	Channel::getClientSize()
{
	std::map<Client*, ClientMode*>::iterator it;

	int	cnt = 0;
	for(it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->second->isJoined())
			cnt++;
	}
	return cnt;
}