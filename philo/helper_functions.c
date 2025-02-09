/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 17:46:44 by dbank             #+#    #+#             */
/*   Updated: 2025/01/30 16:14:12 by dbank            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

unsigned int	convert_to_uint(const char *nptr)
{
	size_t			i;
	unsigned int	nbr;

	if (nptr == NULL)
		return (0);
	i = 0;
	nbr = 0;
	while (nptr[i] == ' ' || nptr[i] == '\t' || nptr[i] == '\n'
		|| nptr[i] == '\v' || nptr[i] == '\f' || nptr[i] == '\r')
		i++;
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		nbr *= 10;
		nbr += (nptr[i++] - '0');
	}
	return (nbr);
}

unsigned int	get_time(void)
{
	struct timeval	timeval;
	unsigned int	timestamp;

	gettimeofday(&timeval, NULL);
	timestamp = timeval.tv_sec * 1000
		+ timeval.tv_usec / 1000;
	return (timestamp);
}

int	all_eaten(t_philo *philos)
{
	size_t	i;

	if (philos->general->eatcount_set == false)
		return (0);
	i = 0;
	while (i < philos->general->philo_count)
	{
		pthread_mutex_lock(&philos[i].philo_mutex);
		if (philos[i].done_eating == false)
			return (pthread_mutex_unlock(&philos[i].philo_mutex), 0);
		pthread_mutex_unlock(&philos[i].philo_mutex);
		i++;
	}
	pthread_mutex_lock(&philos->general->sim_mutex);
	philos->general->sim_running = false;
	pthread_mutex_unlock(&philos->general->sim_mutex);
	return (1);
}

int	check_eating(t_philo *philo)
{
	pthread_mutex_lock(&philo->philo_mutex);
	if (philo->general->eatcount_set == true
		&& philo->mealcount == philo->general->eatcount)
		philo->done_eating = true;
	pthread_mutex_unlock(&philo->philo_mutex);
	return (true);
}

void	put_down_forks(t_philo *philo)
{
	if (philo->fork_in_use[0])
	{
		pthread_mutex_unlock(philo->my_forks[0]);
		philo->fork_in_use[0] = false;
	}
	if (philo->fork_in_use[1])
	{
		pthread_mutex_unlock(philo->my_forks[1]);
		philo->fork_in_use[1] = false;
	}
}
