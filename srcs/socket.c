/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:59:39 by ertrigna          #+#    #+#             */
/*   Updated: 2026/06/11 15:57:34 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nmap.h"

int	create_socket(void)
{
	int p;
    if (proto == PROTO_TCP)
        p = IPPROTO_TCP;
    else if (proto == PROTO_UDP)
        p = IPPROTO_UDP;
    else
        p = IPPROTO_ICMP;
	
	int sockfd = socket(AF_INET, SOCK_RAW, p);
	if (sockfd < 0)
	{
		fprintf(stderr, "Socket creation failed\n");
		return (1);
	}
	return (sockfd);	
}
