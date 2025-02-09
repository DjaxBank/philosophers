/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 14:13:20 by dbank             #+#    #+#             */
/*   Updated: 2025/01/31 12:20:21 by dbank            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	cleanup(t_philo *philos, t_general *general)
{
	size_t	i;

	if (general->forks)
	{
		i = 0;
		while (i < general->philo_count)
		{
			pthread_mutex_destroy(&philos[i].start);
			pthread_mutex_destroy(&philos[i].philo_mutex);
			pthread_mutex_destroy(&general->forks[i++]);
		}
		free(general->forks);
	}
	free(philos);
	pthread_mutex_destroy(&general->writing);
	pthread_mutex_destroy(&general->sim_mutex);
}

int	setup(size_t max, t_general *general,
		pthread_mutex_t *forks, t_philo *philos)
{
	unsigned int	name;
	size_t			i;

	name = 1;
	i = 0;
	while (i < max)
	{
		philos[i].name = name++;
		philos[i].general = general;
		philos[i].state = (philos[i].name - 1) % 2;
		philos[i].mealcount = 0;
		memset(philos[i].fork_in_use, 0, 2 * sizeof(bool));
		philos[i].done_eating = false;
		if ((i == 0 && (pthread_mutex_init(&general->writing, NULL) != 0
					|| pthread_mutex_init(&general->sim_mutex, NULL) != 0))
			|| (pthread_mutex_init(&forks[i], NULL) != 0
				|| pthread_mutex_init(&philos[i].start, NULL) != 0
				|| pthread_mutex_init(&philos[i].philo_mutex, NULL) != 0))
			return (write(STDERR_FILENO, "Mutex failed!\n", 15), 0);
		philos[i].my_forks[0] = &philos[i].general->forks[philos[i].name - 1];
		philos[i].my_forks[1]
			= &philos[i].general->forks[(philos[i].name) % max];
		pthread_mutex_lock(&philos[i++].start);
	}
	return (1);
}

int	fill_general(char **argv, int argc, t_general *general)
{
	general->philo_count = convert_to_uint(argv[1]);
	if (general->philo_count == 0)
		return (write(STDERR_FILENO, "No philosophers!\n", 18), 0);
	general->time_todie = convert_to_uint(argv[2]);
	general->waittimes[eating] = convert_to_uint(argv[3]);
	general->waittimes[sleeping] = convert_to_uint(argv[4]);
	general->waittimes[thinking] = (general->time_todie
			- general->waittimes[eating] - general->waittimes[sleeping]) / 5;
	if (argc == 6)
	{
		general->eatcount_set = true;
		general->eatcount = convert_to_uint(argv[5]);
	}
	else
	{
		general->eatcount_set = false;
		general->eatcount = 0;
	}
	return (1);
}

int	check_input(int argc, char **argv)
{
	size_t	i;
	size_t	j;

	if (argc < 5 || argc > 6)
		return (write(STDERR_FILENO, "Invalid amount of arguments!\n", 30), 0);
	i = 1;
	while (argv[i] != NULL)
	{
		if (argv[i][0] == '-')
			return (write(STDERR_FILENO,
					"instructions can't be negative!\n", 33), 0);
		j = 0;
		while (argv[i][j] != '\0')
		{
			if (!(argv[i][j] >= '0' && argv[i][j] <= '9'))
				return (write(STDERR_FILENO,
						"instructions must be numbers!\n", 31), 0);
			j++;
		}
		i++;
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_philo			*philos;
	t_general		general;

	if (!check_input(argc, argv) || !fill_general(argv, argc, &general))
		return (EXIT_FAILURE);
	if (general.eatcount_set && general.eatcount == 0)
		return (EXIT_SUCCESS);
	philos = malloc(general.philo_count * sizeof(t_philo));
	general.forks = malloc(general.philo_count * sizeof(pthread_mutex_t));
	if (philos == NULL || general.forks == NULL)
		return (write(STDERR_FILENO, "Malloc failed!\n", 16),
			cleanup(philos, &general), EXIT_FAILURE);
	if (setup(general.philo_count, &general, general.forks, philos))
	{
		general.exitcode = 0;
		start_simulation(philos, &general);
	}
	else
		general.exitcode = 1;
	return (cleanup(philos, &general), general.exitcode);
}
