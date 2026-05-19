/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:44 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/19 10:33:26 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	sort_map_priority(t_config *config);
static void	execute_map_priority(t_config *config);
static int	compare(t_coder *coder0, t_coder *coder1, int fifo_edf);
static void	execution(t_config *config, t_coder *coder, int left, int right);

void	monitor_script(t_config *config)
{
	int	i;

	while (check_sim_status(config))
	{
		i = 0;
		if (check_deadlines(config) || check_compiles(config))
		{
			pthread_mutex_lock(&config->lock_sim_status);
			config->sim_status = 0;
			pthread_mutex_unlock(&config->lock_sim_status);
			pthread_mutex_lock(&config->lock_room);
			pthread_cond_broadcast(&config->cond_room);
			pthread_mutex_unlock(&config->lock_room);
			return ;
		}
		sort_map_priority(config);
		execute_map_priority(config);
		usleep(50);
	}
}

static void	sort_map_priority(t_config *config)
{
	int		i;
	int		j;
	int		prio;
	t_coder	*tmp;

	i = 0;
	while (i < config->nbr_coders - 1)
	{
		prio = i;
		j = i + 1;
		while (j < config->nbr_coders)
		{
			if (compare(config->prio_map[j], config->prio_map[prio],
					config->fifo_edf))
				prio = j;
			j++;
		}
		if (prio != i)
		{
			tmp = config->prio_map[i];
			config->prio_map[i] = config->prio_map[prio];
			config->prio_map[prio] = tmp;
		}
		i++;
	}
}

static void	execute_map_priority(t_config *config)
{
	int		i;
	int		request;
	int		left;
	int		right;
	t_coder	*target;

	i = 0;
	while (i < config->nbr_coders)
	{
		target = config->prio_map[i];
		pthread_mutex_lock(&target->lock_state);
		request = (target->state == REQ);
		pthread_mutex_unlock(&target->lock_state);
		if (request)
		{
			left = target->id - 1;
			right = (target->id) % config->nbr_coders;
			execution(config, target, left, right);
		}
		i++;
	}
}

static int	compare(t_coder *coder0, t_coder *coder1, int fifo_edf)
{
	int	result;

	if (fifo_edf == FIFO)
		result = compare_fifo(coder0, coder1);
	else
		result = compare_edf(coder0, coder1);
	return (result);
}

static void	execution(t_config *config, t_coder *coder, int left, int right)
{
	long long	now;
	int			first;
	int			second;

	if (left < right)
	{
		first = left;
		second = right;
	}
	else
	{
		first = right;
		second = left;
	}
	now = get_time();
	if (check_dongles(config, now, first, second))
		greenlight_coder(config, coder, first, second);
}
