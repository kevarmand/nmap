/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nmap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:45:51 by ertrigna          #+#    #+#             */
/*   Updated: 2026/06/11 15:55:10 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NMAP_H
#define NMAP_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <sys/time.h>
#include <time.h>

#include <pthread.h>
#include <pcap.h>

typedef enum	e_proto
{
    PROTO_TCP,
    PROTO_UDP,
    PROTO_ICMP
}	t_proto;

typedef struct s_config
{
	int					verbose;		// for -v flag
	int					syn;
	int					udp;

	uint16_t			*ports;			// port
	size_t 				port_count;		// number of ports

	int 				threads;
	int 				timeout_ms;

	char				*target;

	int					range;			// for a range mode flag
	int					port_start;
	int					port_end;
} t_config;

typedef struct s_target
{
	char 				hostname[256];
	char 				ip[INET_ADDRSTRLEN];
	struct sockaddr_in	addr;
	int					resolved;
} t_target;

// INIT
void	init_config(t_config *conf);

// PARSING

// SOCKET

#endif