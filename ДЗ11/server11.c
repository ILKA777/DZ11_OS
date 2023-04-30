#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int server;
    int client1;
    int client2;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    int addresslen = sizeof(server_addr);
    char buffer[BUFFER_SIZE];

    server = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server, (struct sockaddr *)&server_addr, addresslen);
    listen(server, 2);

    printf("Server is listening on port %d...\n", PORT);

    client1 = accept(server, (struct sockaddr *)&client1_addr, (socklen_t *)&addresslen);
    printf("Client 1 connected\n");

    client2 = accept(server, (struct sockaddr *)&client2_addr, (socklen_t *)&addresslen);
    printf("Client 2 connected\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client1, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "The End") == 0) {
            send(client2, buffer, strlen(buffer), 0); // Send "The End" to client 2
            break;
        }

        send(client2, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(client2, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, "The End") == 0) {
            send(client1, buffer, strlen(buffer), 0); // Send "The End" to client 1
            break;
        }

        send(client1, buffer, strlen(buffer), 0);
    }

    close(client1);
    close(client2);
    close(server);

    printf("Server closed\n");

    return 0;
}

