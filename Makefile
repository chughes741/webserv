#------------------------------------------------------------------------------#
#                                  GENERICS                                    #
#------------------------------------------------------------------------------#

# Special variables
DEFAULT_GOAL: all
.DELETE_ON_ERROR: $(NAME)
.PHONY: all bonus clean fclean re

# Hide calls
HIDE = @

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

# Compiler and flags
CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -Wc++11-extensions -std=c++98 -I$I
RM		=	rm -rf

S = src/
O = obj/
I = include/

# Dir and file names
NAME	=	webserv
SRCS	=	Exception.cpp server.cpp events.cpp webserv.cpp main.cpp
INC		=	Exception.hpp server.hpp events.hpp webserv.hpp
OBJS	=	$(SRCS:%=$O%.o)

#------------------------------------------------------------------------------#
#                                 TARGETS                                      #
#------------------------------------------------------------------------------#

all: $(NAME)

# Generates output file
$(NAME): $(OBJS)
	$(HIDE)$(CC) $(CFLAGS) $(OBJS) -o $@

#Create objects directory
$O:
	@mkdir $@
$(OBJS): | $O
# Compiles sources into objects
$(OBJS): $O%.o: $S%
	@$(CC) $(CFLAGS) -c $< -o $@

# Removes objects
clean:
	$(HIDE)$(RM) $O
	$(HIDE)$(RM) $(OBJS)

# Removes objects and executables
fclean: clean
	$(HIDE)$(RM) $(NAME)

# Removes objects and executables and remakes
re: fclean all

exe: $(NAME)
	./$(NAME) webserv.conf
