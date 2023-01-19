
NAME := microshell
FLAGS := -Wall -Wextra -Werror

all :
	@gcc $(FLAGS) microshell.c -o $(NAME)

run :
	@$(MAKE) all
	@./$(NAME) /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo Hey!