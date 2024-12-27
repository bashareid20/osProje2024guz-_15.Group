void execute_command(char **args) {
    pid_t pid;
    int in_redirect_index = -1;

    // Giriş yönlendirme sembolünü bul
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            in_redirect_index = i;
            break;
        }
    }

    if (in_redirect_index != -1) {
        if (args[in_redirect_index + 1] == NULL) {
            fprintf(stderr, "Giriş dosyası belirtilmedi.\n");
            return;
        }
    }
    int pipefd[2];
    pipe(pipefd); // pipefd[0]: read ucu, pipefd[1]: write ucu

    dup2(pipefd[1], STDOUT_FILENO); // stdout'u borunun yazma ucuna yönlendir
    dup2(pipefd[0], STDIN_FILENO);  // stdin'i borunun okuma ucuna yönlendir


    pid = fork();
    if (pid < 0) {
        perror("Fork hatası");
    } else if (pid == 0) {
        // Çocuk işlem
        if (in_redirect_index != -1) {
            FILE *input_file = fopen(args[in_redirect_index + 1], "r");
            if (!input_file) {
                perror("Giriş dosyası açılamadı");
                exit(EXIT_FAILURE);
            }
            dup2(fileno(input_file), STDIN_FILENO);
            fclose(input_file);

            // Yönlendirme işaretçisini ve dosya adını kaldır
            args[in_redirect_index] = NULL;
        }

        if (execvp(args[0], args) < 0) {
            perror("Komut çalıştırılamadı");
            exit(EXIT_FAILURE);
        }
    } else {
        // Ana işlem
        wait(NULL);
    }
}
