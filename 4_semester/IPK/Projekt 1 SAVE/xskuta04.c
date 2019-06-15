/*	Author: Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*
*		Server poskytujúci informácie o systéme pomocou HTTP protokolu
*
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>	// HOST_NAME_MAX
#include <signal.h> // Signal for SIG_IGN child
#include <errno.h> 	// errno
#include <time.h>		// nanosleep

#define BUFF_LEN 2048
#define MAX_PORT_NUM 65535
#define MAGIC_NUMBER 1
#define MAX_CONNECTIONS 200

#define TEXT_PLAIN 1
#define APPLICATION_JSON 2

#define TEXT_PLAIN_STR "text/plain"
#define APPLICATION_JSON_STR "application/json"

#define BAD_REQUEST "400 Bad Request"
#define NOT_GET "405 Method Not Allowed"
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"
#define INTERNAL_SERVER_HTTP_NOT_SUPPORTED "505 HTTP Version Not Supported"

#define ERROR_HTTP -3
#define ERROR_SERVER -2
#define ERROR_GET -1
#define ERROR_BAD_REQUEST 0

#define RQ_HOSTNAME 1
#define RQ_CPU_NAME 2
#define RQ_CPU_LOAD 3
#define RQ_CPU_LOAD_REFRESH 4

// Calculate CPU load
int getCpuLoad()
{
	// File holder
	FILE *cpu_stat;

	// Create timespec
	struct timespec sleep_time,time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 75000000;

	// Firs cpu variables
	double userPrev = 0;
	double nicePrev = 0;
	double systemPrev = 0;
	double idlePrev = 0;
	double iowaitPrev = 0;
	double irqPrev = 0;
	double softirqPrev = 0;
	double stealPrev = 0;
	double guestPrev = 0;
	double guest_nicePrev = 0;

	// Load file
	if ((cpu_stat = fopen("/proc/stat","rb")) == NULL)
	{
		fprintf(stderr,"Problem with opening file.\n");
		return -1;
	}

	// Get the values from file
	fscanf(cpu_stat,"cpu %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &userPrev, &nicePrev, &systemPrev, &iowaitPrev, &idlePrev, &irqPrev, &softirqPrev, &stealPrev, &guestPrev, &guest_nicePrev);

	// Close file
	fclose(cpu_stat);

	// Wait for 75000000 nanoseconds
	nanosleep(&sleep_time,&time);

	// Load second time
	double user = 0;
	double nice = 0;
	double system = 0;
	double idle = 0;
	double iowait = 0;
	double irq = 0;
	double softirq = 0;
	double steal = 0;
	double guest = 0;
	double guest_nice = 0;

	// Load file
	if ((cpu_stat = fopen("/proc/stat","rb")) == NULL)
	{
		fprintf(stderr,"Problem with opening file.\n");
		return -1;
	}

	// Get the values from file
	fscanf(cpu_stat,"cpu %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &user, &nice, &system, &iowait ,&idle, &irq, &softirq, &steal, &guest, &guest_nice);

	// Close file
	fclose(cpu_stat);

	// Stackoverflow calculation
	double PrevIdle = idlePrev + iowaitPrev;
	double Idle = idle + iowait;
	double PrevNonIdle = userPrev + nicePrev + systemPrev + irqPrev + softirqPrev + stealPrev;
	double NonIdle = user + nice + system + irq + softirq + steal;
	double PrevTotal = PrevIdle + PrevNonIdle;
	double Total = Idle + NonIdle;
	double totald = Total - PrevTotal;
	double idled = Idle - PrevIdle;
	double CPU_Percentage = 0;

	// To protect from division by zero
	if (totald != 0)
	{
		CPU_Percentage = (totald - idled)/totald * 100;
	}

	// Round to integer
	int cpu_usage = (int)(CPU_Percentage + 0.5);

	// Return value
	return cpu_usage;
}

// Get hostname
int getNameOfHost(char *str)
{
	// File pointer
	FILE *hostname_file = NULL;
	char ch = '\0';

	// Clear the string
  memset(str,0,BUFF_LEN);

	// Open file with hostname
	if ((hostname_file = fopen("/proc/sys/kernel/hostname","rb")) == NULL)
	{
		// File opening failed
		fprintf(stderr,"Problem with opening file.\n");
		return -1;
	}

	// Start copying
	for (size_t i = 0; i < BUFF_LEN-1; i++)
	{
		// Get char
		ch = fgetc(hostname_file);

		// Check if char is new line or EOF
		if (ch == '\n' || ch < 0)
		{
			// And end copying
			break;
		}
		// Or esle just copy
		str[i] = ch;
	}

	// Close fire
	fclose(hostname_file);

	// Return OK
  return 0;
}

// Receive cpu-name
int getCpuName(char *str)
{
	// Variables
  FILE *cpuinfo = NULL;
  char tmp_str[BUFF_LEN];
  char *str_ptr = NULL;

	// Clear strings
  memset(str,0,sizeof(*str));
  memset(&tmp_str,0,sizeof(tmp_str));

	// Try to open file /proc/cpuinfo
  if ((cpuinfo = fopen("/proc/cpuinfo","rb")) == NULL)
  {
		fprintf(stderr,"Problem with opening file.\n");
    return -1;
  }

  // Read until you encounter EOF
  while (fgets(tmp_str,sizeof(tmp_str),cpuinfo))
  {
		// We found the line with cpu-name
    if (memcmp(tmp_str,"model name",10) == 0)
    {
			// Split the string on ": "
      str_ptr = strstr(tmp_str,": ");

			// Check if split succeded
      if (str_ptr)
      {
        // We want to see everything after ": " so increase pointer
        str_ptr += 2;

				// Copy until you reach end of string
				for (size_t i = 0; i < strlen(str_ptr); i++)
				{
					// Reached EOL
					if (str_ptr[i] == '\n')
					{
						// Copy null character to end the string
						str[i] = '\0';
						fclose(cpuinfo);
						return 0;
					}
					// Copy char by char
					str[i] = str_ptr[i];
				}
      }
    }
  }
	// Close file
  fclose(cpuinfo);
  return -1;
}

// Generate HTTP Response header
void generateResponse(char *return_str,int num_response,char *str,int accept_type,int refresh_rate)
{
	// Buffer to hold JSON string
  char tmp_buff[BUFF_LEN];

	// Clear string
  memset(return_str,0,sizeof(*return_str));
  memset(tmp_buff,0,sizeof(tmp_buff));

  // Generate json for response
  sprintf(tmp_buff,"{\"response\" : \"%s\"}",str);

	// Determinate the response we want
	switch (num_response)
	{
		case RQ_HOSTNAME:
    case RQ_CPU_NAME:
    case RQ_CPU_LOAD:
			// Check what type of response they accept
	    if (accept_type == TEXT_PLAIN)
      {
				// Generate response 200 OK
        sprintf(return_str,"HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),str);
      }
      else
      {
				// Generate response 200 OK
        sprintf(return_str,"HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),tmp_buff);
      }
    break;

    case RQ_CPU_LOAD_REFRESH:
			// Check what type of response they accept
      if (accept_type == TEXT_PLAIN)
      {
				// Generate response 200 OK
        sprintf(return_str,"HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: Close\r\nRefresh: %d\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),refresh_rate,str);
      }
      else
      {
				// Generate response 200 OK
        sprintf(return_str,"HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: Close\r\nRefresh: %d\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),refresh_rate,tmp_buff);
      }
		break;

		// Bad Request
		case ERROR_BAD_REQUEST:
			// Check what type of response they accept
      if (accept_type == TEXT_PLAIN)
      {
				// Generate response 404 Bad Request
        sprintf(return_str,"HTTP/1.1 400 Bad Request\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),str);
      }
      else
      {
				// Generate response 404 Bad Request
        sprintf(return_str,"HTTP/1.1 400 Bad Request\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),tmp_buff);
      }
		break;

		// 405 Method Not Allowed
		case ERROR_GET:
			// Check what type of response they accept
      if (accept_type == TEXT_PLAIN)
      {
				// Generate response 405 Method Not Allowed
        sprintf(return_str,"HTTP/1.1 405 Method Not Allowed\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),str);
      }
      else
      {
        // Generate response 405 Method Not Allowed
        sprintf(return_str,"HTTP/1.1 405 Method Not Allowed\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),tmp_buff);
      }
		break;

		// 500 Server ERROR
		case ERROR_SERVER:
    default:
			// Check what type of response they accept
      if (accept_type == TEXT_PLAIN)
      {
				// Generate response 500 Internal Server Error
        sprintf(return_str,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),str);
      }
      else
      {
        // Generate response 500 Internal Server Error
        sprintf(return_str,"HTTP/1.1 500 Internal Server Error\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),tmp_buff);
      }
		break;

		// 505 HTTP Version Not Supported
		case ERROR_HTTP:
			// Check what type of response they accept
			if (accept_type == TEXT_PLAIN)
			{
				// Generate response 500 Internal Server Error
				sprintf(return_str,"505 HTTP Version Not Supported\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: text/plain; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(str),str);
			}
			else
			{
				// Generate response 500 Internal Server Error
				sprintf(return_str,"505 HTTP Version Not Supported\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n%s\r\n\r\n",strlen(tmp_buff),tmp_buff);
			}
		break;
	}
}

// Set things appropriately based on GET request
int determinateGetRequest(char *buff,int *type,int *refresh_rate)
{
	// Variables
	char request[BUFF_LEN];
	char *accept_type = NULL;
	char *tmp_buff = NULL;
	char c = buff[4];
	int i = 0;

	// Clear strings
	memset(&request,0,sizeof(request));

	// Lets find accept
	tmp_buff = strstr(buff,"Accept:");

	// tmp_buff can be NULL if we found nothing
	if (tmp_buff == NULL)
	{
		// Set default plain/text
		*type = TEXT_PLAIN;
	}
	// Lets compare the findings
	else
	{
		// Look for application/json in string
		accept_type = strstr(tmp_buff,"application/json");

		// if not found we do default text
		if (accept_type == NULL)
		{
			*type = TEXT_PLAIN;
		}
		// If not response will contain JSON
		else
		{
			*type = APPLICATION_JSON;
		}
	}

	// Is it GET request ?
	if (strncmp(buff,"GET ",4) != 0)
	{
		// Not GET request
		return ERROR_GET;
	}

	// Find the first space
	if ((tmp_buff = strchr(buff,' ')) == NULL)
	{
		return ERROR_BAD_REQUEST;
	}

	tmp_buff++;

	// Find the second space
	if ((tmp_buff = strchr(tmp_buff,' ')) == NULL)
	{
		return ERROR_BAD_REQUEST;
	}

	// Check if we support HTTP version
	if (strncmp(tmp_buff," HTTP/",6) != 0)
	{
		// Not supported HTTP version
		return ERROR_BAD_REQUEST;
	}

	tmp_buff += 6;

	if (strncmp(tmp_buff,"1.1\r\n",5) != 0)
	{
		return ERROR_HTTP;
	}

	// Skip GET_ and start copying
	while (!(c == ' '))
	{
		request[i++] = c;
		c = buff[4 + i];
	}

	// /hostname
	if (strcmp("/hostname",request) == 0 || strcmp("/hostname/",request) == 0)
	{
		return RQ_HOSTNAME;
	}
	// /cpu-name
	else if (strcmp("/cpu-name",request) == 0 || strcmp("/cpu-name/",request) == 0)
	{
		return RQ_CPU_NAME;
	}
	// /load
	else if (strcmp("/load",request) == 0 || strcmp("/load/",request) == 0)
	{
		return RQ_CPU_LOAD;
	}
	else
	// /load?refresh=
	{
		// We found load with refresh
		if (strncmp(request,"/load?refresh=",14) == 0)
		{
			// We expect max 20 chars after =
      char refresh_r[20];

			// Clear string
      memset(&refresh_r,0,sizeof(refresh_r));

			// Loop var
      int i = 0;

			// Start copying from 14th character because first 14 are
			// the link and check numbers if there is some character
      while (!(request[i + 14] == ' ' || request[i + 14] == '/' || request[i + 14] == '\0'))
      {
				// If there is character it is BAD REQUEST
				if ((request[i + 14] < '0') || (request[i + 14] > '9'))
				{
					return ERROR_BAD_REQUEST;
				}
				// copying
        refresh_r[i] = request[i + 14];
				i++;
      }

			// If the last is / then add 1 to the counter
			if (request[strlen(request) - 1] == '/')
			{
				i++;
			}

			// Check if the amount of numbers and text is equal to the request
			if (((i + 14) != ((long)strlen(request))) || i <= 0)
			{
					return ERROR_BAD_REQUEST;
			}

			// Okay we received only numbers tranform them to int value
      *refresh_rate = atoi(refresh_r);

			return RQ_CPU_LOAD_REFRESH;
		}
		else
		{
			return ERROR_BAD_REQUEST;
		}
	}

	return ERROR_BAD_REQUEST;
}

// Check if port number is valid
int checkArg(char *arg)
{
	// Variables
	char *ptr = NULL;

	// Split argument into numbers and string
	strtol(arg,&ptr,10);

	// If there was some character there it is error
	if (strlen(ptr) > 0)
	{
		return -1;
	}

	// If port number is not in range error
	if (atoi(arg) < 0 || atoi(arg) > MAX_PORT_NUM)
	{
		return -1;
	}

	// Okay everything is good
	return 0;
}

int main(int argc, char *argv[])
{
  // Variables
	int result = 0;
	int accept_type = TEXT_PLAIN;
  int refresh_rate = 0;

	int port_number = 0;
	int server_socket = 0;
	struct sockaddr_in6 server_address;
	struct sockaddr_in6 server_address_client;

	char buff[BUFF_LEN];

	socklen_t sa_client_len = sizeof(server_address_client);

	// Is port number set ?
	if (argc != 2)
	{
		// No ? Then error
		fprintf(stderr,"usage: %s <port_number>\n",argv[0]);
		return(-1);
	}

	// Is port number okay ?
	if (checkArg(argv[1]) < 0)
	{
		// No then error
		fprintf(stderr, "Wrong port number or port number is out of range.\n");
		return -1;
	}

	// Convert port number
	port_number = atoi(argv[1]);

	// Creating server socket
	if ((server_socket = socket(PF_INET6,SOCK_STREAM,0)) < 0)
	{
		perror("socket() failed");
		return (EXIT_FAILURE);
	}

	// Create reusable socket that can be binded to the same port without error msg
	int on = 1;
	result = setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(char *)&on,sizeof(on));

	// Error while modifiing socket
	if (result < 0)
	{
		perror("setsockopt() failed for SO_REUSEADDR");
		close(server_socket);
		return -1;
	}

	// Clear the server address
	memset(&server_address,0,sizeof(server_address));

	// Set the server address
	server_address.sin6_family = AF_INET6;
	server_address.sin6_addr = in6addr_any;
	server_address.sin6_port = htons(port_number);

	// Lets try to bind to port
	if ((result = bind(server_socket,(struct sockaddr *)&server_address,sizeof(server_address))) < 0)
	{
		perror("bind() failed");
    close(server_socket);
		return EXIT_FAILURE;
	}

	// Lets try to listen,
	if ((result = listen(server_socket,MAX_CONNECTIONS)) < 0)
	{
		perror("listen() failed");
    close(server_socket);
		return EXIT_FAILURE;
	}

	// Endlessly accept connections and decode them then reply
	while (MAGIC_NUMBER)
	{
		// Waiting here to accept new connection from client
		int incomming_socket = accept(server_socket,(struct sockaddr *)&server_address_client,&sa_client_len);

		// Check if acceted socket is okay
		if (incomming_socket > 0)
		{
			// Create process to handle client
			result = fork();

			// Check if this is main process
			if (result != 0)
			{
				// Close the connection for main process
				close(incomming_socket);

				// If child ends close him
				signal(SIGCHLD,SIG_IGN);

				if (result < 0)
				{
					fprintf(stderr, "Fork failed to create new process\n");
					exit(EXIT_FAILURE);
				}

				// main process jump to accept next one
				continue;
			}

			// Client is closing socket
			close(server_socket);

			// Endless loop
			while(MAGIC_NUMBER)
			{
				// We accepted token so now we can communicate with the other socket
				int res = 0;
				res = recv(incomming_socket,buff,sizeof(buff),0);

				// Error while receiving
				if (res < 0)
				{
					// Close socket
					close(incomming_socket);
					// Client has disconnected
					if (errno == ECONNRESET)
					{
						exit(EXIT_SUCCESS);
					}
					exit(EXIT_FAILURE);
				}
				// Client has successfully disconnected
				else if (res == 0)
				{
					close(incomming_socket);
					exit(EXIT_SUCCESS);
				}
				// Message is Okay
				else
				{
					char tmp_buff[BUFF_LEN];

					// Determinate GET request
					result = determinateGetRequest(buff,&accept_type,&refresh_rate);

	        // Client is requesting hostname
	        if (result == RQ_HOSTNAME)
	        {
	          // Gethostname
	          if (getNameOfHost(tmp_buff) != 0)
	          {
	            // Generate response
	            generateResponse(buff,ERROR_SERVER,INTERNAL_SERVER_ERROR,accept_type,refresh_rate);

	            // Send response
	            if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	            {
								// If error occured say it
	              perror("send() failed");
	            }
							close(incomming_socket);
							exit(EXIT_FAILURE);
	          }

	          // Generate response
	          generateResponse(buff,result,tmp_buff,accept_type,refresh_rate);

	          // Send response
	          if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	          {
	            perror("send() failed");
	          }
	        }
	        // Client is requesting cpu name
	        else if (result == RQ_CPU_NAME)
	        {
	          // Get cpu_name
	          if (getCpuName((tmp_buff)) != 0)
	          {
	            // Generate response
	            generateResponse(buff,ERROR_SERVER,INTERNAL_SERVER_ERROR,accept_type,refresh_rate);

	            // Send response
	            if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	            {
	              perror("send() failed");
	            }
							close(incomming_socket);
							exit(EXIT_FAILURE);
	          }

	          // Generate response
	          generateResponse(buff,result,tmp_buff,accept_type,refresh_rate);

	          // Send response
	          if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	          {
	            perror("send() failed");
	          }
	        }
	        // Client is requesting load of cpu
	        else if (result == RQ_CPU_LOAD)
	        {
	          // Get cpu usage
	          if ((result = getCpuLoad()) < 0)
	          {
	            // Generate response
	            generateResponse(buff,ERROR_SERVER,INTERNAL_SERVER_ERROR,accept_type,refresh_rate);

	            // Send response
	            if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	            {
	              perror("send() failed");
	            }
							close(incomming_socket);
							exit(EXIT_FAILURE);
	          }

	          memset(&tmp_buff,0,sizeof(tmp_buff));

	          sprintf(tmp_buff,"%d%%",result);

	          // Generate response
	          generateResponse(buff,RQ_CPU_LOAD,tmp_buff,accept_type,refresh_rate);

	          // Send response
	          if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	          {
	            perror("send() failed");
	          }
	        }
	        // Client is requesting load of cpu with refresh rate
	        else if (result == RQ_CPU_LOAD_REFRESH)
	        {
						// Get cpu usage
						if ((result = getCpuLoad()) < 0)
						{
							// Generate response
							generateResponse(buff,ERROR_SERVER,INTERNAL_SERVER_ERROR,accept_type,refresh_rate);

							// Send response
							if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
							{
								perror("send() failed");
							}
							close(incomming_socket);
							exit(EXIT_FAILURE);
						}

						memset(&tmp_buff,0,sizeof(tmp_buff));

						sprintf(tmp_buff,"%d%%",result);

						// Generate response
						generateResponse(buff,RQ_CPU_LOAD_REFRESH,tmp_buff,accept_type,refresh_rate);

						// Send response
						if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
						{
							perror("send() failed");
						}
	        }
					else if (result == ERROR_GET)
					{
						// Generate response
						generateResponse(buff,ERROR_GET,NOT_GET,accept_type,refresh_rate);

						// Send response
						if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
						{
							perror("send() failed");
						}
					}
					else if (result == ERROR_BAD_REQUEST)
					{
						// Generate response
						generateResponse(buff,ERROR_BAD_REQUEST,BAD_REQUEST,accept_type,refresh_rate);

						// Send response
						if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
						{
							perror("send() failed");
						}
					}
					else if (result == ERROR_HTTP)
					{
						// Generate response
						generateResponse(buff,ERROR_HTTP,INTERNAL_SERVER_HTTP_NOT_SUPPORTED,accept_type,refresh_rate);

						// Send response
						if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
						{
							perror("send() failed");
						}
						// We need to exit because the http version might be 1.0 which dont close connection on its own
						close(incomming_socket);
						exit(EXIT_SUCCESS);
					}
					// Client requested something I dont understand
	        else
	        {
	          // Generate response
	          generateResponse(buff,ERROR_SERVER,INTERNAL_SERVER_ERROR,accept_type,refresh_rate);

	          // Send response
	          if ((result = send(incomming_socket,buff,strlen(buff),0)) < 0)
	          {
	            perror("send() failed");
	          }
	        }
				}
			}
		}
		// Error while accepting socket
		else
		{
			perror("accept() failed");
		}
	}

  return 0;
}
