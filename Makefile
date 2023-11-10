BLACK = \033[30m
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
PINK =\033[35m
CYAN = \033[36m
GREY =\033[37m
BOLD = \033[1m
LINE = \033[4m
NONE = \033[0m

NAME=ircserv

INC = ./include
HEARDER = $(INC)/irc.hpp $(INC)/server.hpp

CC = c++
CFLAGS = -g3 -Wall -Werror -Wextra -std=c++98

DIR_SRCS = ./srcs
SRCS = main.cpp server.cpp
SRC = $(addprefix $(DIR_SRCS)/, $(SRCS))
OBJS = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEARDER)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Compilation completed successfully$(NONE)"


.cpp.o : 
	@$(CC) $(CFLAGS) -I$(INC) -c $< -o $@


clean:
	@rm -f $(OBJS)
	@echo "$(RED)Objects files deleted$(NONE)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Executable file deleted$(NONE)"

re: fclean all


.PHONY: all clean fclean re