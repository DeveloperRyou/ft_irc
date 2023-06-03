#include "ft_irc.hpp"

ChannelMode::ChannelMode(Channel *channel, std::string password)
{
	this->channel = channel;
	mode = 0;
	this->password = password;
	limit = 0;
	changer['i'] = &ChannelMode::changeInviteMode;
	changer['t'] = &ChannelMode::changeTopicMode;
	changer['k'] = &ChannelMode::changeKeyMode;
	changer['l'] = &ChannelMode::changeLimitMode;
	changer['o'] = &ChannelMode::changeOperMode;
}

bool ChannelMode::isMode(unsigned int mode)
{
	if (this->mode & mode)
		return true;
	return false;
}

void ChannelMode::checkValidMode(Client *client, std::vector<std::string> mode)
{
	std::string::iterator it = mode[0].begin();

	char	sign = '+';
	if (it != mode[0].end() && (*it == '+' || *it == '-'))
	{
		sign = *it;
		it++;
	}

	for (int cnt = 1; it != mode[0].end(); it++)
	{
		if (*it == 'l')
		{
			if (sign == '+' && mode.size() < ++cnt)
				throw IRCException(" 696 " + client->getNickname() + " " + channel->getName() 
					+ " l * :You must specify a parameter for the limit mode. Syntax: <limit>.");
		}
		else if (*it == 'k')
		{
			if (mode.size() < ++cnt)
				throw IRCException(" 696 " + client->getNickname() + " " + channel->getName() 
					+ " k * :You must specify a parameter for the key mode. Syntax: <key>.");
		}
		else if (*it == 'o')
		{
			if (mode.size() < ++cnt)
				throw IRCException(" 696 " + client->getNickname() + " " + channel->getName() 
					+ " o * :You must specify a parameter for the op mode. Syntax: <nick>.");
		}
		else if (*it != 'i' && *it != 't')
		{
			client->send_to_Client(Server::getPrefix() + " 472 " + client->getNickname() + " " + mode[0] 
			+ " :is not a recognised channel mode.");
		}
	}
}

void ChannelMode::changeInviteMode(char sign, std::string none)
{
	static_cast<void>(none);
	if (sign == '+')
		mode |= ChannelMode::INVITE;
	else if (sign == '-')
		mode |= ~ChannelMode::INVITE;
}

void ChannelMode::changeTopicMode(char sign, std::string none)
{
	static_cast<void>(none);
	if (sign == '+')
		mode |= ChannelMode::TOPIC;
	else if (sign == '-')
		mode |= ~ChannelMode::TOPIC;
}

//changeKeyMode에만 클라이언트 필요 -> 어떻게 해결?
void ChannelMode::changeKeyMode(char sign, std::string password)
{
	if (sign == '+')
	{
		if (isMode(ChannelMode::KEY))
			return ;
		mode |= ChannelMode::KEY;
		this->password = password;
	}
	else if (sign == '-')
	{
		if (!isMode(ChannelMode::KEY))
			return ;
		if (!isPassword(password))
		{
			client->send_to_Client(Server::getPrefix() + " 467 " + client->getName() + " " 
				+ channel->getName() + " :Channel key already set");
			return ;
		}
		mode |= ~ChannelMode::KEY;
		this->password = "";
	}
}

void ChannelMode::changeLimitMode(char sign, std::string limit)
{
	if (sign == '+')
	{
		mode |= ChannelMode::LIMIT;
		this->limit = stoi(limit);
	}
	else if (sign == '-')
	{
		if (isMode(ChannelMode::LIMIT))
			return ;
		mode |= ~ChannelMode::LIMIT;
		this->limit = 0;
	}
}

void ChannelMode::changeOperMode(char sign, std::string nickname)
{
	if (sign == '+')
		channel->changeOper(nickname, true);
	else if (sign == '-')
		channel->changeOper(nickname, false);
} 


//실제로 변경한 모드 string으로 저장해 리턴할까?
void ChannelMode::changeMode(Client *client, std::vector<std::string> mode)
{
	checkValidMode(client, mode);
	
	char sign = '+';
	std::string::iterator it = mode[0].begin();
	if (it != mode[0].end() && (*it == '+' || *it == '-'))
	{
		sign = *it;
		it++;
	}
	
	for (int idx = 1; it != mode[0].end(); it++)
	{
		std::string argv;
		if (*it == 'i' || *it == 't' || (sign == '-' && *it == 'l'))
			argv = "";
		else
			argv = mode[idx++];
		(this->*changer[*it])(sign, argv);
	}
}

std::string ChannelMode::getMode(bool isJoined)
{
	std::string str = "+";
	if (isMode(ChannelMode::INVITE))
		str += 'i';
	if (isMode(ChannelMode::TOPIC))
		str += 't';
	if (isMode(ChannelMode::KEY))
		str += 'k';
	if (isMode(ChannelMode::LIMIT))
		str += 'l';

	std::string pw = "<key>";
	if (isJoined)
		pw = password;
	if (isMode(ChannelMode::KEY | ~ChannelMode::LIMIT))
		return str + " :" + pw;
	if (isMode(ChannelMode::KEY))
		str += (" " + pw);
	if (isMode(ChannelMode::LIMIT)) {
		std::string limit_str = std::to_string(limit);
		str += " :" + limit_str;
	}
	return str;
}

bool ChannelMode::isPassword(const std::string &password)
{
	if (this->password == password)
		return true;
	return false;
}

bool ChannelMode::isJoinable(const int client_size)
{
	if (limit == 0 || client_size <= limit)
		return true;
	return false;
}
