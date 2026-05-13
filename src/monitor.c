/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:44 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/13 18:14:43 by kmalfois         ###   ########.fr       */
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

	i = 0;
	while (check_sim_status(config))
	{
		if (check_deadlines(config) || check_compiles(config))
		{
			pthread_mutex_lock(&config->lock_sim_status);
			config->sim_status = 0;
			pthread_mutex_unlock(&config->lock_sim_status);

			while (i < config->nbr_coders)
			{
				pthread_mutex_lock(&config->coders[i].lock_state);
				pthread_cond_signal(&config->coders[i].cond_rdy);
				pthread_mutex_unlock(&config->coders[i].lock_state);
				i++;
			}
			return ;
		}
		sort_map_priority(config);
		execute_map_priority(config);
		usleep(50);
	}
}

static void	sort_map_priority(t_config *config)
{
	int	i;
	int	j;
	int prio;
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
	int		nc;
	int		left;
	int		right;
	t_coder	*target;

	i = 0;
	nc = config->nbr_coders;
	while (i < nc)
	{
		target = config->prio_map[i];
		pthread_mutex_lock(&target->lock_state);
		if (target->state == REQ)
		{
			left = target->id - 1;
			right = (target->id) % nc;
			execution(config, target, left, right);
		}
		pthread_mutex_unlock(&target->lock_state);
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

	now = get_time();
	if (!config->dongles[left].in_use
		&& !config->dongles[right].in_use)
	{
		if ((now - config->dongles[left].last_used >= config->dgl_cd)
				&& (now - config->dongles[right].last_used >= config->dgl_cd))
		{
			coder->state = COMP;
			config->dongles[left].in_use = 1;
			config->dongles[right].in_use = 1;
			pthread_cond_signal(&coder->cond_rdy);
		}
	}
}