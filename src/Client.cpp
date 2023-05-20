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

std::string Client::getPassword(void) const
{
	return password;
}

void Client::setPassword(std::string password)
{
	this->password = password;
}

bool Client::getAuthorization(void) const
{
	return authorization;
}

void Client::setauthorization(bool authorization)
{
	this->authorization = authorization;
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
