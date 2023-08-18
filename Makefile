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
EXECUTION = ./$(NAME) or ./$(NAME) + configFile

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

# Compiler and flags
CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -g -std=c++98 -I$I -DCONFIG_FILE="\"./config/server.conf\""
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

LOG_DIR = logs

#------------------------------------------------------------------------------#
#                                 TARGETS                                      #
#------------------------------------------------------------------------------#

all: $(NAME) | $(LOG_DIR)

# Generates output file
$(NAME): $(OBJS)
	$(HIDE)$(CC) $(CFLAGS) $(VFLAGS) $(OBJS) -o $@
	@echo "\033[0;32mCompiled! Execute as: $(EXECUTION)\033[0m"

#Create objects directory
$O:
	@mkdir $@
	@echo "\033[0;32mCompiling $(NAME)...\033[0m"

$(OBJS): | $O
# Compiles sources into objects
$(OBJS): $O%.o: $S%.cpp $(INC)
	@$(CC) $(CFLAGS) $(VFLAGS) -c $< -o $@

# Removes objects
clean:
	$(HIDE)$(RM) $O
	$(HIDE)$(RM) $(OBJS)
	@echo "\033[0;31mObjects deleted!\033[0m"

# Removes objects and executables
fclean: clean
	$(HIDE)$(RM) $(NAME)
	@echo "\033[0;31mExecutable deleted!\033[0m"

# Removes objects and executables and remakes
re: fclean all

$(LOG_DIR):
	@mkdir -p $@
	@echo "\033[0;32mCreated logs directory!\033[0m"

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
