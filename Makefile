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

#------------------------------------------------------------------------------#
#                                 TARGETS                                      #
#------------------------------------------------------------------------------#

all: $(NAME)

# Generates output file
$(NAME): $(OBJS)
	$(HIDE)$(CC) $(CFLAGS) $(VFLAGS) $(OBJS) -o $@

#Create objects directory
$O:
	@mkdir $@
$(OBJS): | $O
# Compiles sources into objects
$(OBJS): $O%.o: $S%.cpp $(INC)
	@$(CC) $(CFLAGS) $(VFLAGS) -c $< -o $@

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


# Git merging procedure
# First, ensure that you are working with the most up-to-date version of the main branch
# 	by running the command git fetch followed by git pull origin main.
# 	This will ensure that you have all the latest changes made by other contributors in the main branch.
# Switch to the branch that you want to merge into the main branch by running
#	the command git checkout [branch-name].
# Once you are on the branch you want to merge, run git merge main.
#	This will merge any changes made in the main branch into your current branch.
# Next, resolve any merge conflicts that may arise. If there are any conflicting changes
#	between your branch and the main branch, Git will alert you and prompt you to manually
#	resolve the conflicts. You can use a merge tool such as meld or vimdiff to help you resolve the conflicts.
# Once all merge conflicts are resolved, commit the changes to your branch by running
# 	git commit -m "Merged main branch into [branch-name]".
# Finally, create a pull request
