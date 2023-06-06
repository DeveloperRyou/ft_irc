# include "ft_irc.hpp"

IRCException::IRCException(std::string err) 
	: std::runtime_error("[FT_IRC] Error : "  + err) {}
