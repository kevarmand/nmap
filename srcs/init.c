/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 14:28:59 by ertrigna          #+#    #+#             */
/*   Updated: 2026/06/11 15:41:44 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nmap.h"

void	init_config(t_config *conf)
{
	conf->verbose = 0;
	conf->syn = 0;
	conf->udp = 0;
	conf->threads = 10;
	conf->timeout_ms = 1000;
	conf->target = NULL;
	conf->ports = NULL;
	conf->port_count = 0;
	conf->range = 0;
	conf->port_start = 0;
	conf->port_end = 0;

}

void	init_target(t_target *target)
{
	memset(target->hostname, 0, sizeof(target->hostname));
	memset(target->ip, 0, sizeof(target->ip));
	memset(target->addr, 0, sizeof(target->addr));
	
	target->addr.sin_family = AF_INET;
	target->resolved = 0;
}
