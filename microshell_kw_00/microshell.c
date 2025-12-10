/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:15:57 by fmoulin           #+#    #+#             */
/*   Updated: 2025/12/10 15:27:08 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_error(char *msg)
{
	write(2, msg, ft_strlen(msg));
	exit(1);
}

void	ft_error2(char *msg)
{
	write(2, msg, ft_strlen(msg));
}

int	exec_cd(char **argv, int i)
{
	if (i != 2)
		ft_error2("error: cd: bad arguments\n");
	else if (chdir(argv[0]) == -1)
	{
		ft_error2("error: cd: bad arguments\n");
		ft_error2(argv[0]);
		ft_error2("\n");
	}
	return (0);
}

int	exec_cmd(char **argv, int i, char **envp)
{
	int	fd[2];
	int	pid;
	int pipe_needed;

	pipe_needed = (argv[i] && strcmp(argv[i], "|") == 0);
	if (pipe(fd) == -1)
		ft_error("error: fatal\n");
	pid = fork();
	if (pid < 0)
		ft_error("error: fatal\n");
	if (pid == 0)
	{
		if (pipe_needed)
		{
			if (dup2(fd[1], 1) == -1)
				ft_error("error: fatal\n");
		}
		if (close(fd[1]) == -1)
			ft_error("error: fatal\n");
		if (close(fd[0]) == -1)
			ft_error("error: fatal\n");
		argv[i] = NULL;
		execve(argv[0], argv, envp);
		ft_error2("error: cannot execute ");
		ft_error2(argv[0]);
		ft_error2("\n");
		exit(1);
	}
	if (dup2(fd[0], 0) == -1)
		ft_error("error: fatal\n");
	if (close(fd[1]) == -1)
		ft_error("error: fatal\n");
	if (close(fd[0]) == -1)
		ft_error("error: fatal\n");
	if (waitpid(pid, NULL, 0) == -1)
		ft_error("error: fatal\n");
	return (0);
}

int main(int argc, char **argv, char **envp)
{
	(void)argc;

	int	i;

	i = 0;
	while (argv[i] && argv[++i])
	{
		argv = argv + i;
		i = 0;
		while (argv[i] && strcmp(argv[i], ";") == 0)
		{
			argv = argv + 1;
		}
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
		{
			i++;
		}
		if (argv[0])
		{
			if (strcmp(argv[0], "cd") == 0)
				exec_cd(argv, i);
			else
				exec_cmd(argv, i, envp);
		}
	}
	return (0);
}
