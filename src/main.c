/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:36:28 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/13 17:14:15 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	start_sim(t_config *config);
static void	end_sim(t_config *config);
static void	report(t_config *config);

int	main(int argc, char *argv[])
{
	t_config	config;

	if (parser(argc, argv))
	{
		printf("\033[31m/!\\ Invalid arguments, exiting program\033[0m\n");
		return (1);
	}
	if (init_config(&config, argv))
	{
		printf("\033[31m/!\\ Initialization error, exiting program\033[0m\n");
		cleanup(&config);
		return (1);
	}
	if (start_sim(&config))
	{
		printf("\033[31m/!\\ Start up error, exiting program\033[0m\n");
		cleanup(&config);
		return (1);
	}
	end_sim(&config);
	report(&config);
	cleanup(&config);
	return (0);
}

static int	start_sim(t_config *config)
{
	int	i;

	i = 0;
	config->sim_status = 1;
	config->time_start = get_time();
	while (i < config->nbr_coders)
	{
		config->coders[i].last_comp = config->time_start;
		i++;
	}
	if (pthread_create(&config->monitor, NULL,
			(void *)monitor_script, config) != 0)
		return (1);
	i = 0;
	while (i < config->nbr_coders)
	{
		if (pthread_create(&config->coders[i].thread, NULL,
				(void *)coder_script, &config->coders[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

static void	end_sim(t_config *config)
{
	int	i;

	if (config->monitor)
		pthread_join(config->monitor, NULL);
	i = 0;
	while (i < config->nbr_coders)
	{
		if (config->coders[i].thread)
			pthread_join(config->coders[i].thread, NULL);
		i++;
	}
}

static void	report(t_config *config)
{
	int		i;
	double	now;

	i = 0;
	now = get_time();
	printf("\nExecution time: %.3fs",
		((now - (double)config->time_start) / 1000));
	while (i < config->nbr_coders)
	{
		printf("\nCoder %d > %d compilations",
			config->coders[i].id, config->coders[i].compiled);
		i++;
	}
}


