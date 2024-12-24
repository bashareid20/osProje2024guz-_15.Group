#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 100

void print_prompt() {
    printf("> ");
    fflush(stdout);
}

int parse_input(char *input, char **args) {
    char *token;
    int count = 0;

    token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_ARG_SIZE - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL;

    return count;
}

void execute_command(char **args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork hatası");
    } else if (pid == 0) {
        // Çocuk işlem: komutu çalıştır
        if (execvp(args[0], args) < 0) {
            perror("Komut çalıştırılamadı");
            exit(EXIT_FAILURE);
        }
    } else {
        // Ana işlem: çocuk işlemi bekle
        wait(NULL);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_SIZE];

    while (1) {
        print_prompt();

        // Kullanıcı girişini oku
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // EOF veya hata
        }

        // Boş girişleri atla
        if (strcmp(input, "\n") == 0) {
            continue;
        }

        // Komutları ayrıştır
        int arg_count = parse_input(input, args);

        if (arg_count == 0) {
            continue; // Boş giriş
        }

        // "quit" komutu
        if (strcmp(args[0], "quit") == 0) {
            printf("Kabuk sonlandırılıyor...\n");
            break;
        }

        // Tekli komut icrası
        execute_command(args);
    }

    return 0;
}
