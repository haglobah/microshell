
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

/*not needed in exam, but necessary if you want to use this tester:
https://github.com/Glagan/42-exam-rank-04/blob/master/microshell/test.sh*/
// #ifdef TEST_SH
// # define TEST		1
// #else
// # define TEST		0
// #endif

void	ft_putstr_fd2(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

int ft_execute(char *argv[], int i, int tmp_fd, char *env[])
{
	argv[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(argv[0], argv, env);
	return (ft_putstr_fd2("error: cannot execute ", argv[0]), 1);
}

void	exec_cd(char **argv, int i)
{
	if (i != 2)
		ft_putstr_fd2("error: cd: bad arguments", NULL);
	else if (chdir(argv[1]) != 0)
		ft_putstr_fd2("error: cd: cannot change directory to ", argv[1]);
}

void	exec_stdout(char **argv, int i, int *tmp_fd, char **env)
{
	if ( fork() == 0)
	{
		if(ft_execute(argv, i, *tmp_fd, env))
			exit(EXIT_FAILURE);
	}
	else
	{
		close(*tmp_fd);
		waitpid(-1, NULL, WUNTRACED);
		*tmp_fd = dup(STDIN_FILENO);
	}
}

void	exec_pipe(char **argv, int i, int *tmp_fd, char **env)
{
	int	fd[2];

	pipe(fd);
	if ( fork() == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		if(ft_execute(argv, i, *tmp_fd, env))
			exit(EXIT_FAILURE);
	}
	else
	{
		close(fd[1]);
		close(*tmp_fd);
		*tmp_fd = fd[0];
	}
}

void	exec_cmd(char **argv, int i, int *tmp_fd, char **env)
{
	if (strcmp(argv[0], "cd") == 0) //cd
		exec_cd(argv, i);
	else if (i == 0)
		return ;
	else if (argv[i] == NULL || strcmp(argv[i], ";") == 0) //exec in stdout
		exec_stdout(argv, i, tmp_fd, env);
	else if (strcmp(argv[i], "|") == 0) //pipe
		exec_pipe(argv, i, tmp_fd, env);
}

int	main(int argc, char *argv[], char *env[])
{
	int	i;
	int tmp_fd;
	(void)argc;	// is needed in exam, because the exam tester compiles with -Wall -Wextra -Werror

	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	while (argv[i] && argv[i + 1]) //check if the end is reached
	{
		argv = &argv[i + 1];	//the new argv start after the ; or |
		i = -1;
		while (argv[++i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			;
		exec_cmd(argv, i, &tmp_fd, env);
	}
	close(tmp_fd);
	// if (TEST)		// not needed in exam, but necessary if you want to use this tester:
	// 	while (1);	// https://github.com/Glagan/42-exam-rank-04/blob/master/microshell/test.sh
	return (0);
}