/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 15:23:57 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/13 15:34:01 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	is_valid_number(char *str);
static int	is_valid_scheduler(char *str);

int	parser(int argc, char *argv[])
{
	int	i;

	i = 1;
	if (argc != 9)
		return (1);
	while (i < 8)
	{
		if (is_valid_number(argv[i]))
			return (1);
		i++;
	}
	if (is_valid_scheduler(argv[8]))
		return (1);
	return (0);
}

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str || str[i] == '\0' || str[i] == '-')
		return (1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

static int	is_valid_scheduler(char *str)
{
	if (strcmp(str, "fifo") != 0 && strcmp(str, "edf") != 0)
		return (1);
	return (0);
}
