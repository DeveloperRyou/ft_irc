#include "ft_irc.hpp"

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
	std::string op = msg.substr(0, index);
	msg = msg.substr(index + 1);
	return (op);
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

	try
	{	
		if (operators.find(op) != operators.end())
			(this->*operators[op])(serv, cli, argv);
		else
			throw IRCException("Invalid operator");
	}
	catch(const IRCException& e)
	{
		cli->send_to_Client(e.what());
		cli->send_to_Client("\n");
		error(e.what());
	}
}

// private operator function

void Parser::user(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 4)
		throw IRCException("USER : Invalid argument");
	cli->setUsername(argv[0]);
	cli->setHostname(argv[1]);
	cli->setServername(argv[2]);
	cli->setRealname(argv[3]);
}

void Parser::pass(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 1)
		throw IRCException("PASS : Invalid argument");
	serv->checkPassword(argv[0]);
	cli->setAuthorization(true);
}

void Parser::nick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 1)
		throw IRCException("NICK : Invalid argument");
	cli->setNickname(argv[0]);
}

void Parser::quit(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() > 1)
		throw IRCException("QUIT : Invalid argument");
	if (argv.size() == 1)
		cli->send_to_Client("QUIT : " + argv[0]); // is it real? to resend argv?
	serv->deleteClient(cli);
}

// channel operator
void Parser::join(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("JOIN : Client not registerd");
	if (argv.size() < 1 || argv.size() > 2)
		throw IRCException("JOIN : Invalid argument");

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
			chan = serv->createChannel(cli, channels[i], keys[i]);
		chan->join(cli, keys[i]);
	}
}

void Parser::part(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("PART : Client not registerd");
	if (argv.size() != 1)
		throw IRCException("PART : Invalid argument");

	std::vector<std::string> channels;
	split(argv[0], ',', channels);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = serv->getChannel(channels[i]);
		if (chan == NULL)
			throw IRCException("PART : No such channel");
		chan->part(cli);
	}
}

void Parser::invite(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("INVITE : Client not registerd");
	if (argv.size() != 2)
		throw IRCException("INVITE : Invalid argument");
	Client *user = serv->getClient(argv[0]);
	if (user == NULL)
		throw IRCException("INVITE : No such user");
	Channel *chan = serv->getChannel(argv[1]);
	if (chan == NULL)
		throw IRCException("INVITE : No such channel");
	chan->invite(cli, user);
}

void Parser::kick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("KICK : client not registerd");
	if (argv.size() < 2 || argv.size() > 3)
		throw IRCException("KICK : Invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
		throw IRCException("KICK : No such channel");
	Client *user = serv->getClient(argv[1]);
	if (user == NULL)
		throw IRCException("KICK : No such user");
	if (argv.size() == 2) {
		std::string temp = cli->getNickname();
		chan->kick(cli, user, temp);
	}
	else
		chan->kick(cli, user, argv[2]);
}

void Parser::mode(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("MODE : Client not registerd");
	if (argv.size() < 2 || argv.size() > 5)
		throw IRCException("MODE : Invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
		throw IRCException("MODE : No such channel");
	chan->mode(cli, argv);
}

void Parser::topic(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("TOPIC : Client not registerd");
	if (argv.size() < 1 || argv.size() > 2)
		throw IRCException("TOPIC : Invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
		throw IRCException("TOPIC : No such channel");
	if (argv.size() == 1) {
		std::string temp = cli->getNickname();
		chan->topic(cli, temp);
	}
	else
		chan->topic(cli, argv[1]);
}

void Parser::privmsg(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException("PRIVMSG : Client not registerd");
	if (argv.size() != 2)
		throw IRCException("PRIVMSG : Invalid argument");
	std::vector<std::string> receivers;
	split(argv[0], ',', receivers);
	for (size_t i = 0; i < receivers.size(); i++)
	{
		// only channel privmsg
		Channel *chan = serv->getChannel(receivers[i]);
		if (chan == NULL)
			throw IRCException("PRIVMSG : No such channel");
		chan->privmsg(cli, argv[1]);
	}
}
