#include "ft_irc.hpp"

Channel::Channel(Client *client, std::string name)
{
	if (name.at(0) != '#')
	{
		client->send_to_Client(Server::getPrefix() + " 476 " + client->getNickname() +" "+ name + " :Invalid channel name");
		throw IRCException("Cannot create the Channel: Invalid channel name");
	}
	this->ch_info = new ChannelInfo(name);
	this->ch_mode = new ChannelMode(this, ch_info);
	
	addClient(client, ClientMode::OPERATE | ClientMode::JOINED);
	client->send_to_Client(client->getPrefix() + "JOIN :" + name);
}

Channel::~Channel()
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		delete it->second;
	}
	client_map.clear();
	delete ch_info;
	delete ch_mode;
}

void Channel::addClient(Client *client, const unsigned int mode)
{
	if (client_map[client])
		throw IRCException("Cannot add client to channel: already exist");
	client_map[client] = new ClientMode(mode);
	ch_info->setClientSize(ch_info->getClientSize() + 1);
}

void Channel::subClient(Client *client)
{
	std::map<Client*, ClientMode*>::iterator it = client_map.find(client);
	if (it == client_map.end())
		throw IRCException("Cannot subtract client from channel: doesn't exist");
	delete it->second;
	client_map.erase(it);
	ch_info->setClientSize(ch_info->getClientSize() - 1);
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
	std::cout<<"client size : "<<client_map.size()<<'\n';
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		printf("client : %p, clientmode : %p\n", it->first, it->second);
		if (it->first == client || !it->second->isJoined())
			continue;
		it->first->send_to_Client(msg);
	}
}

Client* Channel::getClient(std::string &nickname)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first->getNickname() == nickname)
			return it->first;
	}
	return (NULL);
}

void	Channel::invite(Client *oper, Client *invitee)
{
	if (client_map[invitee]->isJoined())
	{
		oper->send_to_Client(Server::getPrefix() + " 443 " + oper->getNickname() 
			+ " " + invitee->getNickname() + " " + ch_info->getName() + " :is already on channel");
		return ;
	}
	if (ch_mode->isMode(ChannelMode::INVITE) && !client_map[oper]->isOperate())
	{
		oper->send_to_Client(Server::getPrefix() + " 482 " + oper->getNickname()
			+ " " + ch_info->getName() + " :You must be a channel half-operator");
		return ;
	}
	addClient(invitee, ClientMode::INVITED);
	oper->send_to_Client(Server::getPrefix() + " 341 " + oper->getNickname() 
		+ " " + invitee->getNickname() + " :" + ch_info->getName());
	invitee->send_to_Client(oper->getPrefix() + " INVITE " + invitee->getNickname() + " :" + ch_info->getName());
}

void	Channel::join(Client *client, std::string &password)
{
	if (ch_mode->isMode(ChannelMode::LIMIT) && ch_info->isFull())
	{
		client->send_to_Client(Server::getPrefix() + " 471 " + client->getNickname() + " "
			+ ch_info->getName() + " :Cannot join channel (channel is full)");
		return ;
	}
	if (ch_mode->isMode(ChannelMode::KEY) && !ch_info->isPassword(password))
	{
		client->send_to_Client(Server::getPrefix() + " 475 " + client->getNickname() + " "
			+ ch_info->getName() + " :Cannot join channel (incorrect channel key)");
		return ;
	}
	if (client_map.find(client) != client_map.end())
	{
		ClientMode *found = client_map[client];
		if (found->isJoined())
			return ;
		if (ch_mode->isMode(ChannelMode::INVITE) && !found->isInvited())
		{
			client->send_to_Client(Server::getPrefix() + " 476 " + client->getNickname() + " "
				+ ch_info->getName() + " :Cannot join channel (invite only)");
			return ;
		}
		found->setClientMode(ClientMode::JOINED);
	}
	else
	{
		if (ch_mode->isMode(ChannelMode::INVITE))
		{
			client->send_to_Client(Server::getPrefix() + " 476 " + client->getNickname() + " "
				+ ch_info->getName() + " :Cannot join channel (invite only)");
			return ;
		}
		addClient(client, ClientMode::JOINED);
	}
	broadcast(client->getPrefix() + " JOIN :" + ch_info->getName());
	client->send_to_Client(Server::getPrefix() + " 355 " 
		+ client->getNickname() + " = " + ch_info->getName() + getClientNameList());
}

