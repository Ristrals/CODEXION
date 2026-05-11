/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initializer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:59:27 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 18:03:10 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static t_dongle	*init_dongles(int nbr_coders);
static t_coder	*init_coders(t_config *config, int nbr_coders);

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
		config->fifo_edf = 1;
	else
		config->fifo_edf = 0;
	config->time_start = 0;
	config->sim_status = 0;
	pthread_mutex_init(&config->lock_sim_status, NULL);
	pthread_mutex_init(&config->lock_write, NULL);
	config->dongles = init_dongles(config->nbr_coders);
	if (!config->dongles)
		return (1);
	config->coders = init_coders(config, config->nbr_coders);
	if (!config->coders)
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
		dongle_arr[i].last_released = 0;
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
		coder_arr[i].req_time = 0;
		pthread_mutex_init(&coder_arr[i].lock_req_time, NULL);
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

void	cleanup(t_config *config)
{
	int	i;

	i = 0;
	if (config->coders)
	{
		while (i < config->nbr_coders)
		{
			pthread_mutex_destroy(&config->coders[i].lock_compiled);
			pthread_mutex_destroy(&config->coders[i].lock_last_comp);
			i++;
		}
		free(config->coders);
	}
	i = 0;
	if (config->dongles)
	{
		while (i < config->nbr_coders)
		{
			pthread_mutex_destroy(&config->dongles[i].dongle);
			i++;
		}
		free(config->dongles);
	}
	pthread_mutex_destroy(&config->lock_write);
	pthread_mutex_destroy(&config->lock_sim_status);
}
