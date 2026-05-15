/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:26:54 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/15 09:59:09 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	cleanup_coders(t_config *config);
static void	cleanup_dongles(t_config *config);

void	cleanup(t_config *config)
{
	if (config->coders)
		cleanup_coders(config);
	if (config->dongles)
		cleanup_dongles(config);
	if (config->prio_map)
		free(config->prio_map);
	pthread_mutex_destroy(&config->lock_write);
	pthread_mutex_destroy(&config->lock_sim_status);
}

static void	cleanup_coders(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->nbr_coders)
	{
		pthread_mutex_destroy(&config->coders[i].lock_state);
		pthread_mutex_destroy(&config->coders[i].lock_compiled);
		pthread_mutex_destroy(&config->coders[i].lock_last_comp);
		pthread_cond_destroy(&config->coders[i].cond_rdy);
		i++;
	}
	free(config->coders);
}

static void	cleanup_dongles(t_config *config)
{
	int	i;

	i = 0;
	while (i < config->nbr_coders)
	{
		pthread_mutex_destroy(&config->dongles[i].dongle);
		i++;
	}
	free(config->dongles);
}
