#include "Channel.hpp"

Channel::Channel(Client *client, std::string name, std::string password = "")
{
	if (name.at(0) != '@')
		throw IRCException("476 " + client->getNickname() +" "+ name + " :Invalid channel name");
	this->name = name;
	client_map[client] = new ClientMode(ClientMode::OPERATE || ClientMode::JOINED); //operator & join
	ch_mode = new ChannelMode();
	ch_topic = "";
	this->password = password;
	join_cnt = 1;

	client->send_to_Client(client->getPrefix() + "JOIN :" + name);
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
	join_cnt--;
	if (join_cnt == 0)
		throw IRCException("delete this channel");
}

void Channel::broadcast(const std::string &msg)
{
	std::map<Client*, ClientMode*>::iterator it;
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (it->first->isJoined() == false)
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
	if (client_map[invitee]->isJoin())
		throw IRCException(":is already on channel");
	if (ch_mode->isInvited() && !client_map[oper]->isOper())
		throw IRCException(":You must be a channel half-operator");
	addClient(invitee, new ClientMode(ClientMode::INVITED));
	invitee->send_to_Client(oper->getPrefix() + "INVITE");
}

void	Channel::join(Client *client, std::string &password)
{
	ClientMode *found = client_map[client];
	//invite mode check
	if (!(ch_mode->isInvited() && found && found->isInvited()))
		throw IRCException(":Cannot join channel (invite only)");
	//already exist
	if (found->isJoined())
		throw IRCException("already existed: no reply");
	//password check
	if (ch_mode->isPassword() && this->password != password)
		throw IRCException(" :Cannot join channel (incorrect channel key)");
	//limit check
	if (ch_mode->isLimit() && join_cnt > limit)
		throw IRCException(":Cannot join channel (channel is full)");

	if (found)
		found.setMode(ClientMode::JOIN);
	else
		addClient(client, new ClientMode(ClientMode::JOIN));
	join_cnt++;
	broadcast("JOIN :" + name);
}

void	Channel::part(Client* client)
{
	std::map<Client*, ClientMode*>::iterator it;

	it = client_map.find(client);
	if (it == client_map.end() || it->second->isJoined() == false)
		throw IRCException("You're not on that channel");
	client_map.erase(it);
	join_cnt--;
	broadcast("PART :" + name);
}

void Channel::kick(Client *oper, Client *kicked, std::string &comments)
{
	if (client_map[oper]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	if (!client_map[kicked]->isJoined())
		throw IRCException(":They are not on that channel");
	client_map.erase(client_map.find(kicked));
	join_cnt--;
	broadcast("KICK:" + comments);
}

//수정 요함
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
	if (mode->isTopic() && !client_map[client]->isOperate())
		throw IRCException(":You must be a channel half-operator");
	this->ch_topic = topic;
	broadcast("332: " + ch_topic);
}

void Channel::mode(Client *client, std::vector<std::string> mode_str)
{
	if (mode_str.empty())
	{
		client->send_to_Client("324" + ch_mode.toString());
		return ;
	}
	if (!client->isOperate())
		throw IRCException("482" + client->getNickname() +" "+ name + ":You must have channel halfop access or above to set channel mode " + mode_str[0]);
	if (!ChannelMode::isValidMode(mode_str[0]))
		throw IRCException("is not valid mode");
	ChannelMode::changeMode(mode_str[0], ch_mode);
	
	std::string::iterator it = mode_str[0].begin();
	char op = '+';
	if (*it == '-')
		op = '-';
	for (int idx = 1; it != mode_str[0].end(); it++)
	{
		switch (*it)
		{
			case 'i':
			case 't':
				break;
			case 'l':
				if (op == '+')
				{
					try
					{
						limit = mode_str[idx++];
					}
					catch(const std::exception& e)
					{
						throw IRCException("인자 부족");
					}
					
				}
				else
					limit = 0;
				break;
			case 'k':
				if (op == '+')
				{
					if (!password.empty())
						throw IRCException("already password existed: no reply");
					try
					{
						password = mode_str[idx++];
					}
					catch(const std::exception& e)
					{
						throw IRCException(":You must specify a parameter for the key mode. Syntax: <key>.");
					}
					break;
				}
				else
				{
					try
					{
						if (password == mode_str[idx++])
						{
							password = "";
							client->send_to_Client("MODE" + name + "-k :" + password);
						}
						else
							client->send_to_Client("467 :Channel key already set")
					}
					catch(const std::exception& e)
					{
						throw IRCException("인자부족: no reply");
					}
					break;
				}
			case 'o':
			{
				try
				{
					Client *c = getClient(mode_str[idx++]);
					if (!c)
						throw IRCException("클라이언트 없음 :no _reply");
				}
				catch(const std::exception& e)
				{
					throw IRCException("인자 없음 -> 메시지 확인해볼 것");
				}
				if (op == '+')
				{
						
				}
				else
				{

				}
			}
			default:
				;
		}
	}
}

void Channel::privmsg(Client *client, const std::string &msg)
{
	broadcast(client, ("PRIVMSG " + name + ": " + msg));
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
	return join_cnt;
}

std::string Channel::getClientNameList(void) const
{
	std::map<Client*, ClientMode*>::iterator it;
	std::string ret = ":";
	for (it = client_map.begin(); it != client_map.end(); it++)
	{
		if (!it->second->isJoin())
			continue ;
		if (!it->second->isOperator())
			ret += "@";
		ret += it->first->getNickname() + " ";
	}
	return ret;
}