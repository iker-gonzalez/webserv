NAME = web_server

SRC = src/main.cpp\
		src/CommonInfo.cpp\
		src/ConfigFile.cpp\
		src/Location.cpp\
		src/Server.cpp\
		src/Utils.cpp\
		src/Request.cpp\
		src/ServerManager.cpp\
		src/Client.cpp\
		src/Response.cpp\
		src/CGI.cpp
		#tests/non_blocking_server.cpp

OBJ	= $(SRC:.cpp=.o)


CC = g++ -std=c++11 -g -fsanitize=address
CFLAGS = -Wall -Wextra -Werror 

all : $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $(NAME)

%.o	: %.c 
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	 rm -f $(OBJ)
	#no descomentar porque borra los .cpp

fclean: 
	rm -f $(NAME)

re: fclean all 

.PHONY : all clean fclean re bonus