void	Channel::part(Client* client, std::string &reason)
{
	try
	{
		subClient(client);
		broadcast(client->getPrefix() + " PART " + ch_info->getName() + " :\"" + reason + "\"");
	}
	catch(const std::exception& e)
	{
		client->send_to_Client(Server::getPrefix() + " 442 " + client->getNickname() + " "
			+ ch_info->getName() + " :You're not on that channel");
		return ;
	}
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (!client_map[oper]->isOperate())
	{
		oper->send_to_Client(Server::getPrefix() + " 482 " + oper->getNickname() + " "
			+ ch_info->getName() + " :You must be a channel half-operator");
		return ;
	}
	try
	{
		if (!client_map[kicked]->isJoined())
			throw IRCException("Not on that channel");
		subClient(kicked);
		broadcast(oper->getPrefix() + " KICK " + ch_info->getName() + " " 
			+ kicked->getNickname() + " :" + comments);
	}
	catch(const std::exception& e)
	{
		oper->send_to_Client(Server::getPrefix() + " 441 " + oper->getNickname() + " "
			+ kicked->getNickname() + " " + ch_info->getName() + " :They are not on that channel");
	}
}

void Channel::topic(Client *client, std::string &topic)
{
	if (topic.empty())
	{
		std::string ch_topic = ch_info->getTopic();
		if (ch_topic.empty())
			client->send_to_Client(Server::getPrefix() + " 331 " + client->getNickname() + " "
			+ ch_info->getName() + " :No topic is set.");
		else
			client->send_to_Client(Server::getPrefix() + " 332 " + client->getNickname() + " "
			+ ch_info->getName() + " :" + ch_info->getTopic());
	}
	else if (ch_mode->isMode(ChannelMode::TOPIC) && !client_map[client]->isOperate())
	{
		client->send_to_Client(Server::getPrefix() + " 482 " + client->getNickname() + " "
			+ ch_info->getName() + " :You must be a channel half-operator");
	}
	else
	{
		ch_info->setTopic(topic);
		broadcast(client->getPrefix() + " TOPIC " + ch_info->getName() + " :" + ch_info->getTopic());
	}

}

void Channel::mode(Client *client, std::vector<std::string> mode_vect)
{
	if (mode_vect.empty())
	{
		std::map<Client*,ClientMode*>::iterator it = client_map.find(client);
		bool isJoined = true;
		if (it == client_map.end())
			isJoined = false;
		client->send_to_Client(Server::getPrefix() + " 324 " + client->getNickname()
			+ " " + ch_info->getName() + " " + ch_mode->getMode(isJoined));
	}
	else if (!client_map[client]->isOperate())
	{
		client->send_to_Client(Server::getPrefix() + " 482 " + client->getNickname() + " "
			+ ch_info->getName() + " :You must have channel halfop access or above to set channel mode "
			+ mode_vect[0]);
	}
	else
	{
		try
		{
			std::string changed_mode = ch_mode->changeMode(client, mode_vect);
			broadcast(client->getPrefix() + " MODE " + ch_info->getName() + " " + changed_mode);
		}
		catch(const std::exception& e)
		{
			client->send_to_Client(Server::getPrefix() + e.what());
		}
	}
}

void Channel::privmsg(Client *client, const std::string &msg)
{
	broadcast(client, (client->getPrefix() + " PRIVMSG " + ch_info->getName() + " :" + msg));
}

void	Channel::changeOperateClient(std::string &nickname, bool oper)
{
	Client* found = getClient(nickname);
	if (!found)
		return ;
	ClientMode* mode = client_map[found];
	if (!mode->isJoined())
		return ;
	if (oper == true&& mode->isOperate() == false)
		mode->addClientMode(ClientMode::OPERATE);
	else if (oper == false && mode->isOperate() == true)
		mode->subClientMode(ClientMode::OPERATE);
}

std::string Channel::getName(void) const
{
	return (ch_info->getName());
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

int Channel::getClientSize(void) const
{
	return (ch_info->getClientSize());
}