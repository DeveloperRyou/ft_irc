#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"
# define CLIENT_MAX 10
# define BUFFER_SIZE 10

class Channel;
class Client;
class Server
{
private:
	// values
	std::vector<Channel *> channels;
	std::vector<Client *> clients;
	struct pollfd poll_fds[CLIENT_MAX];
	int _server_socket;
	int _port;
	std::string _password;
	
	// methods
	Server();
	// parse
	void parse_message(std::string msg);
	// handle channel
	void create_channel();
	void delete_channel();
	// handle client
	void create_client();
	void delete_client();

public:
	Server(int port, std::string password);
	~Server();
	void open();
	void loop();

	// Exception
	class ServerException : public std::runtime_error {
	public:
		ServerException(std::string err);
	};
};

#endif