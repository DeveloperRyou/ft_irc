#include "ft_irc.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		return (1);

	std::stringstream ss(argv[1]);
	int port;
	ss >> port;
	std::string password = argv[2];

	Server serv(port, password);
	serv.open();
	serv.loop();
}
