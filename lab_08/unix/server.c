#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/un.h>

#define SOCKET "srvr_sock.soc"
#define MSG_SIZE 256

char *srvr_ans = "OK";

static int sock_fd;

void sigint_handler(int sig) // 
{
    close(sock_fd);
    unlink(SOCKET);
    exit(0);
}

int main(void)
{
	sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0); // создаем дескриптор сокета, способ передачи, протокол по умолчанию (UDP)
	if (sock_fd == -1)
	{
		perror("Socket");
		return -1;
	}
	
	struct sockaddr addr; // Адрес сокета
	addr.sa_family = AF_UNIX; // Указываем семейство адресов AF_UNIX
	strcpy(addr.sa_data, SOCKET); // Записываем в поле адреса имя файла
	
	if (bind(sock_fd, &addr, sizeof(addr)) == -1) // сокет, указатель на струк сок адр - связывает дескриптор сокета с адресом (файлом)
	{ 											  // После вызова bind программа-сервер становится доступна для соединения по заданному адресу (имени файла). 
		perror("Bind");
		return -1;
	}
	
	char msg[MSG_SIZE];
	struct sockaddr rcvr_name;
	int namelen = sizeof(rcvr_name);
	
	signal(SIGINT, sigint_handler); // бинд обработчика сигнала
	
	while(1) // в вечном цикле ждем сообщение от клиента
	{
						// сокет сервера, буфер сообщения, длина сообщения
		int len = recvfrom(sock_fd, msg, MSG_SIZE - 1, 0, &rcvr_name, &namelen); // получить информацию от клиента (можно без установления соединения)
		sendto(sock_fd, srvr_ans, strlen(srvr_ans), 0, &rcvr_name, sizeof(rcvr_name)); // сокет клиента, сообщение, длина сообщения, адрес сервера
		msg[len] = '\0';
        printf("Message from client: %s.\n", msg);
	}
	
	return 0;
}