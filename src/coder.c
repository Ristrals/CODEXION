/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:54 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/18 19:49:22 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void		coder_compile(t_coder *self);
static t_dongle	*get_first_dgl(t_coder *self);
static t_dongle	*get_second_dgl(t_coder *self);

void	coder_script(t_coder *self)
{
	while (check_sim_status(self->config))
	{
		sim_print(self, "is \033[32mdebugging\033[0m", STND);
		usleep(self->config->tt_debug * 1000);
		if (!check_sim_status(self->config))
			break ;
		sim_print(self, "is \033[35mrefactoring\033[0m", STND);
		usleep(self->config->tt_refactor * 1000);
		if (!check_sim_status(self->config))
			break ;
		pthread_mutex_lock(&self->lock_state);
		self->req_time = get_time();
		self->state = REQ;
		pthread_mutex_unlock(&self->lock_state);
		pthread_mutex_lock(&self->config->lock_room);
		while (self->state != COMP && check_sim_status(self->config))
			pthread_cond_wait(&self->config->cond_room,
				&self->config->lock_room);
		pthread_mutex_unlock(&self->config->lock_room);
		if (!check_sim_status(self->config))
			break ;
		coder_compile(self);
		usleep(50);
	}
}

static void	coder_compile(t_coder *self)
{
	t_dongle	*first;
	t_dongle	*second;

	first = get_first_dgl(self);
	second = get_second_dgl(self);
	pthread_mutex_lock(&self->lock_last_comp);
	self->last_comp = get_time();
	pthread_mutex_unlock(&self->lock_last_comp);
	pthread_mutex_lock(&self->lock_compiled);
	self->compiled++;
	pthread_mutex_unlock(&self->lock_compiled);
	sim_print(self, "is \033[33mcompiling\033[0m", STND);
	usleep(self->config->tt_compile * 1000);
	first->last_used = get_time();
	second->last_used = get_time();
	first->in_use = 0;
	second->in_use = 0;
	pthread_mutex_unlock(&first->dongle);
	pthread_mutex_unlock(&second->dongle);
	pthread_mutex_lock(&self->lock_state);
	self->state = WORK;
	pthread_mutex_unlock(&self->lock_state);
}

static t_dongle	*get_first_dgl(t_coder *self)
{
	if (self->l_dgl->id < self->r_dgl->id)
	{
		pthread_mutex_lock(&self->l_dgl->dongle);
		sim_print(self, "has taken his \033[31mleft\033[0m dongle", STND);
		return (self->l_dgl);
	}
	else
	{
		pthread_mutex_lock(&self->r_dgl->dongle);
		sim_print(self, "has taken his \033[34mright\033[0m dongle", STND);
		return (self->r_dgl);
	}
}

static t_dongle	*get_second_dgl(t_coder *self)
{
	if (self->l_dgl->id > self->r_dgl->id)
	{
		pthread_mutex_lock(&self->l_dgl->dongle);
		sim_print(self, "has taken his \033[31mleft\033[0m dongle", STND);
		return (self->l_dgl);
	}
	else
	{
		pthread_mutex_lock(&self->r_dgl->dongle);
		sim_print(self, "has taken his \033[34mright\033[0m dongle", STND);
		return (self->r_dgl);
	}
}
