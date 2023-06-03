#include "ft_irc.hpp"

//connect msg 어디서?
//throw vs send_to_Client?

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

	try
	{	
		if (operators.find(op) != operators.end())
			(this->*operators[op])(serv, cli, argv);
		else
			throw IRCException(" 421 " + cli->getNickname() + " " + op + " :Unknown command");
	}
	catch(const IRCException& e)
	{
		cli->send_to_Client(Server::getPrefix() + e.what());
		cli->send_to_Client("\n");
		std::cerr << e.what() << "\n";
	}
}

// private operator function

void Parser::user(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 5)
		throw IRCException(" 461 * USER :Not enough parameters.");
	cli->setUsername(argv[1]);
	cli->setHostname(argv[2]);
	cli->setServername(argv[3]);
	cli->setRealname(argv[4]);
}

void Parser::pass(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 2)
		throw IRCException(" 461 * PASS :Not enough parameters.");
	serv->checkPassword(argv[1]);
	cli->setAuthorization(true);
}

void Parser::nick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() != 2)
		throw IRCException(" 461 * NICK :Not enough parameters.");
	//기존에 저장된 닉네임과 중복검사
	//:*.freenode.net 433 MyGuest1 hello :Nickname is already in use.
	//기존 닉네임과 동일하게 변경 요청 -> 아무 메시지 발생 x
	//파라미터 3개면 
	//:*.freenode.net 477 MyGuest MyGuest :You need to be identified to a registered account to message this user
	cli->setNickname(argv[1]);
}

void Parser::quit(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (argv.size() > 2)//??? 확인해봐야 함
		throw IRCException("QUIT : Invalid argument");
	if (argv.size() == 2) //ERROR :Closing link: (~u@59.13.196.36) [Client exited]
		cli->send_to_Client("QUIT : " + argv[0]);
	//client가 있던 채널에서 삭제 및 브로드캐스팅
	serv->deleteClient(cli);
}

// channel operator
void Parser::join(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	//새로운 채널 생성 -> 생성만 가능 비밀번호 설정 불가 한 채널만 가능
	//기존 생성 조인 -> 여러 채널 한번에 가능 비번 입력 가능
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException(" 451 * JOIN :Client not registerd");
	if (argv.size() < 2)
		throw IRCException(" 461 " + cli->getPrefix() +" JOIN :Not enough parameters.");
	//:*.freenode.net 650 MyGuest JOIN :<channel>[,<channel>]+ [<key>[,<key>]+]

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
		throw IRCException(" 451 * PART :Client not registerd");
	if (argv.size() != 1)
		throw IRCException(" 461 " +cli->getNickname() + " PART :Not enough parameters.");
		//:*.freenode.net 650 MyGuest PART :<channel>[,<channel>]+ [:<reason>]

	std::vector<std::string> channels;
	split(argv[0], ',', channels);
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = serv->getChannel(channels[i]);
		if (chan == NULL)
			throw IRCException(" 403 " + cli->getNickname() + " PART :No such channel");
		chan->part(cli);
	}
}

void Parser::invite(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException(" 451 * INVITE :Client not registerd");
	//인자 없을 때 부족할 때 //:*.freenode.net 337 MyGuest :End of INVITE list
	// :*.freenode.net NOTICE MyGuest :*** Invalid duration for invite // 인자 많을 때
	if (argv.size() != 2)
		throw IRCException("INVITE : Invalid argument");
	Client *user = serv->getClient(argv[1]);
	if (user == NULL)
		throw IRCException(" 401 " + cli->getNickname() + " " + argv[1] + " INVITE :No such nick");
	Channel *chan = serv->getChannel(argv[2]);
	if (chan == NULL)
		throw IRCException(" 403 " + cli->getNickname() + " " + argv[2] + " INVITE :No such channel");
	chan->invite(cli, user);
}

void Parser::kick(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException(" 451 * KICK :client not registerd");

// 	:*.freenode.net 461 MyGuest KICK :Not enough parameters.
// :*.freenode.net 650 MyGuest KICK :<channel> <nick>[,<nick>]+ [:<reason>]
	if (argv.size() < 2 || argv.size() > 3)
		throw IRCException("KICK : Invalid argument");
	Channel *chan = serv->getChannel(argv[1]);
	if (chan == NULL)
		throw IRCException(" 403 " + cli->getNickname() + " " + argv[1] + " KICK :No such channel");
	Client *user = serv->getClient(argv[2]);
	if (user == NULL)
		throw IRCException(" 401 " + cli->getNickname() + " " + argv[2] + " KICK :No such nick");
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
		throw IRCException(" 451 * MODE :Client not registerd");
	if (argv.size() < 2 || argv.size() > 5)
		throw IRCException("MODE : Invalid argument");
	Channel *chan = serv->getChannel(argv[0]);
	if (chan == NULL)
		throw IRCException(" 403 " + cli->getNickname() + " " + argv[1] + " MODE :No such channel");
	chan->mode(cli, argv);
}

void Parser::topic(Server *serv, Client *cli, std::vector<std::string> &argv)
{
	(void)serv; (void)cli; (void)argv;
	if (cli->getAuthorization() == false)
		throw IRCException(" 451 * TOPIC :Client not registerd");
// :*.freenode.net 461 MyGuest TOPIC :Not enough parameters.
// :*.freenode.net 650 MyGuest TOPIC :<channel> [:<topic>]
	if (argv.size() < 1 || argv.size() > 2)
		throw IRCException("TOPIC : Invalid argument");
	Channel *chan = serv->getChannel(argv[1]);
	if (chan == NULL)
		throw IRCException(" 403 " + cli->getNickname() + " " + argv[1] + " TOPIC :No such channel");
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
		throw IRCException(" 451 * PRIVMSG :Client not registerd");
// :*.freenode.net 461 MyGuest PRIVMSG :Not enough parameters.
// :*.freenode.net 650 MyGuest PRIVMSG :<target>[,<target>]+ :<message>
	if (argv.size() != 2)
		throw IRCException("PRIVMSG : Invalid argument");
	std::vector<std::string> receivers;
	split(argv[0], ',', receivers);
	for (size_t i = 0; i < receivers.size(); i++)
	{
		// only channel privmsg
		Channel *chan = serv->getChannel(receivers[i]);
		if (chan == NULL)
			throw IRCException(" 403 " + cli->getNickname() + " " + argv[1] + " PRIVMSG :No such channel");
		chan->privmsg(cli, argv[1]);
	}
	//개인 간 privmsg ??
}
