#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) { fprintf(stderr, "Uso: %s <n>\n", argv[0]); return 1; }
    long n = strtol(argv[1], NULL, 10);
    if (n <= 0) { fprintf(stderr, "n debe ser > 0\n"); return 1; }

    srand((unsigned)time(NULL));
    for (long i = 0; i < n; i++) {
        long long v = rand() % 1000; // 0..999
        printf("%lld", v);
        if (i < n - 1) putchar(' ');
    }
    putchar('\n');
    return 0;
}
