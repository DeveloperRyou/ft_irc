#include "ft_irc.hpp"

ChannelMode::ChannelMode(Channel *channel)
{
	this->channel = channel;
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

void ChannelMode::checkValidMode(std::vector<std::string> mode)
{
	std::string::iterator it = mode[0].begin();

	char	sign = '+';
	if (it != mode[0].end() && (*it == '+' || *it == '-'))
	{
		sign = *it;
		it++;
	}

	int cnt = 0;
	for (; it != mode[0].end(); it++)
	{
		switch (*it)
		{
		case 'i':
		case 't':
			break;
		case 'l':
			if (sign == '+')
				cnt++;
		case 'k':
		case 'o':
			cnt++;
			break;
		default:
			IRCException(mode[0] + ":is not a recognised channel mode.");
		}
	}

	if (cnt >= static_cast<int>(mode.size()))
		IRCException(":You must specify a parameter for the mode.");
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
	if (password.empty())
		throw IRCException("already~");
	if (sign == '+')
	{
		if (isMode(ChannelMode::KEY))
			throw IRCException("already~");
		mode |= ChannelMode::KEY;
		channel->ch_info->setPassword(password);
	}
	else if (sign == '-')
	{
		if (isMode(ChannelMode::KEY))
			throw IRCException("???");
		if (!channel->ch_info->isPassword(password))
			throw IRCException("???");
		mode &= ~ChannelMode::KEY;
		std::string init = "";
		channel->ch_info->setPassword(init);
	}
}

void ChannelMode::changeLimitMode(char sign, std::string limit)
{
	if (sign == '+')
	{
		mode |= ChannelMode::LIMIT;
		//limit이 digit인지 혹은 0이하의 수인지 검사
		int lim = stoi(limit);
		if (lim <= 0)
			throw IRCException("limit is 0 or less than 0");
		channel->ch_info->setLimit(lim);
	}
	else if (sign == '-')
	{
		if (isMode(ChannelMode::LIMIT))
			throw IRCException("???");
		mode &= ~ChannelMode::LIMIT;
		channel->ch_info->setLimit(0);
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

void ChannelMode::changeMode(std::vector<std::string> mode)
{
	checkValidMode(mode);
	
	char sign = '+';
	std::string::iterator it = mode[0].begin();
	if (it != mode[0].end() && (*it == '+' || *it == '-'))
	{
		sign = *it;
		it++;
	}
	
	int idx = 1;
	for (; it != mode[0].end(); it++)
	{
		std::string argv;
		if (*it == 'i' || *it == 't' || (sign == '-' && *it == 'l'))
			argv = "";
		else
			argv = mode[idx++];
		(this->*changer[*it])(sign, argv);
	}
}

// Channel에서 password 및 limit 붙여서 리턴해줄 것
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
		pw = channel->ch_info->getPassword();
	if (isMode(ChannelMode::KEY | ~ChannelMode::LIMIT))
		return str + " :" + pw;
	if (isMode(ChannelMode::KEY))
		str += (" " + pw);
	if (isMode(ChannelMode::LIMIT)) {
		std::string limit_str = std::to_string(channel->ch_info->getLimit());
		str += " :" + limit_str;
	}
	return str;
}
