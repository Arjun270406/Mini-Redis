#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "hashtable.h"

#define PORT 6380
#define BUFFER_SIZE 1024

HashTable *store;
pthread_mutex_t lock;

void trim_newline(char *str) {
    int len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
    if (len > 0 && str[len - 1] == '\r') {
        str[len - 1] = '\0';
    }
}

void handle_command(char *line, char *response) {
    char cmd[16], key[256], value[512];
    int n = sscanf(line, "%15s %255s %511[^\n]", cmd, key, value);

    if (n < 1) {
        strcpy(response, "ERROR\n");
        return;
    }

    if (strcasecmp(cmd, "SET") == 0 && n == 3) {
        pthread_mutex_lock(&lock);
        ht_set(store, key, value);
        pthread_mutex_unlock(&lock);
        strcpy(response, "OK\n");
    }
    else if (strcasecmp(cmd, "GET") == 0 && n >= 2) {
        pthread_mutex_lock(&lock);
        char *val = ht_get(store, key);
        if (val != NULL) {
            snprintf(response, BUFFER_SIZE, "%s\n", val);
        } else {
            strcpy(response, "NULL\n");
        }
        pthread_mutex_unlock(&lock);
    }
    else if (strcasecmp(cmd, "DEL") == 0 && n >= 2) {
        pthread_mutex_lock(&lock);
        int found = ht_del(store, key);
        pthread_mutex_unlock(&lock);
        strcpy(response, found ? "OK\n" : "NOT FOUND\n");
    }
    else if (strcasecmp(cmd, "EXISTS") == 0 && n >= 2) {
        pthread_mutex_lock(&lock);
        int found = ht_exists(store, key);
        pthread_mutex_unlock(&lock);
        strcpy(response, found ? "1\n" : "0\n");
    }
    else {
        strcpy(response, "ERROR unknown command\n");
    }
}

void *client_thread(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        trim_newline(buffer);

        if (strcasecmp(buffer, "QUIT") == 0) {
            break;
        }

        handle_command(buffer, response);
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
    return NULL;
}

int main() {
    store = ht_create();
    pthread_mutex_init(&lock, NULL);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("mini redis server running on port %d\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        int *arg = malloc(sizeof(int));
        *arg = client_socket;

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, arg);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
