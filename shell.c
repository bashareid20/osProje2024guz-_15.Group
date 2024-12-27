/* 
	BASHAR EID G221210585    2.C
	SHHADA AL HARIRI G221210574     2.A 
	Hasan Abdulhay G201210585 1.C
	Abdullah mulle hamid B211210577 1.A  
	ABDULRAHMAN ALJANADI G211210551  2.B
	--------------------------------------------------------------------------
 * Basit bir kabuk (shell) uygulaması:
 *  - Prompt: >
 *  - Built-in komutlar: cd, help, quit
 *  - Noktalı virgülle (;) girilen birden fazla komutu sırayla yürütme
 *  - Tekli veya çoklu komutu (pipe, redirect vb.) işleme
 *  - Arka planda çalıştırma (&)
 ****************************************************************************/

#include "shell.h"

/* Global değişkenler */
char* currentDirectory = NULL;

/* Built-in komutlar ve işlev işaretçileri */
char *builtin_komutlar[] = {
    "cd",
    "help",
    "quit"
};

int (*builtin_func[])(char**) = {
    &shell_cd,
    &shell_help,
    &shell_quit
};

/* Built-in komut sayısı */
int builtin_sayisi(void)
{
    return sizeof(builtin_komutlar) / sizeof(char *);
}

/* cd komutu */
int shell_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}

/* help komutu */
int shell_help(char **args)
{
    (void)args; /* kullanılmadıysa uyarı bastırmamak için */
    printf("KOC SHELL Yardim\n");
    printf("Built-in komutlar:\n");
    for (int i = 0; i < builtin_sayisi(); i++) {
        printf("  %s\n", builtin_komutlar[i]);
    }
    printf("Diğer komutlar için man sayfalarına bakabilirsiniz.\n");
    return 1;
}

/* quit komutu */
int shell_quit(char **args)
{
    (void)args;
    int status;
    /* Arka planda çalışan tüm süreçlerin bitmesini bekleyip çık */
    while (!waitpid(-1, &status, WNOHANG)) { }
    exit(0);
}

/* SIGCHLD handler */
void sig_chld(int signo)
{
    (void)signo; /* kullanılmadıysa uyarı bastırmamak için */
    int status;
    pid_t childPid = waitpid(-1, &status, WNOHANG);
    if (childPid > 0) {
        if (WIFEXITED(status)) {
            int retval = WEXITSTATUS(status);
            printf("[%d] retval: %d\n", childPid, retval);
        }
    }
}

/* Prompt */
void Prompt(void)
{
    char hostn[1024] = "";
    gethostname(hostn, sizeof(hostn));
    printf(KNRM "%s@%s:" KWHT KBLU "%s > " KWHT,
           getenv("LOGNAME"),
           hostn,
           getcwd(currentDirectory, 1024));
    fflush(stdout);
}

/* Açılış ekranı */
void acilis(void)
{
    for (int i = 0; i < 40; i++)
        printf("=");
    printf("\n");

    boslukBirak();
    printf("%s%s%s\n", "=", KCYN "            15.Gruop SHELL               ", KWHT "=");
    boslukBirak();

    for (int i = 0; i < 40; i++)
        printf("=");
    printf("\n");
}

/* Bosluk ekranda 2 satır, kenarları '=' */
void boslukBirak(void)
{
    for (int i = 0; i < 2; i++) {
        printf("=");
        for (int j = 0; j < 38; j++) {
            printf(" ");
        }
        printf("=\n");
    }
}

/* Pipe fonksiyonu */
void pipeFonk(char *args[], char *cmdNext, char *paramNext)
{
    pid_t pid1, pid2;
    int pipefd[2];
    // Burada cmdNext ve paramNext, ikinci komutun (ör. increment) argümanları
    // argv2 -> ["increment", ..., NULL]
    char *argv2[] = { cmdNext, paramNext, NULL };

    if (pipe(pipefd) < 0) {
        perror("pipe");
        return;
    }

    // 1. Child (echo)
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork pid1");
        return;
    }
    if (pid1 == 0) {
        // Child1: pipe'a yazacak, stdin normal
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // Komutu çalıştır: echo 12
        execvp(args[0], args);
        perror("execvp child1");
        _exit(1);  // Hata durumunda sonlansın
    }

    // 2. Child (increment)
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork pid2");
        return;
    }
    if (pid2 == 0) {
        // Child2: pipe'den okuyacak, stdout normal
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);

        // Komutu çalıştır: increment
        execvp(argv2[0], argv2);
        perror("execvp child2");
        _exit(1);
    }

    // Parent: pipe uçlarını kapatıp child'ları bekle
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}


