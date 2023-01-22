NAME = web_server

SRC = main.cpp\
		CommonInfo.cpp\
		ConfigFile.cpp\
		Location.cpp\
		Server.cpp\
		Utils.cpp\

OBJ	= $(SRC:.c=.o)


CC = g++
CFLAGS = -Wall -Wextra -Werror 

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $(NAME)

%.o	: %.c 
	$(CC) $(CFLAGS) -c $< -o $@


fclean: clean

	rm -f $(SERVER) $(CLIENT)

re: fclean all 

.PHONY : all clean fclean re bonus