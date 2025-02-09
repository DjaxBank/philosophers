/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 14:13:17 by dbank             #+#    #+#             */
/*   Updated: 2025/01/31 11:38:42 by dbank            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <pthread.h>
# include <stdbool.h>

typedef enum state
{
	eating,
	sleeping,
	thinking,
	dead,
	pickup_fork
}	t_state;

typedef struct general
{
	size_t			philo_count;
	unsigned int	starttime;
	bool			sim_running;
	pthread_mutex_t	sim_mutex;
	int				exitcode;
	unsigned int	waittimes[3];
	unsigned int	time_todie;
	bool			eatcount_set;
	unsigned int	eatcount;
	pthread_mutex_t	*forks;
	pthread_mutex_t	writing;
	pthread_t		monitor;
}	t_general;

typedef struct philo
{
	unsigned int		name;
	pthread_mutex_t		*my_forks[2];
	bool				fork_in_use[2];
	t_state				state;
	pthread_mutex_t		philo_mutex;
	pthread_t			thread;
	unsigned int		last_meal;
	unsigned int		mealcount;
	bool				done_eating;
	t_general			*general;
	pthread_mutex_t		start;
}	t_philo;

unsigned int	convert_to_uint(const char *nptr);
void			start_simulation(t_philo *philos, t_general *general);
void			get_forks(t_philo *philo, pthread_mutex_t *my_forks[2]);
unsigned int	get_time(void);
int				all_eaten(t_philo *philos);
bool			sim_running(t_general *general);
void			kill_philo(t_philo *philo, t_general *general);
void			set_state(t_philo *philo, t_state new_state);
t_state			get_state(t_philo *philo);
void			put_down_forks(t_philo *philo);
void			stop_sim(t_general *general);
int				check_eating(t_philo *philo);

#endif