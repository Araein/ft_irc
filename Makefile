NAME	= ircserv

SRCS	= main.cpp

COMPIL	= c++

REMOVE	= rm -rf

FLAGS	= -Wall -Wextra -Werror -std=c++98

OBJS	= ${SRCS:.cpp=.o}

all:		${NAME}

%.o: 		%.cpp
				${COMPIL} ${FLAGS} -o  $@ -c $<

$(NAME):	${OBJS}
				${COMPIL} -o ${NAME} ${OBJS}

clean: 
				${REMOVE} ${OBJS}

fclean: 	clean
				${REMOVE} ${NAME}

re:			fclean all

.PHONY: 	all fclean clean re