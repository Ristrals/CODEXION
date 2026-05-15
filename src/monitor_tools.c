/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 13:24:15 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/15 15:52:09 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	check_deadlines(t_config *config)
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

int	compare_fifo(t_coder *coder0, t_coder *coder1)
{
	long	time0;
	long	time1;

	pthread_mutex_lock(&coder0->lock_state);
	time0 = coder0->req_time;
	pthread_mutex_unlock(&coder0->lock_state);
	pthread_mutex_lock(&coder1->lock_state);
	time1 = coder1->req_time;
	pthread_mutex_unlock(&coder1->lock_state);
	if (time0 < time1)
		return (1);
	if (time0 == time1 && coder0->id < coder1->id)
		return (1);
	return (0);
}

int	compare_edf(t_coder *coder0, t_coder *coder1)
{
	long	deadline0;
	long	deadline1;

	pthread_mutex_lock(&coder0->lock_last_comp);
	deadline0 = coder0->last_comp + coder0->config->tt_burnout;
	pthread_mutex_unlock(&coder0->lock_last_comp);
	pthread_mutex_lock(&coder1->lock_last_comp);
	deadline1 = coder1->last_comp + coder1->config->tt_burnout;
	pthread_mutex_unlock(&coder1->lock_last_comp);
	if (deadline0 < deadline1)
		return (1);
	if (deadline0 == deadline1 && coder0->id < coder1->id)
		return (1);
	return (0);
}
