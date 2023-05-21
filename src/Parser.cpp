#include "Parser.hpp"

Parser::Parser(void) : operators()
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
	size_t colon = msg.find_first_of(':');
	if (colon != std::string::npos)
	{
		std::string before_colon = msg.substr(0, colon);
		split(before_colon, ' ', argv);
		argv.push_back(msg.substr(colon + 1));
	}
	else
		split(msg, ' ', argv);
}

void Parser::split(std::string &str, char sep, std::vector<std::string> &array)
{
	size_t index = 0;
	size_t next_index = str.find_first_of(sep);
	while(index != std::string::npos)
	{
		size_t size = next_index - index;	
		if (size > 0)
			array.push_back(str.substr(index, size));
		if (next_index == std::string::npos)
			break;
		index = next_index + 1;
		next_index = str.find_first_of(sep, index);
	}
}

void Parser::parsing(Server *serv, Client *cli, std::string &msg)
{
	msg.erase(msg.find('\n'));
	std::string op = getOperator(msg);
	std::vector<std::string> argv;
	getArguments(msg, argv);

	for (size_t i = 0;i < argv.size();i++)
		std::cout<<argv[i]<<std::endl;

	if (operators.find(op) != operators.end())
		(this->*operators[op])(serv, cli, argv);
	else
		throw ParserException("invalid operator");
	
}

void Parser::user(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 4)
		throw ParserException("USER : invalid argument");
	cli->setUsername(argv[0]);
	cli->setHostname(argv[1]);
	cli->setServername(argv[2]);
	cli->setRealname(argv[3]);
}

void Parser::pass(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 1)
		throw ParserException("PASS : invalid argument");
	if (serv->checkPassword(argv[0]))
		cli->setauthorization(true);
	else
		cli->send_to_Client("Wrong Password"); // return message formating?
}

void Parser::nick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 1)
		throw ParserException("NICK : invalid argument");
	cli->setNickname(argv[0]);
}

void Parser::quit(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() > 1)
		throw ParserException("QUIT : invalid argument");
	if (argv.size() == 1)
		cli->send_to_Client(argv[0]); // is it real? to resend argv?
	serv->delete_client(cli);
}

void Parser::privmsg(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 2)
		throw ParserException("PRIMSG : invalid argument");
	std::vector<std::string> receivers;
	split(argv[0], ',', receivers);
	for (size_t i = 0; i < receivers.size(); i++)
	{
		Client *user = serv->getClient(receivers[i]);
		user->send_to_Client(argv[1]);
	}
}

// channel operator
void Parser::join(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() < 1 || argv.size() > 2)
		throw ParserException("JOIN : invalid argument");
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	split(argv[0], ',', channels);
	if (argv.size() == 2)
		split(argv[1], ',', keys);
	for (size_t i = keys.size(); i < channels.size(); i++)
		keys.push_back("");
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = serv->getChannel(channels[i]);
		if (chan == NULL)
		{
			chan = serv->create_channel(cli, channels[i], keys[i]); // making new channel?
		}
		cli->join_channel(chan);
	}
}

void Parser::part(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 1)
		throw ParserException("PART : invalid argument");
	std::vector<std::string> channels;
	split(argv[0], ',', channels);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = serv->getChannel(channels[i]);
		chan->leave_channel(cli, "????");
		cli->leaveChannel(chan, "??????");
	}
}

void Parser::invite(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 2)
		throw ParserException("INVITE : invalid argument");
	Client *user = serv->getClient(argv[0]);
	Channel *chan = serv->getChannel(argv[1]);
	chan->invite(user, "???");
}

void Parser::kick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() < 2 || argv.size() > 3)
		throw ParserException("KICK : invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	chan->kick(cli, argv[1], argv[2]);
}

void Parser::mode(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() < 2 || argv.size() > 5)
		throw ParserException("MODE : invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	chan->change_mode(cli, argv[1]);
}

void Parser::topic(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() < 1 || argv.size() > 2)
		throw ParserException("TOPIC : invalid argument");
	if (argv.size() == 1)
	{
		std::string topic = serv->getChannel(argv[0])->getTopic();
		cli->send_to_Client(topic); // is this right way to send topic?
	}
	if (argv.size() == 2)
	{
		serv->getChannel(argv[0])->change_topic(cli, argv[1]);
	}
}

Parser::ParserException::ParserException(std::string err) 
	: std::runtime_error("[Parser] Error : "  + err) {}
