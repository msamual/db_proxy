//
// Created by Sergey Uryashev on 17.11.2021.
//

#ifndef HEADER_H
# define HEADER_H

# define DEFAULT_LISTEN "localhost:8080"
# define DEFAULT_DB_ADDRESS "localhost::3306"
# define MAX_CONN 100000
# define TIMEOUT 180000
# define CHUNK_SIZE	4096
# define LOG_FILE_NAME "log.txt"

# include <iostream>
#include <iostream>
#include <unistd.h>
#include <string>
#include <list>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <signal.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/ioctl.h>

# include "Proxy.hpp"

void 	read_from_console(std::string& listen, std::string& db_address);
int 	puterror(std::string msg, int err);
int 	puterror(std::string msg);
bool	check_and_trim_address(std::string& input);

#endif //HEADER_H
