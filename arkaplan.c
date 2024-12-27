#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_BG_PROCESSES 100

int bg_processes[MAX_BG_PROCESSES];
int bg_count = 0;

void handle_background_processes() {
    int status;
    for (int i = 0; i < bg_count; i++) {
        if (bg_processes[i] > 0) {
            pid_t result = waitpid(bg_processes[i], &status, WNOHANG);
            if (result > 0) {
                printf("[PID %d] retval: %d\n", bg_processes[i], WEXITSTATUS(status));
                fflush(stdout);
                bg_processes[i] = -1;
            }
        }
    }
}

int main() {
    char command[256];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) {
            continue;
        }

        if (strcmp(command, "quit") == 0) {
            printf("Waiting for all background processes to finish...\n");
            for (int i = 0; i < bg_count; i++) {
                if (bg_processes[i] > 0) {
                    waitpid(bg_processes[i], NULL, 0);
                }
            }
            printf("All background processes finished. Exiting...\n");
            break;
        }

        int is_background = 0;
        if (command[strlen(command) - 1] == '&') {
            is_background = 1;
            command[strlen(command) - 1] = 0;
            while (strlen(command) > 0 && command[strlen(command) - 1] == ' ') {
                command[strlen(command) - 1] = 0;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            if (strlen(command) > 0) {
                execlp("/bin/sh", "sh", "-c", command, NULL);
                perror("execlp failed");
            }
            exit(EXIT_FAILURE);
        } else {
            if (is_background) {
                bg_processes[bg_count++] = pid;
                printf("Process [PID %d] running in background.\n", pid);
            } else {
                waitpid(pid, NULL, 0);
            }
        }

        handle_background_processes();
    }

    return 0;
}

