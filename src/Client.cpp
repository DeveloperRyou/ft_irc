#include "ft_irc.hpp"

Client::Client(int server_socket) : 
	nickname("*"), username("u"), hostname(""), servername(""), realname(""), welcome("Hello world!"), 
	authorization(false), is_set_user(false), is_set_nick(false), is_set_pass(false)
{
	socklen_t	addr_len = sizeof(addr);

	bzero(&addr, addr_len);
	sock = accept(server_socket, (struct sockaddr*)&addr, &addr_len);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);
}

void Client::send_to_Client(std::string msg)
{
	msg += "\n";
	const char *buf = msg.c_str();
	send(sock, buf, msg.length(), MSG_DONTWAIT);
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
			throw IRCException("Closed Connection");
		else if (len < 0)
			break;
		ret += buf;
	}
	while (true)
	{
		std::string::iterator lf = std::find(ret.begin(), ret.end(), 0x0a);
		if (lf == ret.end())
			break ;
		ret.erase(lf);
	}
	while (true)
	{
		std::string::iterator cr = std::find(ret.begin(), ret.end(), 0x0d);
		if (cr == ret.end())
			break ;
		ret.erase(cr);
	}
	//errno
	//errno
	//if (errno != EAGAIN)
	//	throw ClientException(strerror(errno)); //???
	return ret;
}

int	Client::getSock(void) const
{
	return sock;
}

std::string Client::getPrefix(void) const
{
	return ":" + nickname + "!~" + username + "@" + hostname;
}

bool Client::getAuthorization(void) const
{
	return authorization;
}

void Client::setAuthorization(bool auth)
{
	if (authorization == false && auth == true)
		send_to_Client(Server::getPrefix() + " 001 " + nickname + " :" + welcome);
	authorization = auth;
}

void Client::setIsSetUser(bool set_user)
{
	is_set_user = set_user;
	if (is_set_user && is_set_nick && is_set_pass)
		setAuthorization(true);
}

void Client::setIsSetNick(bool set_nick)
{
	is_set_nick = set_nick;
	if (is_set_user && is_set_nick && is_set_pass)
		setAuthorization(true);
}

void Client::setIsSetPass(bool set_pass)
{
	is_set_pass = set_pass;
	if (is_set_user && is_set_nick && is_set_pass)
		setAuthorization(true);
}

bool Client::getIsSetUser(void) const
{
	return is_set_user;
}

bool Client::getIsSetNick(void) const
{
	return is_set_nick;
}

bool Client::getIsSetPass(void) const
{
	return is_set_pass;
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
