#include "ft_irc.hpp"

ChannelMode::ChannelMode(Channel *channel, ChannelInfo *ch_info)
{
	this->channel = channel;
	this->ch_info = ch_info;
	mode = 0;
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

	for (unsigned int cnt = 1; it != mode[0].end(); it++)
	{
		if (*it == 'l')
		{
			if (sign == '+')
			{
				if (mode.size() < ++cnt)
				{
					throw IRCException(" 696 " + client->getNickname() + " " + channel->getName() 
						+ " l * :You must specify a parameter for the limit mode. Syntax: <limit>.");
				}
				std::stringstream ss(mode[cnt]);
				int lim;
				ss >> lim;
				if (lim <= 0)
					throw IRCException(" 696 " + client->getNickname() + " " + channel->getName() + " l "
						+ mode[cnt] + " :Invalid limit mode parameter. Syntax: <limit>.");
			}
		}
		else if (*it == 'k')
		{
			if (sign == '-')
			{
				std::string password = mode[cnt];
				if (!ch_info->isPassword(password))
					throw IRCException(" 467 " + client->getNickname() + " " 
						+ channel->getName() + " :Channel key already set");
			}
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
		else if (*it == 't') { ; }
		else if (*it == 'i') { ; }
		else
		{
			throw IRCException(" 472 " + client->getNickname() + " " + mode[0] 
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
		mode &= ~ChannelMode::INVITE;
}

void ChannelMode::changeTopicMode(char sign, std::string none)
{
	static_cast<void>(none);
	if (sign == '+')
		mode |= ChannelMode::TOPIC;
	else if (sign == '-')
		mode &= ~ChannelMode::TOPIC;
}

void ChannelMode::changeKeyMode(char sign, std::string password)
{
	if (sign == '+')
	{
		if (isMode(ChannelMode::KEY))
			return ;
		mode |= ChannelMode::KEY;
		ch_info->setPassword(password);
	}
	else if (sign == '-')
	{
		if (!isMode(ChannelMode::KEY))
			return ;
		mode &= ~ChannelMode::KEY;
		std::string init = "";
		ch_info->setPassword(init);
	}
}

void ChannelMode::changeLimitMode(char sign, std::string limit)
{
	if (sign == '+')
	{
		mode |= ChannelMode::LIMIT;
		std::stringstream ss(limit);
		int lim;
		ss >> lim;
		ch_info->setLimit(lim);
	}
	else if (sign == '-')
	{
		if (isMode(ChannelMode::LIMIT))
			return ;
		mode &= ~ChannelMode::LIMIT;
		ch_info->setLimit(0);
	}
}

void ChannelMode::changeOperMode(char sign, std::string nickname)
{
	if (sign == '+')
	{
		channel->changeOperateClient(nickname, true);
	}
	else if (sign == '-')
	{
		channel->changeOperateClient(nickname, false);
	}
} 

std::string ChannelMode::changeMode(Client *client, std::vector<std::string> mode)
{
	checkValidMode(client, mode);
	
	char sign = '+';
	std::string::iterator it = mode[0].begin();
	if (it != mode[0].end() && (*it == '+' || *it == '-'))
	{
		sign = *it;
		it++;
	}
	
	std::string ret = "";
	ret += sign;
	for (int idx = 1; it != mode[0].end(); it++)
	{
		std::string argv;
		if (*it == 'i' || *it == 't' || (sign == '-' && *it == 'l'))
			argv = "";
		else
			argv = mode[idx++];
		(this->*changer[*it])(sign, argv);
		ret += (*it);
	}
	for (size_t idx = 1; idx < mode.size(); idx++)
	{
		ret += " ";
		ret += mode[idx];
	}

	return (ret);
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
		pw = ch_info->getPassword();
	if (isMode(ChannelMode::KEY | ~ChannelMode::LIMIT))
		return str + " :" + pw;
	if (isMode(ChannelMode::KEY))
		str += (" " + pw);
	if (isMode(ChannelMode::LIMIT)) 
	{
		std::stringstream ss;
		ss << ch_info->getLimit();
		std::string limit_str = ss.str();
		str += " :" + limit_str;
	}
	return str;
}
