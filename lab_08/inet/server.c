#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/select.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET "server.soc"
#define MSG_SIZE 100
#define PORT 7770
#define MAX_CLIENTS 10

char *srvr_ans = "OK";

static int sock_fd;

void signint_handler(int sig) 
{
    close(sock_fd);
    unlink(SOCKET);
    exit(0);
}
// сервер обрабатывает сообщения от клиентов
int main(void) // socket, bind, listen, select, accept
{
	int clients[MAX_CLIENTS] = { 0 };
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); // домен имен аф инет, способ передачи, 
	
	if (sock_fd == -1)
	{
		perror("Socket");
		return -1;
	}
	
	signal(SIGINT, signint_handler);
	
	struct sockaddr_in addr = 
	{ 
		.sin_family = AF_INET, // пр-во имен
											  // Позволяет использовать сокету любой свободный адрес на компьютере
											  // сокет будет привязан ко всем локальным интерфейсам.
		.sin_addr.s_addr = htonl(INADDR_ANY),	    //Реализовать на отдельной машине работу распределённой системе.
											  		//Подключение к любому свободному айпи адресу
        .sin_port = htons(PORT)
	};
	
	if (bind(sock_fd, &addr, sizeof(addr)) == -1) // связывает дескриптор сокета с адресом
	{
		perror("Bind");
		return -1;
	}
	
	if (listen(sock_fd, MAX_CLIENTS) == -1) // сервер в режиме прослушивания
	{
		perror("Socket");
		return -1;
	}

	printf("Server addres %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	char msg[MSG_SIZE];
	fd_set rfds;
	struct sockaddr rcvr_name; 
	int retval;
	int len;
	
	FD_ZERO(&rfds); // Удаляет сокеты из набора
	FD_SET(sock_fd, &rfds); // Добавляем интересующий вас сокет в набор дескрипт-в для чтения данных 
	int maxfd = sock_fd;

	while(1)
	{
        retval = select(maxfd + 1, &rfds, NULL, NULL, NULL); // кол-во проверяемых дескрипт, адрес на набор деск-в для проверки на готовность к чтению,
															 // на запись, на наличие исклю ситуаций, таймаут 
		// мультиплексор - одновременно опрашивает мно-во соед
		// Все соединения опрашиваются одновременно, сокращается время блокировки
		// Мультиплексор выбирает первый готовый сокет.
		// Является аналогом многопоточной обработки
        if (retval < 0)
        	perror("Select");
       	else if (retval)
        {
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (clients[i] && FD_ISSET(clients[i], &rfds))
				{
					len = recv(clients[i], msg, MSG_SIZE - 1, 0);
					if (len == 0)
					{
						printf("Server closed connection.\n\n");
						FD_CLR(clients[i], &rfds);
						close(clients[i]);
						clients[i] = 0;
					}
					else
					{
						send(clients[i], srvr_ans, strlen(srvr_ans), 0);
						msg[len] = '\0';
						printf("Message from client: %s.\n\n", msg);
					}
				}
				else
					FD_SET(clients[i], &rfds);
			}

			if (FD_ISSET(sock_fd, &rfds))
			{
				for (int i = 0; i < MAX_CLIENTS; i++)
					if (clients[i] == 0)
					{   // Принимает соединение в ответ на запрос клиента и 
						// создает копию сокета для того, чтобы исходный сокет мог продолжать прослушивание
						clients[i] = accept(sock_fd, (struct sockaddr*) &addr, &len);
						FD_SET(clients[i], &rfds); // добавляем во мн-во дескрипторов для чтения
						if (maxfd < clients[i])
							maxfd = clients[i];
						printf("New connection, address = %s, port = %d.\n\n", inet_ntoa(addr.sin_addr), addr.sin_port);
						break;
					}
			}
			else
				FD_SET(sock_fd, &rfds);
		}	      
	}

	return 0;
}