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

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>

# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Parser.hpp"
# include "ClientMode.hpp"
# include "ChannelMode.hpp"
# include "IRCExeption.hpp"

#endif
