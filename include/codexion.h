/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:33:34 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/11 18:03:10 by kmalfois         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// LIBRARIES
# include <stdio.h>     // printf, fprintf
# include <unistd.h>    // write, usleep
# include <stdlib.h>    // malloc, free, atoi
# include <string.h>    // strcmp, strlen, memset
# include <pthread.h>   // pthread functions
# include <sys/time.h>  // gettimeofday

// STRUCTS
typedef struct s_config	t_config;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;

typedef struct s_dongle
{
	pthread_mutex_t	dongle;
	int				id;
	long			last_released; // last used timestamp
}	t_dongle;

typedef struct s_coder
{
	pthread_t		thread;
	int				id;
	long			req_time;
	pthread_mutex_t	lock_req_time;
	int				compiled;
	pthread_mutex_t	lock_compiled;
	long			last_comp;
	pthread_mutex_t	lock_last_comp;
	t_config		*config;
	t_dongle		*l_dgl;
	t_dongle		*r_dgl;
}	t_coder;

typedef struct s_config
{
	int				nbr_coders;
	int				tt_burnout;
	int				tt_compile;
	int				tt_debug;
	int				tt_refactor;
	int				compiles_req;
	int				dgl_cd;
	int				fifo_edf; // 0:fifo 1:edf
	long			time_start; // time when the simulation starts
	int				sim_status; // boolean for the simulation's status
	pthread_mutex_t	lock_sim_status; // mutex for consulting sim_status
	pthread_mutex_t	lock_write; // mutex to be allowed to write
	pthread_t		monitor; // monitor thread
	t_dongle		*dongles; // dongles array
	t_coder			*coders; // coders array
}	t_config;

// Main function
int		main(int argc, char *argv[]);

// Functions
int		parser(int argc, char *argv[]);
int		init_config(t_config *config, char *argv[]);
void	monitor_script(t_config *config);
void	coder_script(t_coder *self);
void	cleanup(t_config *config);

// Coder Tool functions
int		fifo_priority(t_coder *self, int side);
int		edf_priority(t_coder *self, int side);
int		dongle_cooldown(t_dongle *dongle, int cooldown);

// Utility functions
long	get_time(void);
void	sim_print(t_coder *self, char *msg, int critical);
t_coder	*get_neighbor(t_coder *coder, int side);
int		check_sim_status(t_coder *self);

#endif