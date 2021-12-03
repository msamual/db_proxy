//
// Created by Sergey Uryashev on 17.11.2021.
//

#ifndef PROXY_H
# define PROXY_H

# include "header.hpp"

class Proxy
{
private:
	std::string			_listening_host;
	std::string 		_db_host;
	int 				_listening_port;
	int 				_db_port;
	int 				_listen_sd;
	pollfd 				_fds[MAX_CONN * 2];
	int 				_nfds;
	std::map<int, int>	_connections;
	std::map<int, int>	_rev_connections;
	bool 				_compress;
	int 				_log_fd;

public:
	Proxy(std::string listen		= DEFAULT_LISTEN,
		  std::string db_address	= DEFAULT_DB_ADDRESS);
	~Proxy();

private:
	int 	start();
	int 	events_handle();
	int 	create_new_client_connection();
	int 	read_write_pack(int fd);
	void 	close_connection(int fd, int i);
	void 	compress_array();
	bool 	is_sql_query(const char *buf);
	void	init_host_port(const std::string& host, const std::string& port);
	int 	create_listening_socket();
	int 	create_db_connection();
};


#endif //DATA_ARMOR_PROXY_H
