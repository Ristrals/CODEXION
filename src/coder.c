/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:54 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 18:22:11 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	coder_compile(t_coder *self);
static void	coder_compilation(t_coder *self);
static int	coder_ready(t_coder *self);

void	coder_script(t_coder *self)
{
	while (check_sim_status(self))
	{
		sim_print(self, "is \033[32mdebugging\033[0m", 0);
		usleep(self->config->tt_debug * 1000);
		if (!check_sim_status(self))
			break ;
		sim_print(self, "is \033[35mrefactoring\033[0m", 0);
		usleep(self->config->tt_refactor * 1000);
		if (!check_sim_status(self))
			break ;
		pthread_mutex_lock(&self->lock_req_time);
		self->req_time = get_time();
		pthread_mutex_unlock(&self->lock_req_time);
		coder_compile(self);
		pthread_mutex_lock(&self->lock_req_time);
		self->req_time = 0;
		pthread_mutex_unlock(&self->lock_req_time);
	}
}

static void	coder_compile(t_coder *self)
{
	while (check_sim_status(self))
	{
		if (coder_ready(self))
		{
			if (!pthread_mutex_trylock(&self->l_dgl->dongle))
			{
				sim_print(self, "Has taken his left dongle", 0);
				if (!pthread_mutex_trylock(&self->r_dgl->dongle))
				{
					sim_print(self, "Has taken his right dongle", 0);
					if (!dongle_cooldown(self->l_dgl, self->config->dgl_cd)
						&& !dongle_cooldown(self->r_dgl, self->config->dgl_cd))
					{
						coder_compilation(self);
						pthread_mutex_unlock(&self->l_dgl->dongle);
						pthread_mutex_unlock(&self->r_dgl->dongle);
						return ;
					}
					pthread_mutex_unlock(&self->r_dgl->dongle);
				}
				pthread_mutex_unlock(&self->l_dgl->dongle);
			}
		}
		usleep(400);
	}
}

static void	coder_compilation(t_coder *self)
{
	pthread_mutex_lock(&self->lock_last_comp);
	self->last_comp = get_time();
	pthread_mutex_unlock(&self->lock_last_comp);
	pthread_mutex_lock(&self->lock_compiled);
	self->compiled++;
	pthread_mutex_unlock(&self->lock_compiled);
	sim_print(self, "is \033[33mcompiling\033[0m", 0);
	usleep(self->config->tt_compile * 1000);
	self->r_dgl->last_released = get_time();
	self->l_dgl->last_released = get_time();
}

static int	coder_ready(t_coder *self)
{
	if (self->config->fifo_edf == 0)
	{
		if (fifo_priority(self, 0) || fifo_priority(self, 1))
			return (0);
	}
	else
	{
		if (edf_priority(self, 0) || edf_priority(self, 1))
			return (0);
	}
	return (1);
}
