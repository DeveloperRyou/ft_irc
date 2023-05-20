#include "Parser.hpp"

Parser::Parser(void)
{
	operators["USER"] = &Parser::user;
	operators["PASS"] = &Parser::pass;
	operators["NICK"] = &Parser::nick;
	operators["QUIT"] = &Parser::quit;
	operators["JOIN"] = &Parser::join;
	operators["MODE"] = &Parser::mode;
	operators["TOPIC"] = &Parser::topic;
	operators["INVITE"] = &Parser::invite;
	operators["KICK"] = &Parser::kick;
	operators["PRIMSG"] = &Parser::primsg;
	operators["PART"] = &Parser::part;
}

std::string Parser::getOperator(std::string &msg)
{
	size_t index = msg.find_first_of(' ');
	return (msg.substr(0, index));
}

std::string Parser::getArgument(std::string &msg)
{
	size_t index = msg.find_first_of(' ');
	return (msg.substr(index + 1));
}

void Parser::parsing(Server *serv, Client *cli, std::string &msg)
{
	std::string op = getOperator(msg);
	std::string argv = getArgument(msg);
	std::cout <<op<<std::endl<<argv;

	if (operators.find(op) != operators.end())
		(this->*operators[op])(serv, cli, argv);
	else
		throw std::runtime_error("invalid operator");
}

void Parser::user(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::pass(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::nick(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::quit(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::join(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::mode(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::topic(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::invite(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::kick(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::primsg(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
void Parser::part(Server *serv, Client *cli, std::string &argv)
{(void)serv;(void)cli;(void)argv;}
