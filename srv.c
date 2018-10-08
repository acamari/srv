/*
 * Copyright (c) 2018 Abel Abraham Camarillo Ojeda <acamari@verlet.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

in_port_t
getport(struct sockaddr *sa)
{
	switch(sa->sa_family) {
		case AF_INET:
			return ntohs(((struct sockaddr_in *)sa)->sin_port);
		case AF_INET6:
			return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
		default:
			return -1;
	}
}

/* debug prints inet(6) address */
void
dprintsaddrin(struct sockaddr *saddr)
{
	char		addrstr[INET6_ADDRSTRLEN];
	struct sockaddr_in	*sin;
	struct sockaddr_in6	*sin6;
	in_port_t	port;

	sin = (struct sockaddr_in *)saddr;
	sin6 = (struct sockaddr_in6 *)saddr;
	port = -1;
	printf("len: %u\n", saddr->sa_len);
	printf("family: %u\n", saddr->sa_family);

	if (!(saddr->sa_family == AF_INET || saddr->sa_family == AF_INET6))
		return;
	port = getport(saddr);
	printf("port: %u\n", port);
	if (inet_ntop(saddr->sa_family,
	    saddr->sa_family == AF_INET ? (void *)&sin->sin_addr : (void *)&sin6->sin6_addr,
	    addrstr, sizeof(addrstr)) == NULL)
		err(1, "inet_ntop");
	printf("addr: %s\n", addrstr);
}

/* set close-on-exec flag on fd */
void
cloexec(int fd)
{
	if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
		err(1, "fcntl");
}

void
usage()
{
	err(1, "invalid options");
}

int
main(int argc, char *argv[])
{
	int sockfd; /* listening socket */
	int cfd; /* accepted connection socket */
	struct sockaddr_storage	ss;
	socklen_t		size;
	int wflag = 0; /* how many seconds to wait for connection */
	int dflag = 0; /* dont daemonize before accepting connections */
	int c; /* option char */

	size = sizeof(ss);
	while ((c = getopt(argc, argv, "dw:")) != -1) {
		switch (c) {
		case 'd':
			dflag = 1;
			break;
		case 'w':
			wflag = atoi(optarg);
			if (wflag < 0)
				errx(1, "invalid w argument: %d", wflag);
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc <= 0)
		errx(1, "not enough arguments");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err(1, "socket");
	cloexec(sockfd);
	if (listen(sockfd, 1) == -1)
		err(1, "listen");
	if (getsockname(sockfd, (struct sockaddr *)&ss, &size) == -1)
		err(1, "getsockname");
	/*
	dprintsaddrin((struct sockaddr *)&ss);
	*/
	printf("%u\n", getport((struct sockaddr *)&ss));
	fflush(stdout);
	if (!dflag)
		daemon(1, 0);
	/* wait up to "wflag" seconds for incoming connection */
	if (wflag)
		alarm(wflag);
	if ((cfd = accept(sockfd, NULL, NULL)) == -1)
		err(1, "accept");
	alarm(0); /* connection received, timeout no longer needed */
	cloexec(cfd);
	if (dup2(cfd, STDIN_FILENO) == -1 ||
	    dup2(cfd, STDOUT_FILENO) == -1)
		err(1, "dup2");
	execvp(argv[0], &argv[0]);
	err(1, "execvp");
}
