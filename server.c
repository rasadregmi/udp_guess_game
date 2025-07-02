// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define MAX_PLAYERS 100
#define BUFSIZE 1024
#define TIMEOUT_SECONDS 60

typedef struct {
    char name[50];
    int attempts;
    time_t last_active;
    struct sockaddr_in addr;
    int active;
} Player;

Player players[MAX_PLAYERS];
int numPlayers = 0;
int secretNumber;

FILE *scoreFile;
int sockfd;
struct sockaddr_in serverAddr;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
volatile sig_atomic_t running = 1;

int is_same_client(struct sockaddr_in *a, struct sockaddr_in *b) {
    return (a->sin_addr.s_addr == b->sin_addr.s_addr && a->sin_port == b->sin_port);
}

int find_or_add_player(struct sockaddr_in *clientAddr, char *name) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < numPlayers; ++i) {
        if (is_same_client(&players[i].addr, clientAddr)) {
            players[i].last_active = time(NULL);
            pthread_mutex_unlock(&lock);
            return i;
        }
    }
    if (numPlayers >= MAX_PLAYERS) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    int id = numPlayers++;
    strncpy(players[id].name, name, sizeof(players[id].name) - 1);
    players[id].addr = *clientAddr;
    players[id].attempts = 0;
    players[id].last_active = time(NULL);
    players[id].active = 1;
    pthread_mutex_unlock(&lock);
    return id;
}

void broadcast(const char *message) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < numPlayers; ++i) {
        if (players[i].active) {
            sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&players[i].addr, sizeof(players[i].addr));
        }
    }
    pthread_mutex_unlock(&lock);
}

void reset_game() {
    pthread_mutex_lock(&lock);
    secretNumber = rand() % 100 + 1;
    printf("🎯 New secret number: %d\n", secretNumber);
    for (int i = 0; i < numPlayers; i++) {
        players[i].attempts = 0;
    }
    pthread_mutex_unlock(&lock);
}

void log_score_to_file(const char *winner, int attempts) {
    scoreFile = fopen("scoreboard.txt", "a");
    if (scoreFile != NULL) {
        time_t now = time(NULL);
        fprintf(scoreFile, "%s guessed the number in %d attempts on %s", winner, attempts, ctime(&now));
        fclose(scoreFile);
    }
}

void remove_inactive_players() {
    time_t now = time(NULL);
    pthread_mutex_lock(&lock);
    for (int i = 0; i < numPlayers; ++i) {
        if (players[i].active && difftime(now, players[i].last_active) > TIMEOUT_SECONDS) {
            printf("⏰ Removing inactive player: %s\n", players[i].name);
            const char *kickMsg = "⏰ You have been removed due to inactivity.\nType ./client to start a new game.";
            sendto(sockfd, kickMsg, strlen(kickMsg), 0,
                   (struct sockaddr *)&players[i].addr, sizeof(players[i].addr));
            players[i].active = 0;
        }
    }
    pthread_mutex_unlock(&lock);
}

void *timeout_checker(void *arg) {
    while (running) {
        sleep(10);
        remove_inactive_players();
    }
    return NULL;
}

void handle_shutdown(int signo) {
    running = 0;
    printf("\n🛑 Server shutting down...\n");
    close(sockfd);
    exit(0);
}

int kbhit(void) {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
}

void *admin_console(void *arg) {
    char cmd[BUFSIZE];
    printf("Admin console ready. Type SHOW_SCOREBOARD + Enter\n");
    while (running) {
        if (kbhit()) {
            if (fgets(cmd, sizeof(cmd), stdin) == NULL) continue;
            cmd[strcspn(cmd, "\r\n")] = 0;
            if (strcmp(cmd, "SHOW_SCOREBOARD") == 0) {
                pthread_mutex_lock(&lock);
                printf("\n📊 Scoreboard:\n");
                for (int i = 0; i < numPlayers; i++) {
                    if (players[i].active)
                        printf(" - %s: %d attempts\n", players[i].name, players[i].attempts);
                }
                pthread_mutex_unlock(&lock);
            } else if (strlen(cmd) > 0) {
                printf("⚠️  Only use valid commands like: SHOW_SCOREBOARD\n");
            }
        }
        usleep(100000);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handle_shutdown);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("Socket failed"); exit(1); }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed"); exit(1);
    }

    reset_game();

    pthread_t adminThread, timeoutThread;
    pthread_create(&adminThread, NULL, admin_console, NULL);
    pthread_create(&timeoutThread, NULL, timeout_checker, NULL);

    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFSIZE];

    printf("Server started. Waiting for clients...\n");

    while (running) {
        memset(buffer, 0, BUFSIZE);
        ssize_t n = recvfrom(sockfd, buffer, BUFSIZE - 1, 0,
                             (struct sockaddr *)&clientAddr, &addrLen);
        if (n < 0) continue;
        buffer[n] = '\0';

        if (strncmp(buffer, "REGISTER:", 9) == 0) {
            char name[50];
            strncpy(name, buffer + 9, sizeof(name) - 1);
            int id = find_or_add_player(&clientAddr, name);
            if (id < 0) {
                const char *msg = "❌ Server full. Try again later.";
                sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&clientAddr, addrLen);
                continue;
            }
            char msg[100];
            snprintf(msg, sizeof(msg), "✅ Welcome %s! Start guessing...", players[id].name);
            sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&clientAddr, addrLen);
            continue;
        }

        int id = find_or_add_player(&clientAddr, "Unknown");
        if (id < 0 || !players[id].active) continue;

        int guess = atoi(buffer);
        pthread_mutex_lock(&lock);
        players[id].attempts++;
        pthread_mutex_unlock(&lock);

        char reply[BUFSIZE];
        if (guess < secretNumber) {
            snprintf(reply, sizeof(reply), "🔻 Too low (%d attempts)", players[id].attempts);
            sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&clientAddr, addrLen);
        } else if (guess > secretNumber) {
            snprintf(reply, sizeof(reply), "🔺 Too high (%d attempts)", players[id].attempts);
            sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&clientAddr, addrLen);
        } else {
            snprintf(reply, sizeof(reply), "🎉 %s guessed correctly in %d attempts!", players[id].name, players[id].attempts);
            broadcast(reply);
            log_score_to_file(players[id].name, players[id].attempts);
            broadcast("🔁 New game starting!");
            reset_game();
        }
    }

    close(sockfd);
    return 0;
}