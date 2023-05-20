#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Channel;

class Client{
private:
	int	sock;
	struct sockaddr_in addr;
	std::vector<Channel *> in_channel;
	
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

	int	getSock(void) const;
	
	bool setAuthorization(bool auth);

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

	Channel* getChannel(std::string ch_name);
	void	joinChannel(Channel *channel, std::string &password);
	void	leaveChannel(Channel *channel, std::string &reason);

	class ClientException: public std::runtime_error
	{
	public:
		ClientException(std::string err);
	};
};

#endif