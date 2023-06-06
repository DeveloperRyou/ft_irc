#include "ft_irc.hpp"

void error(std::string err)
{
	std::cerr << RED << err << WHITE << '\n';
}

int main(int argc, char **argv)
{
	if (argc != 3)
		return (1);

	std::stringstream ss(argv[1]);
	int port;
	ss >> port;
	std::string password = argv[2];
	std::cout<<"port : "<<port<<", password : "<<password<<'\n';

	if (port <= 0 || port > 65535)
	{
		error("wrong port");
		return (1);
	}

	Server serv(port, password);
	try
	{
		serv.open();
		serv.loop();
	}
	catch(const std::exception& e)
	{
		error(e.what());
		return (1);
	}
}
