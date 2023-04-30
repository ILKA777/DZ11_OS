#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_sd;

    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    client = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Connected to server %s:%d\n", argv[1], PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(client, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        max_sd = (client > STDIN_FILENO) ? client : STDIN_FILENO;

        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            printf("Enter message: \n");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            send(client, buffer, strlen(buffer), 0);

            if (strcmp(buffer, "The End") == 0) {
                break;
            }
        }

        if (FD_ISSET(client, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            recv(client, buffer, BUFFER_SIZE, 0);
            printf("Received: %s\n", buffer);

            if (strcmp(buffer, "The End") == 0) {
                break;
            }
        }
    }

    close(client);

    printf("Client closed\n");

    return 0;
}
