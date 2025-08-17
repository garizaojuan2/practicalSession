#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) { fprintf(stderr, "Uso: %s <inicio> <fin>\n", argv[0]); return 1; }
    long long a = atoll(argv[1]), b = atoll(argv[2]);
    if (b < a) { long long t=a; a=b; b=t; }

    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) { // hijo
        close(fd[0]);
        long long s = 0;
        for (long long i = a; i <= b; ++i) s += i;
        if (write(fd[1], &s, sizeof(s)) != sizeof(s)) perror("write");
        close(fd[1]);
        _exit(0);
    } else {        // padre
        close(fd[1]);
        long long s = 0;
        if (read(fd[0], &s, sizeof(s)) != sizeof(s)) perror("read");
        close(fd[0]);
        waitpid(pid, NULL, 0);
        printf("%lld\n", s);
    }
    return 0;
}
