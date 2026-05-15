/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 13:39:54 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/15 09:48:32 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static void	coder_compile(t_coder *self);

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
		while (self->state == REQ && check_sim_status(self->config))
			pthread_cond_wait(&self->cond_rdy, &self->lock_state);
		pthread_mutex_unlock(&self->lock_state);
		coder_compile(self);
		usleep(50);
	}
}

static void	coder_compile(t_coder *self)
{
	pthread_mutex_lock(&self->l_dgl->dongle);
	sim_print(self, "has taken his \033[31mleft\033[0m dongle", STND);
	pthread_mutex_lock(&self->r_dgl->dongle);
	sim_print(self, "has taken his \033[34mright\033[0m dongle", STND);
	pthread_mutex_lock(&self->lock_last_comp);
	self->last_comp = get_time();
	pthread_mutex_unlock(&self->lock_last_comp);
	pthread_mutex_lock(&self->lock_compiled);
	self->compiled++;
	pthread_mutex_unlock(&self->lock_compiled);
	sim_print(self, "is \033[33mcompiling\033[0m", STND);
	usleep(self->config->tt_compile * 1000);
	self->r_dgl->last_used = get_time();
	self->l_dgl->last_used = get_time();
	pthread_mutex_unlock(&self->l_dgl->dongle);
	pthread_mutex_unlock(&self->r_dgl->dongle);
	pthread_mutex_lock(&self->lock_state);
	self->r_dgl->in_use = 0;
	self->l_dgl->in_use = 0;
	self->state = WORK;
	pthread_mutex_unlock(&self->lock_state);
}
