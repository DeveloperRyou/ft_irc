#ifndef CLIENTMODE_HPP
# define CLIENTMODE_HPP

#include "ft_irc.hpp"

class ClientMode{
private:
	unsigned int _type;
public:
	const unsigned int INVITED = 0x00;
	const unsigned int JOINED = 0x01;
	const unsigned int OPERATE = 0x02;
	
	ClientMode(const unsigned int type);

	bool isInvited() const;
	bool isJoined() const;
	bool isOperate() const;
	void setClientMode(const unsigned int type);
};


#endif