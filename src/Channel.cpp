#include "ft_irc.hpp"

Channel::Channel(Client *client, std::string name, std::string password = "")
{
	if (name.at(0) != '#')
		throw IRCException("476 " + client->getNickname() +" "+ name + " :Invalid channel name");
	this->name = name;
	client_map[client] = new ClientMode(ClientMode::OPERATE | ClientMode::JOINED);
	ch_mode = new ChannelMode(this, password);
	ch_topic = "";
	client_size = 1;
	client->send_to_Client(client->getPrefix() + "JOIN :" + name);
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
		throw IRCException("already exist");
	client_map[client] = mode;
}

void Channel::subClient(Client *client)
{
	std::map<Client*, ClientMode*>::iterator it = client_map.find(client);
	if (it == client_map.end())
		throw IRCException("cannot erase");
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
	if (ch_mode->isMode(ChannelMode::KEY) && ch_mode->isPassword(password))
		throw IRCException(" :Cannot join channel (incorrect channel key)");
	if (ch_mode->isMode(ChannelMode::LIMIT) && ch_mode->isJoinable(client_size))
		throw IRCException(":Cannot join channel (channel is full)");

	if (found)
		found->setClientMode(ClientMode::JOINED);
	else
		addClient(client, new ClientMode(ClientMode::JOINED));
	client_size++;
	broadcast(client->getPrefix() + "JOIN :" + name);
}

void	Channel::part(Client* client)
{
	std::map<Client*, ClientMode*>::iterator it;

	it = client_map.find(client);
	if (it == client_map.end() || it->second->isJoined() == false)
		throw IRCException("You're not on that channel");
	delete it->first;
	delete it->second;
	client_map.erase(it);
	client_size--;
	broadcast("PART :" + name);
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (client_map[oper]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	if (!client_map[kicked]->isJoined())
		throw IRCException(":They are not on that channel");
	
	std::map<Client*, ClientMode*>::iterator it = client_map.find(kicked);
	delete it->first;
	delete it->second;
	client_map.erase(it);
	client_size--;
	broadcast("KICK:" + comments);
}

void Channel::topic(Client *client, std::string &topic)
{
	if (topic.empty())
	{
		if (ch_topic.empty())
			client->send_to_Client(name + ":No topic is set.");
		else
			client->send_to_Client(name + ":" + ch_topic);
		return ;
	}
	if (ch_mode->isMode(ChannelMode::TOPIC) && !client_map[client]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	this->ch_topic = topic;
	broadcast("332: " + ch_topic);
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
		return ;
	}
	if (!client_map[client]->isOperate())
		throw IRCException("482" + client->getNickname() +" "+ name + ":You must have channel halfop access or above to set channel mode " + mode_vect[0]);
	ch_mode->changeMode(client, mode_vect);
}

void Channel::privmsg(Client *client, const std::string &msg)
{
	broadcast(client, ("PRIVMSG " + name + ": " + msg));
}

void	Channel::changeOper(std::string nickname, bool oper)
{
	ClientMode* found = findClient(nickname);
	if (!found || !found->isJoined())
		throw IRCException("???");
	if (oper)
		found->setClientMode(ClientMode::JOINED);
	else
		found->setClientMode(~ClientMode::JOINED);
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
	return client_size;
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