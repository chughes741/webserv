#------------------------------------------------------------------------------#
#                                  GENERICS                                    #
#------------------------------------------------------------------------------#

# Special variables
DEFAULT_GOAL: all
.DELETE_ON_ERROR: $(NAME)
.PHONY: all bonus clean fclean re segfault leak exe vcpkg

# Hide calls
HIDE = @

# Argument test
ARG = 

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

# Compiler and flags
CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -g -Wc++11-extensions -std=c++98 -I$I
SFLAGS	=	-fsanitize=address
LFLAGS	=	--leak-check=full --show-leak-kinds=all
RM		=	rm -rf

S = src/
O = obj/
I = include/

# Dir and file names
NAME	=	webserv
SRCS	=	$(wildcard $S*.cpp)
INC		=	$(wildcard $I*.hpp)
OBJS	=	$(SRCS:$S%.cpp=$O%.o)

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
$(OBJS): $O%.o: $S%.cpp $(INC)
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
	./$(NAME) $(ARG)

leak: $(NAME)
	valgrind $(LFLAGS) ./$(NAME) $(ARG)

segfault: $(OBJ)
	$(HIDE) $(CC) $(CFLAGS) $(SFLAGS) $(OBJS) -o $(NAME)

# Installs vcpkg and gtest, doesn't actually work, run it manually
vcpkg:
	./vcpkg/bootstrap-vcpkg.sh
	./vcpkg/vcpkg install gtest
