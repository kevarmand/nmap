/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:59:39 by ertrigna          #+#    #+#             */
/*   Updated: 2026/06/23 16:45:02 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nmap.h"

int	create_raw_socket(int protocol)
{
	int	sock;
	int	on;

	sock = socket(AF_INET, SOCK_RAW, protocol);
	if (sock < 0)
	{
		fprintf("You must have sudo permission\n");
		exit (1);
	}
	if (protocol == IPPROTO_TCP || protocol == IPPROTO_UDP)
	{
		if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
		{
			close(sock);
			return (-1);
		}
	}
	return (sock);	
}



