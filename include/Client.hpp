#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Channel;

class Client{
private:
	int	sock;
	struct sockaddr_in addr;
	std::vector<Channel *> in_channel;

	std::string password;
	bool authorization;
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string servername;
	std::string realname;
public:
	Client(int server_socket);

	void send_to_Client(std::string msg);
	std::string recv_from_Client(void);

	void join_channel(Channel *channel);
	void leave_channel();

	int	getSock(void) const;

	std::string getPassword(void) const;
	void setPassword(std::string password);

	bool getAuthorization(void) const;
	void setauthorization(bool authorization);

	std::string getNickname(void) const;
	void setNickname(std::string nickname);

	std::string getUsername(void) const;
	void setUsername(std::string username);

	std::string getHostname(void) const;
	void setHostname(std::string hostname);

	std::string getServername(void) const;
	void setServername(std::string servername);

	std::string getRealname(void) const;
	void setRealname(std::string realname);
};

#endif