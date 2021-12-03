//
// Created by Sergey Uryashev on 17.11.2021.
//

#include "../includes/Proxy.hpp"

Proxy::Proxy(std::string listen, std::string db_address)
{
	std::cout << "starting proxy server..." << std::endl;
	std::cout << "listen: " << listen << std::endl;
	std::cout << "db: " << db_address << std::endl;
	init_host_port(listen, db_address);
	_log_fd = open(LOG_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND);
	if (create_listening_socket() == -1)
		return ;
	start();
}

Proxy::~Proxy()
{
	close(_listen_sd);
	close(_log_fd);
	for (int i = 0; i < _nfds; ++i)
	{
		if (_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
}

void 	Proxy::init_host_port(const std::string &host, const std::string &port)
{
	int 	i;

	i = host.find(':', 0);
	_listening_host = std::string(host.begin(), host.begin() + i);
	_listening_port = std::stoi(std::string(host.begin() + i + 1, host.end()));
	i = port.find(':', 0);
	_db_host = std::string(port.begin(), port.begin() + i);
	_db_port = std::stoi(std::string(port.begin() + i + 1, port.end()));
}

int 	Proxy::create_listening_socket()
{
	int					ret, on = 1;
	struct sockaddr_in	addr;

	_listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_sd < 0)
		return puterror("can't create socket", errno);
	ret  = setsockopt(_listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if  (ret == -1)
		return puterror("setsockopt() failed", errno);
	ret = fcntl(_listen_sd, F_SETFL, O_NONBLOCK);
	if (ret < 0)
		return (puterror("fcntl() failed", errno));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_listening_port);
	addr.sin_addr.s_addr = inet_addr(_listening_host.data());
	ret = bind(_listen_sd, (struct sockaddr*)&addr, sizeof(sockaddr));
	if (ret == -1)
		return puterror("bind() failed", errno);
	ret = listen(_listen_sd, 100);
	if (ret == -1)
		return puterror("listen() failed", errno);
	return _listen_sd;
}

int 	Proxy::create_db_connection()
{
	int					new_sd, ret;
	struct sockaddr_in	addr;

	new_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (new_sd < 0)
		return puterror("can't create db socket", errno);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_db_port);
	addr.sin_addr.s_addr = inet_addr(_db_host.data());
	ret = connect(new_sd, (struct sockaddr*)&addr, sizeof(sockaddr));
	if (ret < 0)
		return puterror("connect() failed", errno);
	_fds[_nfds].fd = new_sd;
	_fds[_nfds++].events = POLLIN;
	return new_sd;
}

int 	Proxy::start()
{
	int 	ret;

	_compress = false;
	memset(_fds, 0 , sizeof(_fds));
	_fds[0].fd = _listen_sd;
	_fds[0].events = POLLIN;
	_nfds = 1;
	while(true)
	{
		ret = poll(_fds, _nfds, TIMEOUT);
		if (ret < 0)
			return (puterror("poll() failed", errno));
		if (ret == 0)
			continue ;
		ret = events_handle();
		if (ret == -1)
			break ;
	}
	return 0;
}

int 	Proxy::events_handle()
{
	int 	current_size, ret;

	current_size = _nfds;
	for(int i = 0; i < current_size; ++i)
	{
		if (_fds[i].revents == 0)
			continue ;
		if (_fds[i].revents != POLLIN)
		{
			close_connection(_fds[i].fd, i);
			continue ;
		}
		if (_fds[i].fd == _listen_sd)
		{
			ret = create_new_client_connection();
			if (ret == -1)
				return ret;
		}
		else
		{
			ret = read_write_pack(_fds[i].fd);
			if (ret == -1)
				return -1;
		}
	}
	if (_compress == true)
		compress_array();
	return 0;
}

int 	Proxy::create_new_client_connection()
{
	int new_sd = 0;
	int db_sd = 0;

	while (new_sd != -1)
	{
		new_sd = accept(_listen_sd, NULL, NULL);
		if (new_sd < 0)
		{
			if (errno != EWOULDBLOCK)
				return puterror("accept() failed", errno);
			break ;
		}
		_fds[_nfds].fd = new_sd;
		_fds[_nfds++].events = POLLIN;
		db_sd = create_db_connection();
		if (db_sd < 0)
			return -1;
		_connections[new_sd] = db_sd;
		_rev_connections[db_sd] = new_sd;
	}
	std::cout << "new connection " << new_sd << " <--> " << db_sd << std::endl;
	return 0;
}

int 	Proxy::read_write_pack(int fd)
{
	char 	buf[CHUNK_SIZE];
	int 	ret , len, dest_fd;

	ret = recv(fd, buf, CHUNK_SIZE, 0);
	if (ret < 0 && errno != EWOULDBLOCK)
		return puterror("recv() failed", errno);
	if (ret == 0)
		return 0;
	len = ret;
	std::cout << len << " bytes received from fd " << fd << std::endl;
	if (_connections.find(fd) != _connections.end())
		dest_fd = _connections[fd];
	else
		dest_fd = _rev_connections[fd];
	if (is_sql_query(buf))
	{
		write(_log_fd, buf + 5, len - 5);
		write(_log_fd, "\n", 1);
	}
	ret = send(dest_fd, buf, len, 0);
	if (ret < 0)
		return puterror("send() failed", errno);
	return 0;
}

void 	Proxy::close_connection(int fd, int i)
{
	if (_connections.find(fd) != _connections.end())
	{
		std::cout << "close connection " << fd << " <--> " <<
			_connections[fd] << std::endl;
		close(_connections[fd]);
		_rev_connections.erase(_connections[fd]);
		_connections.erase(fd);
	}
	else
	{
		std::cout << "close connection " << fd << " <--> " <<
			_rev_connections[fd] << std::endl;
		close(_rev_connections[fd]);
		_connections.erase(_rev_connections[fd]);
		_rev_connections.erase(fd);
	}
	close(fd);
	_fds[i].fd = -1;
	_compress = true;
}

void 	Proxy::compress_array()
{
	int		i, j;

	_compress = false;
	for (i = 0; i < _nfds; i++)
	{
		if (_fds[i].fd == -1)
		{
			for (j = i; j < _nfds; j++)
			{
				_fds[j].fd = _fds[j + 1].fd;
			}
			i--;
			_nfds--;
		}
	}
}

bool 	Proxy::is_sql_query(const char *buf)
{
	if (buf[4] == 3)
		return true;
	return false;
}

