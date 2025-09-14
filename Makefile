NAME := ircserv
CC := c++
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c++98 -g3

SRCDIR := sources
INCDIR := includes
BUILDDIR := build

INCLUDES := $(shell find $(INCDIR) -name "*.hpp")

SRCS := $(shell find $(SRCDIR) -name "*.cpp")

OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
