#include <sys/types.h> 
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERV_SOCKET "srvr_sock.soc"
#define SOCKET "socket.soc"
#define MSG_SIZE 100

int main(void)
{
	int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0); // пр-во имен, способ передачи, протокол по умолчанию (юдп)
	if  (sock_fd == -1)
	{
		perror("Socket");
		return -1;
	}
	
	struct sockaddr srvr_addr; // адрес сервера
	srvr_addr.sa_family = AF_UNIX; // пр-во имен машины
	strcpy(srvr_addr.sa_data, SERV_SOCKET);

	struct sockaddr addr;
	addr.sa_family = AF_UNIX;
	strcpy(addr.sa_data, SOCKET);
	
	if (bind(sock_fd, &addr, sizeof(addr)) == -1) // связываем структуру сокета клиента и адреса клиента
	{
		perror("Bind");
		return -1;
	}
	
	char msg[MSG_SIZE];
	int cli_pid = getpid();
	snprintf(msg, MSG_SIZE - 1, "hello from process with PID = %d", cli_pid);

	int namelen;
	struct sockaddr rcvr_name;
	sendto(sock_fd, msg, strlen(msg), 0, &srvr_addr, strlen(srvr_addr.sa_data) + sizeof(srvr_addr.sa_family)); // отправляет по адресу сервера
	int len = recvfrom(sock_fd, msg, MSG_SIZE - 1, 0, &rcvr_name, &namelen); // ждем сообщение от сервера
	msg[len] = '\0';

    printf("[client #%d] Message from server: %s.\n", cli_pid, msg);

	close(sock_fd);
    unlink(SOCKET);
	
	return 0;
}