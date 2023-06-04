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
	{
		cli->send_to_Client(Server::getPrefix() + " 421 " + cli->getNickname() + " " + op + " :Unknown command");
		return ;
	}
}

// private operator function

void Parser::user(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() < 4)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " USER :Not enough parameters.");
		return ;
	}
	cli->setUsername(argv[0]);
	cli->setHostname(argv[1]);
	cli->setServername(argv[2]);
	cli->setRealname(argv[3]);
}

void Parser::pass(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() == 0)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " PASS :Not enough parameters.");
		return ;
	}
	serv->checkPassword(argv[0]);
	cli->setAuthorization(true);
}

void Parser::nick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() == 0)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " NICK :Not enough parameters.");
		return ;
	}
	if (cli->getNickname() == argv[0])
		return ;
	if (serv->getClient(argv[0]))
	{
		cli->send_to_Client(Server::getPrefix() + " 433 " + cli->getNickname() + " " 
			+ argv[0] + " :Nickname is already in use.");
		return;
	}
	cli->setNickname(argv[0]);
}

void Parser::quit(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (argv.size() == 0)
		cli->send_to_Client("ERROR: Closing link: (~" + cli->getUsername() +"@" 
			+ cli->getHostname() + ") [Client  exited]");
	else
	{
		std::string reason = "";
		for (size_t i = 0; i < argv.size(); i++)
			reason += argv[i] + " ";
		cli->send_to_Client("ERROR: Closing link: (~" + cli->getUsername() +"@" 
			+ cli->getHostname() + ") [QUIT: " + reason + "]");
	}
	serv->deleteClient(cli);
}

// channel operator
void Parser::join(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() +" JOIN :Client not registerd\r\n");
		return;
	}
	if (argv.size() == 0)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() +" JOIN :Not enough parameters.\r\n");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() +" JOIN :<channel>[,<channel>]+ [<key>[,<key>]+]\r\n");
		return ;
	}
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
		else
			chan->join(cli, keys[i]);
	}
}

void Parser::part(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " PART :Client not registerd");
		return;
	}
	if (argv.size() == 0)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " PART :Not enough parameters.");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() + " PART :<channel>[,<channel>]+ [:<reason>]");
		return ;
	}
	std::string reason = "";
	for (size_t i = 1; i < argv.size(); i++)
		reason += argv[i] + " ";

	std::vector<std::string> channels;
	split(argv[0], ',', channels);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = serv->getChannel(channels[i]);
		if (chan == NULL)
			cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " + channels[i] + " PART :No such channel");
		else
			chan->part(cli, reason);
	}
}

void Parser::invite(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " INVITE :Client not registerd");
		return;
	}
	if (argv.size() < 3)
	{
		cli->send_to_Client(Server::getPrefix() + " 337 " + cli->getNickname() + " :End of INVITE list");
		return ;
	}
	else if (argv.size() > 3)
	{
		cli->send_to_Client(Server::getPrefix() + " NOTICE " + cli->getNickname() + " :*** Invalid duration for invite");
		return ;
	}

	Channel *chan = serv->getChannel(argv[1]);
	if (chan == NULL)
	{
		cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " + argv[1] + " INVITE :No such channel");
		return ;
	}
	Client *user = serv->getClient(argv[0]);
	if (user == NULL)
	{
		cli->send_to_Client(Server::getPrefix() + " 401 " + cli->getNickname() + " " + argv[0] + " INVITE :No such nick");
		return ;
	}
	chan->invite(cli, user);
}

void Parser::kick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " KICK :Client not registerd");
		return;
	}

	if (argv.size() < 2)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() +" KICK :Not enough parameters.\r\n");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() +" KICK :<channel> <nick>[,<nick>]+ [:<reason>]\r\n");
		return ;
	}

	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
	{
		cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " + argv[0] + " KICK :No such channel");
		return ;
	}

	std::string comment = "";
		for (size_t i = 2; i < argv.size(); i++)
			comment += argv[i] + " ";

	std::vector<std::string> nick;
	split(argv[1], ',', nick);
	for (size_t i = 0; i < nick.size(); i++)
	{
		Client *client = serv->getClient(nick[i]);
		if (client == NULL)
		{
			cli->send_to_Client(Server::getPrefix() + " 401 " + cli->getNickname() + " " + nick[i] + " KICK :No such nick");
			return ;
		}
		else
			chan->kick(cli, client, comment);
	}
}

void Parser::mode(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " MODE :Client not registerd");
		return;
	}
	if (argv.size() == 1)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " MODE :Not enough parameters.");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() 
			+ " MODE :<target> [[(+|-)]<modes> [<mode-parameters>]]");
		return ;
	}
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
	{
		cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " + argv[0] + " MODE :No such channel");
		return ;
	}
	argv.erase(argv.begin());
	chan->mode(cli, argv);
}

void Parser::topic(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " TOPIC :Client not registerd");
		return;
	}
	if (argv.size() == 1)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " TOPIC :Not enough parameters.");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() + " TOPIC :<channel> [:<topic>]");
		return ;
	}
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
	{
		cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " + argv[0] + " TOPIC :No such channel");
		return ;
	}
	std::string ch_topic = "";
	for (size_t i = 1; i < argv.size(); i++)
		ch_topic += argv[i] +" ";
	chan->topic(cli, ch_topic);
}

void Parser::privmsg(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv;
	if (cli->getAuthorization() == false)
	{
		cli->send_to_Client(Server::getPrefix() + " 451 " + cli->getNickname() + " PRIVMSG :Client not registerd");
		return;
	}
	if (argv.size() < 2)
	{
		cli->send_to_Client(Server::getPrefix() + " 461 " + cli->getNickname() + " PRIVMSG :Not enough parameters.");
		cli->send_to_Client(Server::getPrefix() + " 650 " + cli->getNickname() + " PRIVMSG :<target>[,<target>]+ :<message>");
		return ;
	}

	std::string msg = "";
	for (size_t i = 1; i < argv.size(); i++)
		msg += argv[i] + " ";

	std::vector<std::string> target;
	split(argv[0], ',', target);
	for (size_t i = 0; i < target.size(); i++)
	{
		if (target[i][0] == '#')
		{
			Channel *chan = serv->getChannel(target[i]);
			if (chan == NULL)
			{
				cli->send_to_Client(Server::getPrefix() + " 403 " + cli->getNickname() + " " 
					+ target[i] + " PRIVMSG :No such channel");
				continue;
			}
			chan->privmsg(cli, msg);
		}
		else
		{
			Client *client = serv->getClient(target[i]);
			if (client == NULL)
			{
				cli->send_to_Client(Server::getPrefix() + " 401 " + cli->getNickname() + " " 
					+ target[i] + " PRIVMSG :No such nick");
				continue;
			}
			client->send_to_Client(cli->getPrefix() + " PRIVMSG " + cli->getNickname() + " :" + msg );
		}
	}
}
