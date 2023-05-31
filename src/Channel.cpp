#include "ft_irc.hpp"

Channel::Channel(Server *server, Client *client, std::string name, std::string password = "")
{
	if (name.at(0) != '#')
	{
		client->send_to_Client(server->getPrefix() + " 476 " + client->getNickname() +" "+ name + " :Invalid channel name");
		throw IRCException("Cannot create the Channel: Invalid channel name");
	}
	this->server = server;
	this->name = name;
	client_map[client] = new ClientMode(ClientMode::OPERATE | ClientMode::JOINED);
	ch_mode = new ChannelMode(this, password);
	ch_topic = "";
	client_size = 1;
	client->send_to_Client(client->getPrefix() + "JOIN :" + this->name);
}

Channel::~Channel()
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		delete it->first;
		delete it->second;
	}
	client_map.clear();
	delete ch_mode;
}

void Channel::addClient(Client *client, ClientMode *mode)
{
	if (client_map[client])
		throw IRCException("Cannot add client to channel: already exist");
	client_map[client] = mode;
}

void Channel::subClient(Client *client)
{
	std::map<Client*, ClientMode*>::iterator it = client_map.find(client);
	if (it == client_map.end())
		throw IRCException("Cannot subtract client from channel: doesn't exist");
	delete it->first;
	delete it->second;
	client_map.erase(it);
	client_size--;
}

void Channel::broadcast(const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->second->isJoined() == false)
			continue;
		it->first->send_to_Client(msg);
	}
}

void Channel::broadcast(Client *client, const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first == client || !it->second->isJoined())
			continue;
		it->first->send_to_Client(msg);
	}
}

ClientMode* Channel::findClient(std::string nickname)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first->getNickname() == nickname)
			return it->second;
	}
	return (NULL);
}

void	Channel::invite(Client *oper, Client *invitee)
{
	if (client_map[invitee]->isJoined())
	{
		oper->send_to_Client(server->getPrefix() + " 443 " + oper->getNickname() 
			+ " " + invitee->getNickname() + " " + this->name + " :is already on channel");
		return ;
	}
	if (ch_mode->isMode(ChannelMode::INVITE) && !client_map[oper]->isOperate())
	{
		oper->send_to_Client(server->getPrefix() + " 482 " + oper->getNickname()
			+ " " + this->name + " :You must be a channel half-operator");
		return ;
	}
	addClient(invitee, new ClientMode(ClientMode::INVITED));
	oper->send_to_Client(server->getPrefix() + " 341 " + oper->getNickname() 
		+ " " + invitee->getNickname() + " :" + this->name);
	invitee->send_to_Client(oper->getPrefix() + "INVITE");
}

void	Channel::join(Client *client, std::string &password)
{
	ClientMode *found = client_map[client];

	if (found->isJoined())
		return ;
	if (!(ch_mode->isMode(ChannelMode::INVITE) && found && found->isInvited()))
	{
		client->send_to_Client(server->getPrefix() + " 476 " + client->getNickname() + " "
			+ this->name + " :Cannot join channel (invite only)");
		return ;
	}
	if (ch_mode->isMode(ChannelMode::KEY) && ch_mode->isPassword(password))
	{
		client->send_to_Client(server->getPrefix() + " 475 " + client->getNickname() + " "
			+ this->name + " :Cannot join channel (incorrect channel key)");
		return ;
	}
	if (ch_mode->isMode(ChannelMode::LIMIT) && ch_mode->isJoinable(client_size))
	{
		client->send_to_Client(server->getPrefix() + " 471 " + client->getNickname() + " "
			+ this->name + " :Cannot join channel (channel is full)");
		return ;
	}

	if (found)
		found->setClientMode(ClientMode::JOINED);
	else
		addClient(client, new ClientMode(ClientMode::JOINED));
	client_size++;
	broadcast(client->getPrefix() + " JOIN :" + this->name);
	client->send_to_Client(server->getPrefix() + " 355 " 
		+ client->getNickname() + " = " + this->name + getClientNameList());
}

