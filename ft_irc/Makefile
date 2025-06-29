NAME := ircserv

CC := c++

FLAGS := -Wall -Wextra -Werror -std=c++98

OBJ_DIR := obj

CPPFILES := main.cpp server/Server.cpp client/Client.cpp

OBJFILES := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(CPPFILES))

all: $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJFILES)
	$(CC) $(FLAGS) $(OBJFILES) -o $(NAME)

v:
	valgrind --leak-check=full ./$(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all v clean fclean re