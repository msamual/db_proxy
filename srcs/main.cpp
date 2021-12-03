#include "header.hpp"

int main(int ac, char **av)
{
	std::string 	listen;
	std::string 	db_adress;

	if (ac < 3)
		read_from_console(listen, db_adress);
	else
	{
		listen = av[1];
		db_adress = av[2];
		if (check_and_trim_address(listen) == false ||
				check_and_trim_address(db_adress) == false)
			return puterror("Error: incorrect address");
	}
	Proxy	server(listen, db_adress);
	return 0;
}