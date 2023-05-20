#include "Client.hpp"

Client::Client(int server_socket)
{
	socklen_t	addr_len = sizeof(addr);

	bzero(&addr, addr_len);
	sock = accept(server_socket, (struct sockaddr*)&addr, &addr_len);
	if (sock == -1)
		throw std::exception();
}

void Client::send_to_Client(std::string msg)
{
	const char *buf = msg.c_str();
	send(sock, buf, msg.length() + 1, MSG_DONTWAIT);
}

std::string Client::recv_from_Client(void)
{
	std::string ret;
	char buf[1024];
	ssize_t	len;

	bzero(buf, 1024);
	while (true)
	{
		len = recv(sock, buf, 1024, 0);
		if (len == 0)
			break;
		else if (len < 0)
			throw std::exception();
		ret += buf;
	}
	return ret;
}

int	Client::getSock(void) const
{
	return sock;
}

std::string Client::getNickname(void) const
{
	return nickname;
}

void Client::setNickname(std::string nickname)
{
	this->nickname = nickname;
}

std::string Client::getUsername(void) const
{
	return username;
}

void Client::setUsername(std::string username)
{
	this->username = username;
}

std::string Client::getHostname(void) const
{
	return hostname;
}

void Client::setHostname(std::string hostname)
{
	this->hostname = hostname;
}

std::string Client::getServername(void) const
{
	return servername;
}

void Client::setServername(std::string servername)
{
	this->servername = servername;
}

std::string Client::getRealname(void) const
{
	return realname;
}

void Client::setRealname(std::string realname)
{
	this->realname = realname;
}

Channel* Client::getChannel(std::string ch_name)
{
	std::vector<Channel*>::iterator it;

	for (it = in_channel.begin(); it != in_channel.end(); it++)
	{
		if ((*it)->getName() == ch_name)
			return (*it);
	}
	return (NULL);
}

void	Client::joinChannel(Channel *channel, std::string &password)
{
	bool success = channel->add_client(this, password);
	if (!success)
	{
		send_to_Client("[fail to join]");
		return;	
	}
	in_channel.push_back(channel);
}

void	Client::leaveChannel(std::string ch_name, std::string &reason)
{
	std::vector<Channel*>::iterator it;

	for (it = in_channel.begin(); it != in_channel.end(); it++)
	{
		if ((*it)->getName() == ch_name)
			break;
	}
	if (it == in_channel.end())
	{
		send_to_Client("일치하는 채널 없음");
		return;
	}
	(*it)->sub_client(this, reason);
	in_channel.erase(it);
}
