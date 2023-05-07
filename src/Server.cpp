#include "Server.hpp"

Server::Server() : _server_socket(0)
{
	for (int i=0; i<CLIENT_MAX; i++)
		poll_fds[i].fd = -1;
}

Server::Server(int port, std::string password) : _port(port), _password(password)
{}

Server::~Server()
{
	for (std::vector<Client *>::iterator it = clients.begin();it < clients.end(); it++)
		delete *it;
}

void Server::open()
{
	// make socket file discriptor
	_server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1)
		throw ServerException("Failed making socket");

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
			create_client();
		else {
			for(int i = 1; i < CLIENT_MAX; i++) {
                if(poll_fds[i].fd >= 0 && poll_fds[i].revents & (POLLIN | POLLERR))
				{
					char buf[BUFFER_SIZE];
					memset(buf, 0, sizeof(buf));
					//string msg = clients[i].receive();
					int n = read(poll_fds[i].fd, buf, sizeof(buf));
					printf("client%d : %s", i, buf);
					if(n < 0){
						// delete clients[i];
						// clients[i].erase();
						close(poll_fds[i].fd);
						poll_fds[i].fd = -1;
					}
					else{
						parse_message(buf);
						for(int j = 1;j < CLIENT_MAX; j++){
							if (poll_fds[j].fd >= 0)
								dprintf(poll_fds[j].fd, "client%d : %s", i, buf);
								//clients[i].send(msg);
						}
					}
                }
        	}
		}
	}
}


std::string to_upper(std::string str)
{
	for (std::string::iterator it = str.begin(); it < str.end(); it++)
		*it = ::tolower(*it);
	return (str);
}

void Server::parse_message(std::string msg)
{
	if(to_upper(msg) == "QUIT")
		std::cout << "QUIT" << std::endl;
}

void Server::delete_client()
{

}

void Server::create_client()
{
	// add client
	struct sockaddr_in client_addr;
	socklen_t client_addr_size  = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));
	int client_socket = accept(_server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_socket == -1)
		throw ServerException("Failed accepting");

	// Client c = new Client(client_socket);
	// clients.push_back(c);	
	for(int i = 1; i < CLIENT_MAX; i++){
		if(poll_fds[i].fd < 0){
			printf("new client join : client%d\n", i);
			poll_fds[i].fd = client_socket;
			poll_fds[i].events = POLLIN;
			dprintf(poll_fds[i].fd, "Hello, you are client%d\n", i);
			break;
		}
	}
}

Server::ServerException::ServerException(std::string err) 
	: std::runtime_error("[Server] Error : "  + err) {}
