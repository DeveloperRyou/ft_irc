#ifndef PARSER_HPP
# define PARSER_HPP

#include "ft_irc.hpp"
#include <map>

class Server;
class Client;

class Parser{
private:
	std::map<std::string, void (Parser::*)(Server &, Client &, std::string &)> operators;
	std::string getOperator(std::string &msg);
	std::string getArgument(std::string &msg);
	void user(Server &serv, Client &cli, std::string &argv);
	void pass(Server &serv, Client &cli, std::string &argv);
	void nick(Server &serv, Client &cli, std::string &argv);
	void quit(Server &serv, Client &cli, std::string &argv);
	void join(Server &serv, Client &cli, std::string &argv);
	void mode(Server &serv, Client &cli, std::string &argv);
	void topic(Server &serv, Client &cli, std::string &argv);
	void invite(Server &serv, Client &cli, std::string &argv);
	void kick(Server &serv, Client &cli, std::string &argv);
	void primsg(Server &serv, Client &cli, std::string &argv);
	void part(Server &serv, Client &cli, std::string &argv);
public:
	Parser(void);
	void parsing(Server &serv, Client &cli, std::string &msg);
};

#endif