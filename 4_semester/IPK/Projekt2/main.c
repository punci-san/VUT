#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>

//#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
//#include <netinet/ip_icmp.h>

#define PACKET_LEN 8192
#define BUF_SIZE 500

void GetLocalAddress()
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];

	memset(&hints,0,sizeof(hints));

	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME;

	s = getaddrinfo("localhost",NULL,&hints,&result);

	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	void *ptr = NULL;

  while (result)
    {
      inet_ntop (result->ai_family, result->ai_addr->sa_data, buf, BUF_SIZE);

      switch (result->ai_family)
        {
        case AF_INET:
          ptr = &((struct sockaddr_in *) result->ai_addr)->sin_addr;
          break;
        case AF_INET6:
          ptr = &((struct sockaddr_in6 *) result->ai_addr)->sin6_addr;
          break;
        }
      inet_ntop (result->ai_family, ptr, buf, BUF_SIZE);
      printf ("IPv%d address: %s (%s)\n", result->ai_family == PF_INET6 ? 6 : 4,
              buf, result->ai_canonname);
      result = result->ai_next;
    }
}

void SendUDPPacket(char *address, int port)
{
	int udp_socket = 0;
  u_int16_t src_port, dst_port;
  u_int32_t src_addr, dst_addr;

  char buffer[PACKET_LEN];
  struct ip *ip = (struct ip *)buffer;
  struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ip));

  struct sockaddr_in sin, din;
  int one = 1;
  const int *val = &one;

  memset(&buffer,0,PACKET_LEN);

  udp_socket = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);

  if (udp_socket < 0)
  {
    perror("soccket() error");
    exit(1);
  }

  printf("OK: raw socket is created.\n");

	if(setsockopt(udp_socket, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}

  // Set packet info
  src_port = htons(port);
  dst_port = htons(port);
  src_addr = inet_addr("127.0.0.1");
  dst_addr = inet_addr(address);

  sin.sin_family = AF_INET;
  din.sin_family = AF_INET;

  sin.sin_port = htons(src_port);
  din.sin_port = htons(dst_port);

  inet_pton(AF_INET,"127.0.0.1",&(sin.sin_addr));
  inet_pton(AF_INET,address,&(din.sin_addr));

  ip->ip_hl = 5;          /* Header length */
  ip->ip_v = 4;           /* Version */

  ip->ip_tos = 0;         /* Type of service */
  ip->ip_len = sizeof(struct ip) + sizeof(struct udphdr);         /* Total length */
  ip->ip_id = htons(1254);          /* Identification */
  ip->ip_off = 0;         /* Fragment offset field */

  ip->ip_ttl = MAXTTL;         /* Time to live */
  ip->ip_p = IPPROTO_UDP;           /* Protocol */
  ip->ip_sum = 0;         /* Checksum */
  ip->ip_src = sin.sin_addr;         /* Source address */
  ip->ip_dst = din.sin_addr;         /* Destination address */

  udp->uh_sport = src_port;
  udp->uh_dport = dst_port;
  udp->uh_ulen = htons	(sizeof(struct udphdr));
	udp->uh_sum = 0;
	printf("UDP LEN %u \n",sizeof(struct udphdr));

  if (sendto(udp_socket,buffer,ip->ip_len,0,(struct sockaddr *)&sin,sizeof(sin)) < 0)
  {
		perror("sendto() failed");
		exit(1);
  }

  printf("OK: sendto.\n");

  close(udp_socket);
}


int main(int argc, char *argv[])
{
  GetLocalAddress();
  return 0;
}
