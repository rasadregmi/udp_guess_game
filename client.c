// client.c - Final version with real-time display and prompt cleaning
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFSIZE 1024

int sockfd;
struct sockaddr_in serverAddr;
volatile int running = 1;

// Color codes
#define COLOR_RESET  "\033[0m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_RED    "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_CYAN   "\033[36m"

void handle_shutdown(int signo) {
    running = 0;
    printf("\n🛑 Client exiting...\n");
    close(sockfd);
    exit(0);
}

// Thread to receive messages in real-time
void *receive_thread(void *arg) {
    char buffer[BUFSIZE];
    while (running) {
        ssize_t n = recvfrom(sockfd, buffer, BUFSIZE - 1, 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';

            // Clear current line and print message
            printf("\r\033[K");  // clear line
            if (strstr(buffer, "removed due to inactivity")) {
                printf(COLOR_RED "🧠 Server: %s\n" COLOR_RESET, buffer);
                raise(SIGINT);  // exit gracefully
                break;
            }

            printf(COLOR_CYAN "🧠 Server: %s\n" COLOR_RESET, buffer);
            printf(COLOR_YELLOW "Enter your guess (1-100): " COLOR_RESET);
            fflush(stdout);
        }
    }
    return NULL;
}

int main() {
    signal(SIGINT, handle_shutdown);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    char name[50];
    printf(COLOR_CYAN "Enter your name: " COLOR_RESET);
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    // Send registration message
    char registerMsg[64];
    snprintf(registerMsg, sizeof(registerMsg), "REGISTER:%s", name);
    sendto(sockfd, registerMsg, strlen(registerMsg), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Receive welcome message
    char buffer[BUFSIZE];
    ssize_t n = recvfrom(sockfd, buffer, BUFSIZE - 1, 0, NULL, NULL);
    if (n > 0) {
        buffer[n] = '\0';
        printf(COLOR_GREEN "🧠 Server: %s\n" COLOR_RESET, buffer);
    }

    // Launch receiver thread
    pthread_t recvThread;
    pthread_create(&recvThread, NULL, receive_thread, NULL);

    // Input loop
    while (running) {
        printf(COLOR_YELLOW "Enter your guess (1-100): " COLOR_RESET);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) continue;
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    }

    handle_shutdown(0);
    return 0;
}
