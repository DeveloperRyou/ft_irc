#ifndef CLIENTMODE_HPP
# define CLIENTMODE_HPP

# include "ft_irc.hpp"

class ClientMode{
private:
	unsigned int _type;
public:
	static const unsigned int INVITED = 0x00;
	static const unsigned int JOINED = 0x01;
	static const unsigned int OPERATE = 0x02;
	
	ClientMode(const unsigned int type);

	bool isInvited() const;
	bool isJoined() const;
	bool isOperate() const;

	unsigned int getClientMode() const;
	void setClientMode(const unsigned int type);
	void addClientMode(const unsigned int type);
	void subClientMode(const unsigned int type);
};


#endif