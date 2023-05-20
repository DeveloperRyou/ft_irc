#ifndef PARSER_HPP
# define PARSER_HPP

#include "ft_irc.hpp"
#include <map>

class Server;
class Client;

class Parser{
private:
	std::map<std::string, void (Parser::*)(Server *, Client *, std::vector<std::string> &)> operators;
	std::string getOperator(std::string &msg);
	void getArguments(std::string &msg, std::vector<std::string> &argv);
	void split(std::string &str, char sep, std::vector<std::string> &array);
	void user(Server *serv, Client *cli, std::vector<std::string> &argv);
	void pass(Server *serv, Client *cli, std::vector<std::string> &argv);
	void nick(Server *serv, Client *cli, std::vector<std::string> &argv);
	void quit(Server *serv, Client *cli, std::vector<std::string> &argv);
	void join(Server *serv, Client *cli, std::vector<std::string> &argv);
	void mode(Server *serv, Client *cli, std::vector<std::string> &argv);
	void topic(Server *serv, Client *cli, std::vector<std::string> &argv);
	void invite(Server *serv, Client *cli, std::vector<std::string> &argv);
	void kick(Server *serv, Client *cli, std::vector<std::string> &argv);
	void privmsg(Server *serv, Client *cli, std::vector<std::string> &argv);
	void part(Server *serv, Client *cli, std::vector<std::string> &argv);
public:
	Parser(void);
	void parsing(Server *serv, Client *cli, std::string &msg);

	// Exception
	class ParserException : public std::runtime_error {
	public:
		ParserException(std::string err);
	};
};

#endif