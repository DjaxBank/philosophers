# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dbank <dbank@student.codam.nl>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/16 14:12:53 by dbank             #+#    #+#              #
#    Updated: 2025/01/29 19:05:48 by dbank            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = philo
SRC = philo.c helper_functions.c simulation.c states.c
OBJS_DIR = .objs
OBJS = $(SRC:%.c=$(OBJS_DIR)/%.o)
CC = cc
CCFLAGS= -Wall -Wextra -Werror

all: $(NAME)
$(NAME): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR):
	mkdir $(OBJS_DIR)

$(OBJS_DIR)/%.o: %.c | $(OBJS_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re