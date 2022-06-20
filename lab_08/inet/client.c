#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SERV_SOCKET "my_socket.soc"
#define SOCKET "client.soc"
#define MSG_SIZE 100
#define PORT 7770
static int sock_fd;

void sigint_handler(int sig) 
{
    close(sock_fd);
    unlink(SOCKET);
    exit(0);
}

int main()
{
	struct hostent *server = gethostbyname("localhost");
	if (server == NULL)
 	{
  	   printf("Hostent");
 	   return -1;
 	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if  (sock_fd == -1)
	{
		perror("Socket");
		return -1;
	}
	
	signal(SIGINT, sigint_handler);
	
	struct sockaddr_in serv_addr = 
	{
		.sin_family = AF_INET, // Указываем семейство адресов AF_INET
		.sin_port = htons(PORT), // Порт
		.sin_addr.s_addr = htonl(INADDR_ANY) 	// Позволяет использовать сокету любой свободный адрес на компьютере
												// сокет будет привязан ко всем локальным интерфейсам.
	};
	
	char msg[MSG_SIZE];
	int namelen;
	struct sockaddr rcvr_name;
	if (connect(sock_fd, &serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("Connect");
		return -1;
	}

	int cli_pid = getpid();


	snprintf(msg, MSG_SIZE - 1, "hello from process with PID = %d", cli_pid);
	send(sock_fd, msg, strlen(msg), 0);
	int len = recv(sock_fd, msg, MSG_SIZE - 1, 0);
	msg[len] = '\0';
	printf("[client #%d] Message from server: %s.\n", cli_pid, msg);
	
	return 0;
}