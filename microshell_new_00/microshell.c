/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 15:24:06 by fmoulin           #+#    #+#             */
/*   Updated: 2025/12/09 16:45:42 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "microshell.h"

void	ft_putstr_fd(char *str, char *argv)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(2, &str[i], 1);
		i++;
	}
	if (argv)
	{
		i = 0;
		while (argv[i])
		{
			write(2, &argv[i], 1);
			i++;
		}
	}
	write(2, "\n", 1);
}

void ft_exec(char **argv, int i, int tmp_fd, char **envp)
{
	argv[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(argv[0], argv, envp);
	ft_putstr_fd("error: cannot execute ", argv[0]);
	exit(1);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	int		i;
	int		fd[2];
	int		tmp_fd;
	int		pid;
	
	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				ft_putstr_fd("error: cd: bad arguments", NULL);
			if (chdir(argv[1]) == -1)
				ft_putstr_fd("error: cd: cannot change directory to ", argv[1]);
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
		{
			pid = fork();
			if (pid < 0)
				ft_putstr_fd("error: fatal", NULL);
			if (pid == 0)
				ft_exec(argv, i, tmp_fd, envp);
			else
			{
				close(tmp_fd);
				waitpid(pid, NULL, 0);
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], "|") == 0))
		{
			if (pipe(fd) == -1)
				ft_putstr_fd("error: fatal", NULL);
			pid = fork();
			if (pid < 0)
				ft_putstr_fd("error: fatal", NULL);
			if (pid == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				ft_exec(argv, i, tmp_fd, envp);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return (0);
}
