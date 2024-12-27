#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>

/* Makro tanımları */
#define TRUE  1
#define FALSE 0

#define LIMIT 256    /* max token sayısı */
#define SATIR 1024   /* kullanıcıdan okunacak max satır uzunluğu */

/* Renk kodları (isteğe bağlı) */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* Mevcut dizin pointer'ı ve environ */
extern char* currentDirectory;
extern char** environ;

/* Yerleşik (Built-in) komutların fonksiyon prototipleri */
int shell_cd(char **args);
int shell_help(char **args);
int shell_quit(char **args);

/* Yürütme ile ilgili fonksiyonlar */
int calistir(char **args, int background);
int arkaPlandaCalistir(char **args);

/* Dosya yönlendirme fonksiyonları */
void dosyaInput(char *args[], char* inputFile);
void dosyaOutput(char *args[], char* outputFile);

/* Pipe fonksiyonu */
void pipeFonk(char *args[], char *cmdNext, char *paramNext);

/* Prompt ve açılış ekranı */
void Prompt(void);
void boslukBirak(void);
void acilis(void);

/* Sinyal yakalama (SIGCHLD) */
void sig_chld(int signo);

/* Built-in komut sayısı ve komut yorumlayıcı */
int builtin_sayisi(void);
int komutYorumla(char *args[]);

#endif /* SHELL_H */
