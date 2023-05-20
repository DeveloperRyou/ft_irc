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
	operators["PRIVMSG"] = &Parser::privmsg;
	operators["PART"] = &Parser::part;
}

std::string Parser::getOperator(std::string &msg)
{
	size_t index = msg.find_first_of(' ');
	return (msg.substr(0, index));
}

void Parser::getArguments(std::string &msg, std::vector<std::string> &argv)
{
	size_t index = msg.find_first_of(' ');
	size_t colon = msg.find_first_of(':');
	while (index + 1 < colon)
	{
		size_t next_index = msg.find_first_of(' ', index + 1);
		size_t size = next_index - index - 1;
		if (size > 0)
			argv.push_back(msg.substr(index + 1, size));
		index = next_index;
	}
	if (colon != std::string::npos)
		argv.push_back(msg.substr(colon + 1));
}

void Parser::parsing(Server *serv, Client *cli, std::string &msg)
{
	std::string op = getOperator(msg);
	std::vector<std::string> argv;
	getArguments(msg, argv);

	std::cout<<op<<std::endl;
	for (size_t i = 0;i < argv.size();i++)
		std::cout<<argv[i]<<std::endl;

	if (operators.find(op) != operators.end())
		(this->*operators[op])(serv, cli, argv);
	else
		throw ParserException("invalid operator");
}

void Parser::user(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() != 4)
		throw ParserException("USER : invalid argument");
	cli->setUsername(argv[0]);
	cli->setHostname(argv[1]);
	cli->setServername(argv[2]);
	cli->setRealname(argv[3]);
}

void Parser::pass(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() != 1)
		throw ParserException("PASS : invalid argument");
	//serv->checkPassword()
}

void Parser::nick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() != 1)
		throw ParserException("NICK : invalid argument");
	cli->setNickname(argv[0]);
}

void Parser::quit(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() > 2)
		throw ParserException("QUIT : invalid argument");
	//serv->deleteClient();
}

void Parser::join(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() < 1 || argv.size() > 3)
		throw ParserException("JOIN : invalid argument");
	//serv->createChannel
	//cli->joinChannel
}

void Parser::mode(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() < 2 || argv.size() > 5)
		throw ParserException("MODE : invalid argument");
}

void Parser::topic(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() < 1 || argv.size() > 3)
		throw ParserException("TOPIC : invalid argument");
	//serv->createChannel
	//cli->joinChannel
}

void Parser::invite(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() != 2)
		throw ParserException("INVITE : invalid argument");
	//serv->deleteClient();
}

void Parser::kick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() < 2 || argv.size() > 3)
		throw ParserException("KICK : invalid argument");
	//serv->deleteClient();
}

void Parser::privmsg(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() != 2)
		throw ParserException("PRIMSG : invalid argument");
	//serv->deleteClient();
}

void Parser::part(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	if (argv.size() != 1)
		throw ParserException("PART : invalid argument");
	//serv->deleteClient();
}

Parser::ParserException::ParserException(std::string err) 
	: std::runtime_error("[Parser] Error : "  + err) {}
