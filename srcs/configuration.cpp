//
// Created by Sergey Uryashev on 17.11.2021.
//

#include "../includes/header.hpp"

std::string 	trim(std::string s)
{
	s.erase(s.begin(), s.begin() + s.find_first_not_of(" \t"));
	s.erase(s.begin() + s.find_last_not_of(" \t") + 1, s.end());
	return s;
}

bool	check_and_trim_address(std::string& input)
{
	std::string	host;
	std::string	port;
	size_t 		i;

	if ((i = input.find(':', 0)) == std::string::npos)
		return false;
	host = trim(std::string(input.begin(), input.begin() + i));
	port = trim(std::string(input.begin() + i + 1, input.end()));
	if (port.find_last_not_of("0123456789") != std::string::npos)
		return false;
	if (host == "localhost")
		host = "127.0.0.1";
	input = host + ":" + port;
	return true;
}

void read_from_console(std::string& listen, std::string& db_address)
{
	while (true)
	{
		std::cout << "input listen adress: ";
		std::getline(std::cin, listen);
		if (check_and_trim_address(listen) == false)
			std::cout << "Please input correct address" << std::endl <<
				"For example: \"127.0.0.1:3305\"" << std::endl;
		else
			break ;
	}
	while (true)
	{
		std::cout << "input db adress: ";
		std::getline(std::cin, db_address);
		if (check_and_trim_address(db_address) == false)
			std::cout << "For example: \"localhost:3306\"" << std::endl;
		else
			break ;
	}
}