void	Channel::part(Client* client)
{
	std::map<Client*, ClientMode*>::iterator it;

	it = client_map.find(client);
	if (it == client_map.end() || it->second->isJoined() == false)
	{
		client->send_to_Client(server->getPrefix() + " 442 " + client->getNickname() + " "
			+ this->name + " :You're not on that channel");
		return ;
	}
	delete it->first;
	delete it->second;
	client_map.erase(it);
	client_size--;
	broadcast(client->getPrefix() + " PART :" + this->name);
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (client_map[oper]->isOperate())
	{
		oper->send_to_Client(server->getPrefix() + " 482 " + oper->getNickname() + " "
			+ this->name + " :You must be a channel half-operator");
		return ;
	}
	if (!client_map[kicked] || !client_map[kicked]->isJoined())
	{
		oper->send_to_Client(server->getPrefix() + " 441 " + oper->getNickname() + " "
			+ kicked->getNickname() + " " + this->name + " :They are not on that channel");
		return ;
	}
	
	std::map<Client*, ClientMode*>::iterator it = client_map.find(kicked);
	delete it->first;
	delete it->second;
	client_map.erase(it);
	client_size--;
	broadcast(oper->getPrefix() + " KICK " + this->name + " " 
		+ kicked->getNickname() + " :" + comments);
}

void Channel::topic(Client *client, std::string &topic)
{
	if (topic.empty())
	{
		if (ch_topic.empty())
			client->send_to_Client(server->getPrefix() + " 331 " + client->getNickname() + " "
			+ this->name + " :No topic is set.");
		else
			client->send_to_Client(server->getPrefix() + " 332 " + client->getNickname() + " "
			+ this->name + " :" + this->ch_topic);
		return ;
	}
	if (ch_mode->isMode(ChannelMode::TOPIC) && !client_map[client]->isOperate())
	{
		client->send_to_Client(server->getPrefix() + " 482 " + client->getNickname() + " "
			+ this->name + " :You must be a channel half-operator");
		return ;
	}
	this->ch_topic = topic;
	broadcast(client->getPrefix() + " TOPIC " + this->name + " :" + ch_topic);
}

void Channel::mode(Client *client, std::vector<std::string> mode_vect)
{
	if (mode_vect.empty())
	{
		std::map<Client*,ClientMode*>::iterator it = client_map.find(client);
		bool isJoined = true;
		if (it == client_map.end())
			isJoined = false;
		client->send_to_Client(server->getPrefix() + " 324 " + client->getNickname()
			+ " " + this->name + " " + ch_mode->getMode(isJoined));
		return ;
	}
	if (!client_map[client]->isOperate())
	{
		client->send_to_Client(server->getPrefix() + " 482 " + client->getNickname() + " "
			+ this->name + " :You must have channel halfop access or above to set channel mode "
			+ mode_vect[0]);
		return ;
	}
	ch_mode->changeMode(client, mode_vect);
	broadcast(client->getPrefix() + " MODE " + this->name + ch_mode->getMode(true));
}

void Channel::privmsg(Client *client, const std::string &msg)
{
	broadcast(client, (client->getPrefix() + " PRIVMSG " + this->name + ": " + msg));
}

void	Channel::changeOper(std::string nickname, bool oper)
{
	ClientMode* found = findClient(nickname);
	if (!found || !found->isJoined())
		return ;
	if (oper == true && found->isOperate() == false)
		found->setClientMode(ClientMode::OPERATE);
	else if (oper == false && found->isOperate() == true)
		found->setClientMode(~ClientMode::OPERATE);
}

std::string Channel::getName(void) const
{
	return this->name;
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
	return client_size;
}

std::string Channel::getClientNameList(void) const
{
	std::string ret = " :";
	// const라서 순회 안될지도
	for (std::map<Client*, ClientMode*>::const_iterator it = client_map.begin(); it != client_map.end(); it++)
	{
		if (!it->second->isJoined())
			continue ;
		if (!it->second->isOperate())
			ret += "@";
		ret += it->first->getNickname() + " ";
	}
	return ret;
}