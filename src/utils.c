/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:20:52 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 17:59:07 by kmalfois         ###   ########.fr       */
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

t_coder	*get_neighbor(t_coder *coder, int side)
{
	int	nghbr_id;
	int	nc;

	nghbr_id = 0;
	nc = coder->config->nbr_coders;
	if (side == 0)
		nghbr_id = (coder->id - 2 + nc) % nc;
	else if (side == 1)
		nghbr_id = coder->id % nc;
	return (&coder->config->coders[nghbr_id]);
}

int	check_sim_status(t_coder *self)
{
	int	sim_status;

	pthread_mutex_lock(&self->config->lock_sim_status);
	sim_status = self->config->sim_status;
	pthread_mutex_unlock(&self->config->lock_sim_status);
	pthread_mutex_lock(&self->lock_compiled);
	if (self->compiled >= self->config->compiles_req)
	{
		pthread_mutex_unlock(&self->lock_compiled);
		return (0);
	}
	pthread_mutex_unlock(&self->lock_compiled);
	return (sim_status);
}
