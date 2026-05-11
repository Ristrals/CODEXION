/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:44 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 17:55:13 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int	check_deadlines(t_config *config);
static int	check_compiles(t_config *config);
static int	check_deadline(t_coder *coder);
static int	check_sim_status_monitor(t_config *config);

void	monitor_script(t_config *config)
{
	while (check_sim_status_monitor(config))
	{
		if (check_deadlines(config) || check_compiles(config))
		{
			pthread_mutex_lock(&config->lock_sim_status);
			config->sim_status = 0;
			pthread_mutex_unlock(&config->lock_sim_status);
			return ;
		}
		usleep(1000);
	}
}

static int	check_deadlines(t_config *config)
{
	int		i;
	long	current_time;
	long	last_compile;

	i = 0;
	current_time = get_time();
	while (i < config->nbr_coders)
	{
		pthread_mutex_lock(&config->coders[i].lock_last_comp);
		last_compile = config->coders[i].last_comp ;
		if (check_deadline(&config->coders[i]))
		{
			sim_print(&config->coders[i], "\033[31m/!\\ BURNOUT\033[0m", 1);
			pthread_mutex_unlock(&config->coders[i].lock_last_comp);
			return (1);
		}
		pthread_mutex_unlock(&config->coders[i].lock_last_comp);
		i++;
	}
	return (0);
}

static int	check_compiles(t_config *config)
{
	int	i;
	int	compiled_reached;

	i = 0;
	compiled_reached = 0;
	while (i < config->nbr_coders)
	{
		pthread_mutex_lock(&config->coders[i].lock_compiled);
		if (config->coders[i].compiled == config->compiles_req)
			compiled_reached++;
		pthread_mutex_unlock(&config->coders[i].lock_compiled);
		i++;
	}
	if (compiled_reached == config->nbr_coders)
	{
		pthread_mutex_lock(&config->lock_write);
		printf("\033[32mJOB'S DONE\033[0m\n");
		pthread_mutex_unlock(&config->lock_write);
		return (1);
	}
	return (0);
}

static int	check_deadline(t_coder *coder)
{
	if (get_time() - coder->last_comp >= coder->config->tt_burnout)
		return (1);
	return (0);
}

static int	check_sim_status_monitor(t_config *config)
{
	int	sim_status;

	pthread_mutex_lock(&config->lock_sim_status);
	sim_status = config->sim_status;
	pthread_mutex_unlock(&config->lock_sim_status);
	return (sim_status);
}
