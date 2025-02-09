/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 13:02:11 by dbank             #+#    #+#             */
/*   Updated: 2025/01/31 11:37:47 by dbank            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor(void	*ptr)
{
	size_t			i;
	t_philo			*philos;
	t_general		*general;
	unsigned int	last_meal;

	philos = ptr;
	general = philos[0].general;
	while (sim_running(general) && !all_eaten(philos))
	{
		i = 0;
		while (i < general->philo_count && sim_running(general))
		{
			pthread_mutex_lock(&philos[i].philo_mutex);
			last_meal = philos[i].last_meal;
			pthread_mutex_unlock(&philos[i].philo_mutex);
			if (((get_state(&philos[i]) != eating
						&& get_time() - last_meal
						>= general->time_todie)
					|| get_state(&philos[i]) == dead) && !all_eaten(philos))
				return (kill_philo(&philos[i], general), NULL);
			i++;
		}
		usleep(1000);
	}
	return (stop_sim(general), NULL);
}

void	print_state(t_philo *philo)
{
	int					check;
	unsigned int		timestamp;
	const t_state		state = get_state(philo);

	pthread_mutex_lock(&philo->general->writing);
	if (!sim_running(philo->general) || state == dead)
		return ((void)pthread_mutex_unlock(&philo->general->writing));
	timestamp = get_time() - philo->general->starttime;
	if (state == pickup_fork)
		check = printf("%u %u has taken a fork\n", timestamp, philo->name);
	else if (state == eating)
		check = printf("%u %u is eating\n", timestamp, philo->name);
	else if (state == sleeping)
		check = printf("%u %u is sleeping\n", timestamp, philo->name);
	else
		check = printf("%u %u is thinking\n", timestamp, philo->name);
	if (check == -1)
	{
		philo->general->exitcode = (check == -1);
		philo->general->sim_running = false;
	}
	pthread_mutex_unlock(&philo->general->writing);
}

void	try_to_eat(t_philo *philo, pthread_mutex_t *my_forks[2])
{
	unsigned int	eattime;

	set_state(philo, pickup_fork);
	pthread_mutex_lock(my_forks[1 - philo->name % 2]);
	philo->fork_in_use[0] = true;
	print_state(philo);
	if (philo->general->philo_count > 1)
	{
		pthread_mutex_lock(my_forks[(philo->name % 2)]);
		philo->fork_in_use[1] = true;
		print_state(philo);
	}
	eattime = get_time();
	if (philo->general->philo_count > 1)
	{
		pthread_mutex_lock(&philo->philo_mutex);
		philo->last_meal = eattime;
		pthread_mutex_unlock(&philo->philo_mutex);
		set_state(philo, eating);
		philo->mealcount++;
	}
}

void	*philosopher_actions(void *ptr)
{
	t_philo			*philo;

	philo = ptr;
	pthread_mutex_lock(&philo->start);
	while (sim_running(philo->general) && check_eating(philo))
	{
		if (get_state(philo) == eating)
		{
			put_down_forks(philo);
			set_state(philo, sleeping);
		}
		else if (get_state(philo) == sleeping)
			set_state(philo, thinking);
		else if (philo->state == thinking)
		{
			if (philo->general->philo_count > 1)
				try_to_eat(philo, philo->my_forks);
			else
				set_state(philo, sleeping);
		}
		print_state(philo);
		if (get_state(philo) != dead)
			usleep(philo->general->waittimes[get_state(philo)] * 1000);
	}
	return (put_down_forks(philo), pthread_mutex_unlock(&philo->start), NULL);
}

void	start_simulation(t_philo *philos, t_general *general)
{
	int	i;

	i = 0;
	while (i < (int)general->philo_count)
	{
		if (pthread_create(&philos[i].thread, NULL,
				philosopher_actions, &philos[i]) != 0)
		{
			philos[0].general->exitcode = EXIT_FAILURE;
			return ((void)write(STDERR_FILENO,
					"Thread creation failed!\n", 25));
		}
		i++;
	}
	general->starttime = get_time();
	general->sim_running = true;
	while (i-- > 0)
	{
		philos[i].last_meal = philos[i].general->starttime;
		pthread_mutex_unlock(&philos[i].start);
	}
	pthread_create(&general->monitor, NULL, monitor, philos);
	while (++i < (int)general->philo_count)
		pthread_join(philos[i].thread, NULL);
	pthread_join(general->monitor, NULL);
}
