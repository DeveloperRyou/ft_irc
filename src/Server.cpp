#include "ft_irc.hpp"

Server::Server(int port, std::string password) : _server_socket(0), _port(port), _password(password)
{
	parser = new Parser();
	for (int i=0; i<CLIENT_MAX + 2; i++)
		poll_fds[i].fd = -1;
}

Server::~Server()
{
	for (std::vector<Client *>::iterator it = clients.begin();it < clients.end(); it++)
		delete *it;
	for (std::vector<Channel *>::iterator it = channels.begin();it < channels.end(); it++)
		delete *it;
	delete parser;
}

void Server::open()
{
	// make socket file discriptor
	_server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
		throw ServerException("Failed making socket");
	fcntl(_server_socket, F_SETFL, O_NONBLOCK);

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(_port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// binding socket to process
	if (bind(_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		throw ServerException("Failed binding socket");
	
	// listening client connect request
	if(listen(_server_socket, 5) == -1)
		throw ServerException("Failed listening socket");
	
	poll_fds[0].fd = _server_socket;
	poll_fds[0].events = POLLIN;
	std::cout << "ircserv started" << std::endl;
}

void Server::loop()
{
	while (1)
	{
		int p = poll(poll_fds, CLIENT_MAX, 5000);

		if (p < 0)
			throw ServerException("Poll error");
		if (p == 0)
			continue;
		
		if(poll_fds[0].revents & POLLIN)
			createClient()->send_to_Client("Hello\n");
		else
			readClient();
	}
}

void Server::checkPassword(std::string &password)
{
	if (password != this->_password)
		throw IRCException("Invalid password");
}

Client* Server::getClient(std::string &client_name)
{
	for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->getNickname() == client_name)
			return (*it);
	}
	return NULL;
}

Channel* Server::getChannel(std::string &channel_name)
{
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
		if ((*it)->ch_info->getName() == channel_name)
			return (*it);
	return NULL;
}


std::string Server::getPrefix(void)
{
	return ":ft_irc:";
}

Channel* Server::createChannel(Client *client, std::string &name, std::string &password)
{
	if (channels.size() == CHANNEL_MAX)
		throw ServerException("Too many channels");
	try
	{
		Channel* c = new Channel(client, name, password);
		channels.push_back(c);

		std::cout << "new channel created" << std::endl;
		return c;
	}
	catch(const std::exception& e)
	{
		throw ServerException("Failed making new Channel");
	}
}

void Server::deleteChannel(int index)
{
	delete channels[index];
	channels.erase(channels.begin() + index);
}

void Server::deleteChannel(Channel *chan)
{
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i] == chan)
		{
			deleteChannel(i);
			return ;	
		}
	}
	throw IRCException("No such channel");
}

void Server::readClient()
{
	for(size_t i = 1; i <= clients.size(); i++) {
		if(poll_fds[i].revents & (POLLIN | POLLERR)) {
			Client* cli = clients[i - 1];
			std::string receive;
			try
			{
				receive = cli->recv_from_Client();
			}
			catch(const std::exception& e)
			{
				deleteClient(i - 1);
				throw ServerException("Failed to receive from Client");
			}
			std::cout<<"client"<<i<<" : "<<receive;
			try
			{
				parser->parsing(this, cli, receive);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
}

Client* Server::createClient()
{
	if (clients.size() == CLIENT_MAX)
		throw ServerException("Too many Clients");
	try
	{
		Client* c = new Client(_server_socket);
		clients.push_back(c);

		// modify poll array
		int index = clients.size();
		poll_fds[index].fd = c->getSock();
		poll_fds[index].events = POLLIN;

		std::cout << "new client join" << std::endl;
		return c;
	}
	catch(const std::exception& e)
	{
		throw ServerException("Failed making new Client");
	}
}

void Server::deleteClient(int index)
{
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		try
		{
			(*it)->subClient(clients[index]);
			if ((*it)->ch_info->getClientSize() == 0)
				deleteChannel(*it);
		}
		catch (std::exception &e) {}
	}
	delete clients[index];
	clients.erase(clients.begin() + index);

	// modify poll array
	close(poll_fds[index + 1].fd);
	for (++index; index <= CLIENT_MAX; index++) {
		poll_fds[index] = poll_fds[index + 1];
	}
}

void Server::deleteClient(Client *cli)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i] == cli)
		{
			deleteClient(i);
			return ;
		}
	}
	throw IRCException("No such client");
}

Server::ServerException::ServerException(std::string err) 
	: std::runtime_error("[Server] Error : "  + err) {}
