#include "Channel.hpp"

Channel::Channel(Server* server, Client *client, std::string &name, std::string &password)
{
	this->server = server;
	this->name = name;
	topic = "";
	mode = 0x00;
	password = this->password;
	clients.push_back(client);
	channel_operator = client;
}

void Channel::join_channel(Client *client, std::string &password)
{
	if (this->password != password)
	{
		client->send_to_Client("[fail to join]");
		return ;
	}
	clients.push_back(client);
	broadcast(client, "[join the new user]");
}

void Channel::leave_channel(Client *client, std::string &reason)
{
	std::vector<Client*>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if ((*it) == client)
			break;
	}
	if (it == clients.end())
	{
		client->send_to_Client("you're not in this channel");
		return;
	}
	broadcast((*it), "[leave the user]" + reason);
	clients.erase(it);
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

void Channel::invite(Client *client, std::string &nickname)
{
	if (client != channel_operator)
	{
		client->send_to_Client("not channel operator");
		return;
	}
	Client *invitee = server->getClient(nickname);
	if (invitee == NULL)
	{
		client->send_to_Client("일치하는 유저 없음");
		return ;
	}
	invitee->send_to_Client("invite this channel");
}

void Channel::change_topic(Client *client, std::string &topic)
{
	if (client != channel_operator)
	{
		client->send_to_Client("not channel operator");
		return;
	}
	this->topic == topic;
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

void Channel::broadcast(Client *client, const std::string &msg)
{
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it) != client)
			continue;
		(*it)->send_to_Client(msg);
	}
}