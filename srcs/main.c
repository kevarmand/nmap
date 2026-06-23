/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ertrigna <ertrigna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:59:54 by ertrigna          #+#    #+#             */
/*   Updated: 2026/06/23 17:04:51 by ertrigna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nmap.h"

static void	print_usage()
{
	printf("./ft_nmap: [Options]\n");
	printf("-help Print this help screen\n");
	printf("--ports ports to scan (eg: 1-10 or 1,2,3 or 1,5-15)\n");
	printf("-ip ip addresses to scan in dot format\n");
	printf("--file File name containing IP addresses to scan,\n");
	printf("--speedup [250 max] number of parallel threads to use\n")
	printf("--scan SYN/NULL/FIN/XMAS/ACK/UDP\n")
}

int	main(int ac, char *av[])
{
	// if (ac < 5)
	// {
	// 	print_usage()
	// 	exit (1);
	// }

	t_scan	scan;

	init_scan(&scan);
	if (init_socket(&scan) < 0)
	{
		fprintf(stderr, "error in socket creation");
		return (-1);	
	}
	printf("TCP : %d\n", scan.socket.tcp);
	printf("UDP : %d\n", scan.socket.udp);
	printf("ICMP: %d\n", scan.socket.icmp);

	close_socket(&scan);
	return (0);
}
