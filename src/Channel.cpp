#include "ft_irc.hpp"

Channel::Channel(Client *client, std::string name, std::string password = "")
{
	if (name.at(0) != '#')
		throw IRCException("476 " + client->getNickname() +" "+ name + " :Invalid channel name");
	this->ch_info = new ChannelInfo(client, name, password);
	client_map[client] = new ClientMode(ClientMode::OPERATE | ClientMode::JOINED);
	ch_mode = new ChannelMode(this);
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

void Channel::addClient(Client *client, ClientMode *mode)
{
	if (client_map[client])
		throw IRCException("already exist");
	client_map[client] = mode;
	ch_info->setClientSize(ch_info->getClientSize() + 1);
}

void Channel::subClient(Client *client)
{
	std::map<Client*, ClientMode*>::iterator it = client_map.find(client);
	if (it == client_map.end())
		throw IRCException("cannot erase");
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
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
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
		throw IRCException(":is already on channel");
	if (ch_mode->isMode(ChannelMode::INVITE) && !client_map[oper]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	addClient(invitee, new ClientMode(ClientMode::INVITED));
	invitee->send_to_Client(oper->getPrefix() + "INVITE");
}

void	Channel::join(Client *client, std::string &password)
{
	ClientMode *found = client_map[client];

	if (!(ch_mode->isMode(ChannelMode::INVITE) && found && found->isInvited()))
		throw IRCException(":Cannot join channel (invite only)");
	if (found->isJoined())
		throw IRCException("already existed: no reply");
	if (ch_mode->isMode(ChannelMode::KEY) && !ch_info->isPassword(password))
		throw IRCException(" :Cannot join channel (incorrect channel key)");
	if (ch_mode->isMode(ChannelMode::LIMIT) && ch_info->isFull())
		throw IRCException(":Cannot join channel (channel is full)");

	if (found)
		found->setClientMode(ClientMode::JOINED);
	else
		addClient(client, new ClientMode(ClientMode::JOINED));
	broadcast(client->getPrefix() + "JOIN :" + client->getNickname());
}

void	Channel::part(Client* client)
{
	try
	{
		subClient(client);
		broadcast("PART :" + client->getNickname());
	}
	catch(const std::exception& e)
	{
		throw IRCException("You're not on that channel");
	}
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (!client_map[oper]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	if (!client_map[kicked]->isJoined())
		throw IRCException(":They are not on that channel");
	try
	{
		subClient(kicked);
		broadcast("KICK:" + comments);
	}
	catch(const std::exception& e)
	{
		throw IRCException(":They are not on that channel");
	}
}

void Channel::topic(Client *client, std::string &topic)
{
	if (topic.empty())
	{
		std::string ch_topic = ch_info->getTopic();
		if (ch_topic.empty())
			client->send_to_Client(ch_info->getName() + ":No topic is set.");
		else
			client->send_to_Client(ch_info->getName() + ":" + ch_topic);
	}
	else
	{
		if (ch_mode->isMode(ChannelMode::TOPIC) && !client_map[client]->isOperate())
			throw IRCException(":You must be a channel half-operator");
		ch_info->setTopic(topic);
		broadcast("332: " + topic);
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
		client->send_to_Client("324" + ch_mode->getMode(isJoined));
	}
	else
	{
		if (!client_map[client]->isOperate())
			throw IRCException("482" + client->getNickname() +" "+ ch_info->getName() + ":You must have channel halfop access or above to set channel mode " + mode_vect[0]);
		ch_mode->changeMode(mode_vect);\
	}
}

void Channel::privmsg(Client *client, const std::string &msg)
{
	broadcast(client, ("PRIVMSG " + ch_info->getName() + ": " + msg));
}

void	Channel::changeOperateClient(std::string &nickname, bool oper)
{
	Client* client = getClient(nickname);
	if (!client)
		throw IRCException("No such client");
	ClientMode* mode = client_map[client];
	if (!mode->isJoined())
		throw IRCException("Client is not joined");
	if (oper)
		mode->addClientMode(ClientMode::OPERATE);
	else
		mode->subClientMode(ClientMode::OPERATE);
}

std::string Channel::getClientNameList(void) const
{
	std::string ret = ":";
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