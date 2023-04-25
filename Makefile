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
		src/CGI.cpp\

OBJ	= $(SRC:.cpp=.o)

CC = c++ -std=c++98 -g3 -fsanitize=address
CFLAGS = -Wall -Wextra -Werror 

all: $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all 

.PHONY : all clean fclean re