/* Input redirect */
void dosyaInput(char *args[], char* inputFile)
{
    if (access(inputFile, F_OK) == -1) {
        printf("shell: %s bulunamadi\n", inputFile);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        printf("Child olusturulamadi\n");
        return;
    }
    if (pid == 0) {
        int fd = open(inputFile, O_RDONLY, 0600);
        dup2(fd, STDIN_FILENO);
        close(fd);
        if (execvp(args[0], args) == -1) {
            printf("exec hata\n");
            kill(getpid(), SIGTERM);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
}

void dosyaOutput(char *args[], char *outputFile)
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("Fork hatası");
        return;
    }

    if (pid == 0) { 
        // Çocuk proses: Çıktıyı dosyaya yönlendir
        int fd = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
        if (fd < 0) {
            perror("Dosya açılamadı");
            _exit(EXIT_FAILURE);
        }

        // Standart çıktıyı dosyaya yönlendir
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 hatası");
            close(fd);
            _exit(EXIT_FAILURE);
        }
        close(fd);

        // Komutu çalıştır
        if (execvp(args[0], args) < 0) {
            perror("execvp hatası");
            _exit(EXIT_FAILURE);
        }
    } else {
        // Ebeveyn: Çocuk süreci bekle
        waitpid(pid, NULL, 0);
    }
}

/* Arka planda komut çalıştır */
int arkaPlandaCalistir(char **args)
{
    struct sigaction act;
    act.sa_handler = sig_chld;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        fprintf(stderr, "sigaction failed\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            printf("Komut bulunamadi\n");
            kill(getpid(), SIGTERM);
        }
    } else if (pid < 0) {
        perror("shell");
    } else {
        printf("Proses PID:%d olarak baslatildi.\n", pid);
    }
    return 1;
}

/* Normal veya arka planda komut çalıştırma */
int calistir(char **args,int background)
{
    if (!background) {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                printf("Komut bulunamadi\n");
                kill(getpid(), SIGTERM);
            }
        } else if (pid < 0) {
            perror("shell");
        } else {
            waitpid(pid, NULL, 0);
        }
    } else {
        arkaPlandaCalistir(args);
    }
    return 1;
}

int komutYorumla(char *args[])
{
    int i = 0, j = 0;
    int arkaPlan = 0;
    char *args_split[LIMIT];

    // İlk olarak '>' işareti veya diğer operatörlere kadar argümanları ayrıştır
    while (args[j] != NULL) {
        if ((strcmp(args[j], ">") == 0) || 
            (strcmp(args[j], "<") == 0) ||
            (strcmp(args[j], "&") == 0) ||
            (strcmp(args[j], "|") == 0)) 
        {
            break;
        }
        args_split[j] = args[j];
        j++;
    }
    args_split[j] = NULL;  // Komutu sonlandır

    if (args[0] == NULL) {
        // Eğer komut boşsa devam et
        return 1;
    }

    // Yerleşik komut kontrolü
    for (int m = 0; m < builtin_sayisi(); m++) {
        if (strcmp(args[0], builtin_komutlar[m]) == 0) {
            (*builtin_func[m])(args);
            return 1;
        }
    }

    // Diğer operatörler kontrolü
    while (args[i] != NULL && arkaPlan == 0) {
        if (strcmp(args[i], "&") == 0) {
            arkaPlan = 1;
        }
        else if (strcmp(args[i], ">") == 0) {
            // Çıkış yönlendirme tespit edildi
            if (args[i + 1] == NULL) {
                printf("Eksik dosya adı: >\n");
                return 1;
            }

            // Çıkış yönlendirme işlemi
            args[i] = NULL;  // '>' işaretini ve sonrasını ayır
            dosyaOutput(args_split, args[i + 1]);
            return 1;
        }
        else if (strcmp(args[i], "<") == 0) {
            // Giriş yönlendirme tespit edildi
            if (args[i + 1] == NULL) {
                printf("Eksik dosya adı: <\n");
                return 1;
            }

            // Giriş yönlendirme işlemi
            dosyaInput(args_split, args[i + 1]);
            return 1;
        }
        else if (strcmp(args[i], "|") == 0) {
            // Pipe tespit edildi
            if (args[i + 1] == NULL) {
                printf("Eksik komut: |\n");
                return 1;
            }

            // Pipe işlemi
            pipeFonk(args_split, args[i + 1], args[i + 2]);
            return 1;
        }
        i++;
    }

    args_split[i] = NULL;  // Sonlandır
    calistir(args_split, arkaPlan);
    return 1;
}


/* main */
int main(int argc, char *argv[], char *envp[])
{
    (void)argc; 
    (void)argv;

    /* environ */
    environ = envp;

    /* Karşılama ve değişken hazırlığı */
    acilis();
    static char line[SATIR];
    char *tokens[LIMIT];

    int status = 1;

    while (status) {
        Prompt();

        /* Satırı oku */
        if (!fgets(line, SATIR, stdin)) {
            continue;
        }
        /* Satır boş mu */
        if (line[0] == '\n') {
            continue;
        }

        /* 1) Önce ';' karakterine göre böl */
        char *parca = strtok(line, ";");
        while (parca) {
            /* 2) Her parça tekrar boşluk/tab ile ayrıştırılacak */
            char *saveptr;
            tokens[0] = strtok_r(parca, " \t\n", &saveptr);
            if (tokens[0] == NULL) {
                // boş komutsa geç
                parca = strtok(NULL, ";");
                continue;
            }
            int idx = 1;
            while ((tokens[idx] = strtok_r(NULL, " \t\n", &saveptr)) != NULL) {
                idx++;
            }

            /* 3) komutu yorumla/çalıştır */
            komutYorumla(tokens);

            /* Sıradaki parçaya geç */
            parca = strtok(NULL, ";");
        }
    }
    return 0;
}
