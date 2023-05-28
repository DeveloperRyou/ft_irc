#ifndef IRC_EXCEPTION_HPP
# define IRC_EXCEPTION_HPP

# include "ft_irc.hpp"

class IRCException : public std::runtime_error {
public:
	IRCException(std::string err);
};

#endif