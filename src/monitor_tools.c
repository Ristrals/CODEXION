/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:24:15 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/19 10:39:49 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	check_deadlines(t_config *config)
{
	int			i;
	long long	current_time;
	long long	last_compile;

	i = 0;
	current_time = get_time();
	while (i < config->nbr_coders)
	{
		pthread_mutex_lock(&config->coders[i].lock_last_comp);
		last_compile = config->coders[i].last_comp ;
		pthread_mutex_unlock(&config->coders[i].lock_last_comp);
		if (current_time - last_compile
			>= config->coders[i].config->tt_burnout)
		{
			sim_print(&config->coders[i], "\033[31m/!\\ BURNOUT\033[0m", CRIT);
			return (1);
		}
		i++;
	}
	return (0);
}

int	check_compiles(t_config *config)
{
	int	i;
	int	compiled_reached;

	i = 0;
	compiled_reached = 0;
	while (i < config->nbr_coders)
	{
		pthread_mutex_lock(&config->coders[i].lock_compiled);
		if (config->coders[i].compiled >= config->compiles_req)
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

void	greenlight_coder(t_config *config, t_coder *coder,
			int first, int second)
{
	pthread_mutex_lock(&config->lock_room);
	pthread_mutex_lock(&coder->lock_state);
	coder->state = COMP;
	pthread_mutex_unlock(&coder->lock_state);
	config->dongles[first].in_use = 1;
	config->dongles[second].in_use = 1;
	pthread_cond_broadcast(&config->cond_room);
	pthread_mutex_unlock(&config->lock_room);
}

int	check_dongles(t_config *config, long long now, int first, int second)
{
	int	result;

	result = 0;
	pthread_mutex_lock(&config->dongles[first].dongle);
	pthread_mutex_lock(&config->dongles[second].dongle);
	if (!config->dongles[first].in_use && !config->dongles[second].in_use)
	{
		if ((now - config->dongles[first].last_used >= config->dgl_cd) && (now
				- config->dongles[second].last_used >= config->dgl_cd))
			result = 1;
	}
	pthread_mutex_unlock(&config->dongles[second].dongle);
	pthread_mutex_unlock(&config->dongles[first].dongle);
	return (result);
}
