/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 16:40:44 by dbank             #+#    #+#             */
/*   Updated: 2025/01/30 15:42:02 by dbank            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_state	get_state(t_philo *philo)
{
	t_state	status;

	pthread_mutex_lock(&philo->philo_mutex);
	status = philo->state;
	pthread_mutex_unlock(&philo->philo_mutex);
	return (status);
}

void	set_state(t_philo *philo, t_state new_state)
{
	pthread_mutex_lock(&philo->philo_mutex);
	philo->state = new_state;
	pthread_mutex_unlock(&philo->philo_mutex);
}

void	kill_philo(t_philo *philo, t_general *general)
{
	set_state(philo, dead);
	printf("%u %u died\n", get_time() - general->starttime, philo->name);
	stop_sim(general);
}

bool	sim_running(t_general *general)
{
	bool	status;

	pthread_mutex_lock(&general->sim_mutex);
	status = general->sim_running;
	pthread_mutex_unlock(&general->sim_mutex);
	return (status);
}

void	stop_sim(t_general *general)
{
	pthread_mutex_lock(&general->sim_mutex);
	general->sim_running = false;
	pthread_mutex_unlock(&general->sim_mutex);
}
