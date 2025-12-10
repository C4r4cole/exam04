/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmoulin <fmoulin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 15:44:32 by fmoulin           #+#    #+#             */
/*   Updated: 2025/12/09 12:11:19 by fmoulin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "microshell.h"

int	ft_error(char *msg)
{
	write(2, msg, strlen(msg));
	return (1);
}

int	exec_cd (char **argv, int size)
{
	if (size != 2)
		return (ft_error("error: cd: bad arguments\n"));
	if (chdir(argv[1]) == -1)
	{
		ft_error("error: cd: cannot change directory to ");
		ft_error(argv[1]);
		ft_error("\n");
		return (1);
	}
	return (0);
}

int exec_cmd(char **argv, int size, char **envp, int *saved_stdin)
{
	int	pipe_needed;
	int	fd[2];
	int	pid;

	pipe_needed = (argv[size] && strcmp(argv[size], "|") == 0);
	argv[size] = NULL;
	if (pipe_needed && pipe(fd) == -1)
		return (ft_error("error: fatal\n"));
	pid = fork();
	if (pid < 0)
		return (ft_error("error: fatal\n"));
	if (pid == 0)
	{
		dup2(*saved_stdin, 0);
		close(*saved_stdin);
		if (pipe_needed)
		{
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
		}
		execve(argv[0], argv, envp);
		ft_error("error: cannot execute ");
		ft_error(argv[0]);
		ft_error("\n");
		exit(1);
	}
	waitpid(pid, NULL, 0);
	if (pipe_needed)
	{
		close(fd[1]);
		dup2(fd[0], *saved_stdin);
		close(fd[0]);
	}
	else
	{
		dup2(*saved_stdin, 0);
	}
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	int 	saved_stdin;
	int 	i;
	char 	**cmd;
	int		size;

	saved_stdin = dup(0);
	i = 1;
	while(argv[i])
	{
		cmd = &argv[i];
		size = 0;
		
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
		{
			i++;
			size++;
		}
		if (size)
		{
			if (strcmp(cmd[0], "cd") == 0)
				exec_cd(cmd, size);
			else
				exec_cmd(cmd, size, envp, &saved_stdin);
		}
		if (argv[i])
			i++;
	}
	close(saved_stdin);
	return (0);
}
