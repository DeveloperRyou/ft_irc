#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"

class Channel;

class Client{
private:
	int	sock;
	struct sockaddr_in addr;

	std::string nickname;
	std::string username;
	std::string hostname;
	std::string servername;
	std::string realname;

	const std::string welcome = "Hello world!\n"; 
	
	bool authorization;
	bool is_set_user;
	bool is_set_nick;
	bool is_set_pass;

	void setAuthorization(bool auth);

public:
	Client(int server_socket);

	void send_to_Client(std::string msg);
	std::string recv_from_Client(void);

	int	getSock(void) const;
	std::string getPrefix(void) const;

	bool getAuthorization(void) const;
	void setIsSetUser(bool set_user);
	void setIsSetNick(bool set_nick);
	void setIsSetPass(bool set_pass);

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