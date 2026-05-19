/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:59:27 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/18 18:10:32 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int		init_arrays(t_config *config);
static t_dongle	*init_dongles(int nbr_coders);
static t_coder	*init_coders(t_config *config, int nbr_coders);
static t_coder	**init_prio_map(t_config *config, int nbr_coders);

int	init_config(t_config *config, char *argv[])
{
	config->dongles = NULL;
	config->coders = NULL;
	config->nbr_coders = atoi(argv[1]);
	config->tt_burnout = atoi(argv[2]);
	config->tt_compile = atoi(argv[3]);
	config->tt_debug = atoi(argv[4]);
	config->tt_refactor = atoi(argv[5]);
	config->compiles_req = atoi(argv[6]);
	config->dgl_cd = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		config->fifo_edf = FIFO;
	else
		config->fifo_edf = EDF;
	config->time_start = 0;
	config->sim_status = 0;
	pthread_mutex_init(&config->lock_sim_status, NULL);
	pthread_mutex_init(&config->lock_write, NULL);
	pthread_cond_init(&config->cond_room, NULL);
	pthread_mutex_init(&config->lock_room, NULL);
	if (init_arrays(config))
		return (1);
	return (0);
}

static t_dongle	*init_dongles(int nbr_coders)
{
	int			i;
	t_dongle	*dongle_arr;

	i = 0;
	dongle_arr = malloc(sizeof(t_dongle) * nbr_coders);
	if (!dongle_arr)
		return (NULL);
	while (i < nbr_coders)
	{
		pthread_mutex_init(&dongle_arr[i].dongle, NULL);
		dongle_arr[i].id = i + 1;
		dongle_arr[i].in_use = 0;
		dongle_arr[i].last_used = 0;
		i++;
	}
	return (dongle_arr);
}

static t_coder	*init_coders(t_config *config, int nbr_coders)
{
	int		i;
	t_coder	*coder_arr;

	i = 0;
	coder_arr = malloc(sizeof(t_coder) * nbr_coders);
	if (!coder_arr)
		return (NULL);
	while (i < nbr_coders)
	{
		coder_arr[i].id = i + 1;
		coder_arr[i].state = WORK;
		coder_arr[i].req_time = 0;
		pthread_mutex_init(&coder_arr[i].lock_state, NULL);
		coder_arr[i].compiled = 0;
		pthread_mutex_init(&coder_arr[i].lock_compiled, NULL);
		coder_arr[i].last_comp = 0;
		pthread_mutex_init(&coder_arr[i].lock_last_comp, NULL);
		coder_arr[i].config = config;
		coder_arr[i].l_dgl = &config->dongles[i];
		coder_arr[i].r_dgl = &config->dongles[(i + 1) % nbr_coders];
		i++;
	}
	return (coder_arr);
}

static t_coder	**init_prio_map(t_config *config, int nbr_coders)
{
	int		i;
	t_coder	**coder_ptr_arr;

	i = 0;
	coder_ptr_arr = malloc(sizeof(t_coder *) * nbr_coders);
	if (!coder_ptr_arr)
		return (NULL);
	while (i < nbr_coders)
	{
		coder_ptr_arr[i] = &config->coders[i];
		i++;
	}
	return (coder_ptr_arr);
}

static int	init_arrays(t_config *config)
{
	config->dongles = init_dongles(config->nbr_coders);
	if (!config->dongles)
		return (1);
	config->coders = init_coders(config, config->nbr_coders);
	if (!config->coders)
		return (1);
	config->prio_map = init_prio_map(config, config->nbr_coders);
	if (!config->prio_map)
		return (1);
	return (0);
}
