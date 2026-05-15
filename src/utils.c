/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:20:52 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/15 16:00:02 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long	get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL))
		return (0);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000L));
}

void	sim_print(t_coder *self, char *msg, int critical)
{
	long	time;

	pthread_mutex_lock(&self->config->lock_write);
	time = get_time() - self->config->time_start;
	pthread_mutex_lock(&self->config->lock_sim_status);
	if (self->config->sim_status == 1 || critical)
		printf("\033[36m%ld\033[0m - Coder %d %s\n", time, self->id, msg);
	pthread_mutex_unlock(&self->config->lock_sim_status);
	pthread_mutex_unlock(&self->config->lock_write);
}

int	check_sim_status(t_config *config)
{
	int	sim_status;

	pthread_mutex_lock(&config->lock_sim_status);
	sim_status = config->sim_status;
	pthread_mutex_unlock(&config->lock_sim_status);
	return (sim_status);
}
