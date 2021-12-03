//
// Created by Sergey Uryashev on 18.11.2021.
//

#include "header.hpp"

int		puterror(std::string msg)
{
	std::cerr << "Error: " << msg << std::endl;
	return -1;
}

int 	puterror(std::string msg, int errno_code)
{
	char buf[256];

	strerror_r(errno_code, buf, sizeof(buf));
	std::cerr << "Error: " << msg << std::endl;
	std::cerr << "   Errno = " << errno_code << " " <<  buf << std::endl;
	return -1;
}