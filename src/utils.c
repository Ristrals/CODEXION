/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 14:20:52 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/19 12:00:21 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

long long	get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL))
		return (0);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000L));
}

void	sim_print(t_coder *self, char *msg, int critical)
{
	int	time;

	pthread_mutex_lock(&self->config->lock_write);
	time = get_time() - self->config->time_start;
	pthread_mutex_lock(&self->config->lock_sim_status);
	if (self->config->sim_status == 1 || critical)
		printf("\033[36m%d\033[0m - Coder %d %s\n", time, self->id, msg);
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

int	compare_fifo(t_coder *coder0, t_coder *coder1)
{
	long long	time0;
	long long	time1;

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
	long long	deadline0;
	long long	score0;
	long long	deadline1;
	long long	score1;

	pthread_mutex_lock(&coder0->lock_last_comp);
	deadline0 = coder0->last_comp + coder0->config->tt_burnout;
	pthread_mutex_unlock(&coder0->lock_last_comp);
	pthread_mutex_lock(&coder1->lock_last_comp);
	deadline1 = coder1->last_comp + coder1->config->tt_burnout;
	pthread_mutex_unlock(&coder1->lock_last_comp);
	score0 = deadline0 + (coder0->compiled * 10);
	score1 = deadline1 + (coder1->compiled * 10);
	if (score0 < score1)
		return (1);
	if (score0 == score1 && coder0->id < coder1->id)
		return (1);
	return (0);
}
