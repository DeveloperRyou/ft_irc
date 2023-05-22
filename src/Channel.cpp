#include "Channel.hpp"

Channel::Channel(Client *client, std::string name, std::string password)
{
	client_map[client] = new ClientMode(ClientMode::OPERATE || ClientMode::JOINED); //operator & join
	mode = new ChannelMode();
	this->name = name;
	ch_topic = "";
	this->password = password;
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
	client_map.erase(it);
}

void Channel::broadcast(const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (!it->first->isJoined())
			continue;
		it->first->send_to_Client(msg);
	}
}

void Channel::broadcast(Client *client, const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first == client || !it->first->isJoined())
			continue;
		it->first->send_to_Client(msg);
	}
}

void	Channel::invite(Client *oper, Client *invitee)
{
	if (client_map[oper]->isOper())
		throw IRCException("is not operator");
	if (client_map[invitee]->isJoin())
		throw IRCException("is already joined");
	addClient(invitee, new ClientMode(ClientMode::INVITED));
	invitee->send_to_Client("invite this channel");
}

void	Channel::join(Client *client, std::string &password)
{
	ClientMode *found = client_map[client];
	//invite mode check
	if (!(mode->isInvited() && found && found->isInvited()))
		throw IRCException("invite mode && not invited");
	//already exist
	if (found->isJoined())
		throw IRCException("already existed");
	//password mode check
		//password check
	if (mode->isPassword() && this->password == password)
		throw IRCException("incorrect PW");

	if (found)
		found.setMode(JOIN);
	else
		addClient(client, new ClientMode(ClientMode::JOIN));
	broadcast(client, "[join the new user]");
}

void	Channel::part(Client* client)
{
	std::map<Client*, ClientMode*>::iterator it;

	it = client_map.find(client);
	if (it == client_map.end() || it->second->isJoined() == false)
		throw IRCException("you are not joind");
	client_map.erase(it);
	client->send_to_Client("part");
	broadcast(client, "part");
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (client_map[oper]->isOperate())
		throw IRCException("is not operator");
	if (!client_map[kicked]->isJoined())
		throw IRCException("not join client");
	client_map.erase(client_map.find(kicked));
	kicked->send_to_Client("you are kicked");
	broadcast(kicked, "kick user");
}

void Channel::topic(Client *client, std::string &topic)
{
	if (mode->isTopic() && !client_map[client]->isOperate())
		throw IRCException("need operator priviliged");
	this->ch_topic = topic;
	broadcast("topic changed");
}

//std::string mode_str = "itkol";
void Channel::mode(Client *client, std::string &mode_str)
{
	if (!client->isOperate())
		throw IRCException("need operator priviliged");
	
	unsigned int mode_value = mode->getMode();
	char	op = '+';
	std::string::iterator it = mode_str.begin();
	if (*it == '+' || *it == '-')
			op = *it;
	if (std::string::iterator it = mode_str.begin(); it != mode_str.end(); it++)
	{
		
		
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