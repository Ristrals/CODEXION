/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmalfois <kmalfois@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 10:33:34 by kmalfois          #+#    #+#             */
/*   Updated: 2026/05/15 16:36:18 by kmalfois         ###   ########.fr       */
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

//ENUMS
// Coder state
typedef enum e_state
{
	REQ,
	COMP,
	WORK
}	t_state;

// Print critical
typedef enum e_scheduler
{
	EDF,
	FIFO
}	t_scheduler;

// Print critical
typedef enum e_print
{
	STND,
	CRIT
}	t_print;

// STRUCTS
typedef struct s_config	t_config;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;

typedef struct s_dongle
{
	pthread_mutex_t	dongle;
	int				id;
	int				in_use;
	long			last_used; // last used timestamp
}	t_dongle;

typedef struct s_coder
{
	pthread_t		thread;
	int				id;
	t_state			state;
	long			req_time;
	pthread_mutex_t	lock_state;
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
	pthread_cond_t	cond_room;
	pthread_t		monitor; // monitor thread
	t_dongle		*dongles; // dongles array
	t_coder			*coders; // coders array
	t_coder			**prio_map; // coder pointers for ordered priority
}	t_config;

// Main function
int		main(int argc, char *argv[]);

// Functions
int		parser(int argc, char *argv[]);
int		init_config(t_config *config, char *argv[]);
void	monitor_script(t_config *config);
void	coder_script(t_coder *self);
void	cleanup(t_config *config);

// Monitor Tool functions
int		check_deadlines(t_config *config);
int		check_compiles(t_config *config);
int		compare_fifo(t_coder *coder0, t_coder *coder1);
int		compare_edf(t_coder *coder0, t_coder *coder1);

// Utility functions
long	get_time(void);
void	sim_print(t_coder *self, char *msg, int critical);
int		check_sim_status(t_config *config);

#endif