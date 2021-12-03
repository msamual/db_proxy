NAME	= db_proxy

FILES	= main configuration puterror Proxy
HEADERS = Proxy

CXX		= clang++

SRC_DIR	= srcs/
OBJ_DIR	= objs/
INCLUDE_DIR = includes/

FLAGS	= -Wall -Wextra -Werror -I $(INCLUDE_DIR)

SRCS 	= $(patsubst %, $(SRC_DIR)%.cpp, $(FILES))
HDRS	= $(patsubst %, $(INCLUDE_DIR)%.hpp, $(HEADERS))
OBJS 	= $(patsubst %, $(OBJ_DIR)%.o, $(FILES))

all: $(NAME)

$(NAME): $(OBJS) $(HDRS)
	$(CXX) -o $(NAME) $(OBJS) $(FLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HDRS)
	mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f ./$(NAME)

re: fclean all

.PHONY: all clean fclean re