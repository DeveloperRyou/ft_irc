#ifndef FT_IRC_HPP
# define FT_IRC_HPP

# include <iostream>
# include <stdexcept>
# include <unistd.h>
# include <fcntl.h>
# include <poll.h>
# include <vector>
# include <map>
# include <cstring>
# include <sstream>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>

void error(std::string err);

# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Parser.hpp"
# include "ClientMode.hpp"
# include "ChannelMode.hpp"
# include "ChannelInfo.hpp"
# include "IRCExeption.hpp"

# define RED "\033[0;31m"
# define WHITE "\033[0;37m"

#endif
