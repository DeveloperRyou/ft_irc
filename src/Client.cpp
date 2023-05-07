#include "Client.hpp"

Client::Client(int server_socket)
{
	socklen_t	addr_len = sizeof(addr);

	bzero(&addr, addr_len);
	sock = accept(server_socket, (struct sockaddr*)&addr, &addr_len);
	if (sock == -1)
		throw std::exception();
	in_channel = NULL;
}

void Client::send_to_Client(const char *msg)
{
	send(sock, msg, strlen(msg), MSG_DONTWAIT);
}

std::string Client::recv_from_Client(void)
{
	char *buf = new char[1024];
	recv(sock, buf, 1024, 0);
	return buf;
}

Channel *Client::getChannel(void) const
{
	return in_channel;
}

void Client::setChannel(Channel *in_channel)
{
	this->in_channel = in_channel;
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
