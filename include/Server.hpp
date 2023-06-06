#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

# define CLIENT_MAX 10
# define CHANNEL_MAX 5
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

	void readClient();
	void deleteClient(int index);
	void deleteChannel(int index);
public:
	Server(int port, std::string password);
	~Server();
	void open();
	void loop();
	void checkPassword(std::string &password);
	Client* getClient(std::string &client_name);
	Channel* getChannel(std::string &channel_name);

	static std::string getPrefix(void);

	// handle client
	Client* createClient();
	void deleteClient(Client *cli);
	// handle channel
	Channel* createChannel(Client *client, std::string &name);
	void deleteChannel(Channel *chan);
	// Exception
	class ServerException : public std::runtime_error {
	public:
		ServerException(std::string err);
	};
};

#endif
