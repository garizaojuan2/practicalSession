#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 40

static int A[N][N], B[N][N];
static int SUM[N][N], DIFF[N][N], PROD[N][N];

static inline long long now_ns(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec*1000000000LL + ts.tv_nsec;
}

static void add(int C[N][N], int X[N][N], int Y[N][N]){
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            C[i][j] = X[i][j] + Y[i][j];
}

static void subm(int C[N][N], int X[N][N], int Y[N][N]){
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            C[i][j] = X[i][j] - Y[i][j];
}

static void matmul(int C[N][N], int X[N][N], int Y[N][N]){
    for (int i=0;i<N;i++){
        for (int j=0;j<N;j++){
            int acc = 0;
            for (int k=0;k<N;k++) acc += X[i][k]*Y[k][j];
            C[i][j] = acc;
        }
    }
}

static long long checksum(int M[N][N]){
    long long s=0;
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++) s += M[i][j];
    return s;
}

int main(void){
    srand((unsigned)time(NULL));
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++){
            A[i][j] = rand()%2;
            B[i][j] = rand()%2;
        }

    long long t0 = now_ns();


    add(SUM, A, B);
    subm(DIFF, A, B);

    matmul(PROD, SUM, DIFF);

    long long t1 = now_ns();

    printf("checksum(SUM)=%lld  checksum(DIFF)=%lld  checksum(PROD)=%lld\n",
           checksum(SUM), checksum(DIFF), checksum(PROD));
    double ms = (t1 - t0)/1e6;
    printf("Tiempo (secuencial): %.3f ms\n", ms);
    return 0;
}
