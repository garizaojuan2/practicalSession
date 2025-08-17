#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    // 1) Leer números de stdin en un arreglo dinámico
    size_t cap = 1024, n = 0;
    long long *a = malloc(cap * sizeof(*a));
    if (!a) { perror("malloc"); return 1; }

    while (1) {
        long long x;
        int r = scanf("%lld", &x);
        if (r == 1) {
            if (n == cap) { cap *= 2; a = realloc(a, cap * sizeof(*a)); if (!a) { perror("realloc"); return 1; } }
            a[n++] = x;
        } else if (r == EOF) break;
        else { // token no numérico: saltar separador
            int c;
            do { c = getchar(); } while (c != EOF && c!=' ' && c!='\n' && c!='\t' && c!='\r');
        }
    }
    if (n == 0) { fprintf(stderr, "(adder) No recibí números por stdin.\n"); free(a); return 1; }

    // 2) Cantidad de hijos = floor(n/3) (al menos 1, como pide el enunciado)
    size_t k = n / 3;
    if (k < 1) k = 1;
    if (k > n) k = n;

    // 3) Crear k pipes e hijos; cada hijo suma su segmento
    int (*pipes)[2] = malloc(k * sizeof(int[2]));
    pid_t *pids = malloc(k * sizeof(pid_t));
    if (!pipes || !pids) { perror("malloc"); return 1; }

    for (size_t i = 0; i < k; i++) {
        if (pipe(pipes[i]) == -1) { perror("pipe"); return 1; }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }

        if (pid == 0) {
            // hijo i
            close(pipes[i][0]);
            size_t start = (i * n) / k;
            size_t end   = ((i + 1) * n) / k; // exclusivo
            long long s = 0;
            for (size_t j = start; j < end; j++) s += a[j];
            if (write(pipes[i][1], &s, sizeof(s)) != sizeof(s)) perror("write");
            close(pipes[i][1]);
            _exit(0);
        } else {
            // padre
            pids[i] = pid;
            close(pipes[i][1]);
        }
    }

    // 4) Padre acumula resultados
    long long total = 0, part = 0;
    for (size_t i = 0; i < k; i++) {
        if (read(pipes[i][0], &part, sizeof(part)) == sizeof(part)) total += part;
        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0);
    }

    printf("%lld\n", total);

    free(pids); free(pipes); free(a);
    return 0;
}
