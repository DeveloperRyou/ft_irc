#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

# define CLIENT_MAX 10
# define CHANNEL_MAX 2
# define BUFFER_SIZE 10

class Client;
class Channel;
class Parser;

class Server
{
private:
	// values
	std::vector<Channel *> channels;
	std::vector<Client *> clients;
	Parser* parser;
	struct pollfd poll_fds[CLIENT_MAX + 2];
	int _server_socket;
	int _port;
	std::string _password;

	void read_client();
public:
	Server(int port, std::string password);
	~Server();
	void open();
	void loop();
	bool checkPassword(std::string &password);
	Client* getClient(std::string &client_name);
	Channel* getChannel(std::string &channel_name);

	// handle client
	Client* create_client();
	void delete_client(int index);
	void delete_client(Client *cli);
	// handle channel
	Channel* create_channel(Client *client, std::string &name, std::string &password);
	void delete_channel(int index);
	void delete_channel(Channel *chan);
	// Exception
	class ServerException : public std::runtime_error {
	public:
		ServerException(std::string err);
	};
};

#endif
