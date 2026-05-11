/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:26:54 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 18:03:10 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	fifo_priority(t_coder *self, int side)
{
	int		prio;
	t_coder	*nghbr;

	prio = 0;
	nghbr = get_neighbor(self, side);
	if (self->id > nghbr->id)
	{
		pthread_mutex_lock(&self->lock_req_time);
		pthread_mutex_lock(&nghbr->lock_req_time);
	}
	else
	{
		pthread_mutex_lock(&nghbr->lock_req_time);
		pthread_mutex_lock(&self->lock_req_time);
	}
	if (nghbr->req_time > 0 && self->req_time > nghbr->req_time)
		prio = 1;
	else if (nghbr->req_time > 0 && self->req_time == nghbr->req_time)
	{
		if (nghbr->id < self->id)
			prio = 1;
	}
	pthread_mutex_unlock(&self->lock_req_time);
	pthread_mutex_unlock(&nghbr->lock_req_time);
	return (prio);
}

int	edf_priority(t_coder *self, int side)
{
	int		prio;
	t_coder	*nghbr;

	prio = 0;
	nghbr = get_neighbor(self, side);
	if (self->id > nghbr->id)
	{
		pthread_mutex_lock(&self->lock_last_comp);
		pthread_mutex_lock(&nghbr->lock_last_comp);
	}
	else
	{
		pthread_mutex_lock(&nghbr->lock_last_comp);
		pthread_mutex_lock(&self->lock_last_comp);
	}
	if (self->last_comp > nghbr->last_comp)
		prio = 1;
	else if (self->last_comp == nghbr->last_comp)
	{
		if (nghbr->id < self->id)
			prio = 1;
	}
	pthread_mutex_unlock(&self->lock_last_comp);
	pthread_mutex_unlock(&nghbr->lock_last_comp);
	return (prio);
}

int	dongle_cooldown(t_dongle *dongle, int cooldown)
{
	long	now;

	now = get_time();
	if (now < dongle->last_released + cooldown)
		return (1);
	return (0);
}
