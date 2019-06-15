// (c) O.Rysavy, 2012
// Demo for IPK.
//
// This demo should demonstrate how tcp socket needs to be segmented
// if we set
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "bigmsg.h"

int main (int argc, const char * argv[]) {
	int rc;
	int welcome_socket;
	struct sockaddr_in6 sa;
	struct sockaddr_in6 sa_client;
	char str[INET6_ADDRSTRLEN];
	socklen_t sa_client_len=sizeof(sa_client);
	if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0)
	{
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	/*************************************************************/
	/* Allow socket descriptor to be reuseable                   */
	/*************************************************************/
	int on = 1;
	rc = setsockopt(welcome_socket, SOL_SOCKET,  SO_REUSEADDR,
					(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed fo SO_REUSEADDR");
		close(welcome_socket);
		exit(-1);
	}

	memset(&sa,0,sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_addr = in6addr_any;
	sa.sin6_port = htons(11111);
	if ((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0)
	{
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	if ((rc = listen(welcome_socket, 1)) < 0)
	{
		perror("listen() failed");
		exit(EXIT_FAILURE);
	}

	printf("Listening... \n");

	while(1)
	{
		// still blocking accept
		int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);
		if (comm_socket > 0)
		{

			/*************************************************************/
			/* Set MSS for TCP socket
			/*************************************************************/
			int mss = 0 ;
			int sl = sizeof(mss) ;
			if(getsockopt(comm_socket, IPPROTO_TCP, TCP_MAXSEG,  &mss, &sl) == 0) {
				fprintf(stdout, "MSS = %u (sizeof(MSS) = %d)\n", mss, sl) ;
			}

			int tcpmaxseg = 1000;
			rc = setsockopt(welcome_socket, IPPROTO_TCP, TCP_MAXSEG,(char *) &tcpmaxseg,sizeof(tcpmaxseg));
			if (rc != 0)
			{
				perror("setsockopt() failed for TCP_MAXSEG");
			}

			if(inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
				printf("New connection:");
				printf("  Client address is %s\n", str);
				printf("  Client port is %d\n", ntohs(sa_client.sin6_port));
			}

			send(comm_socket, BIG_MSG, strlen(BIG_MSG), 0);

			close(comm_socket);
		}
	}
}
