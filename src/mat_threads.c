
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define N 40

static int A[N][N], B[N][N];
static int SUM[N][N], DIFF[N][N], PROD[N][N];

typedef struct {
    int (*X)[N];
    int (*Y)[N];
    int (*OUT)[N];
} binop_args_t;

static inline long long now_ns(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec*1000000000LL + ts.tv_nsec;
}

void* add_thread(void* arg){
    binop_args_t* a = (binop_args_t*)arg;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            a->OUT[i][j] = a->X[i][j] + a->Y[i][j];
    return NULL;
}

void* sub_thread(void* arg){
    binop_args_t* a = (binop_args_t*)arg;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            a->OUT[i][j] = a->X[i][j] - a->Y[i][j];
    return NULL;
}

static void matmul(int C[N][N], int X[N][N], int Y[N][N]){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            int acc = 0;
            for(int k=0;k<N;k++) acc += X[i][k]*Y[k][j];
            C[i][j] = acc;
        }
    }
}

static long long checksum(int M[N][N]){
    long long s=0;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++) s += M[i][j];
    return s;
}

int main(void){
    srand((unsigned)time(NULL));
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++){
            A[i][j] = rand()%2;
            B[i][j] = rand()%2;
        }

    long long t0 = now_ns();

    pthread_t t_add, t_sub;
    binop_args_t args_add = { .X=A, .Y=B, .OUT=SUM };
    binop_args_t args_sub = { .X=A, .Y=B, .OUT=DIFF };

    if(pthread_create(&t_add, NULL, add_thread, &args_add)!=0){perror("pthread_create add"); return 1;}
    if(pthread_create(&t_sub, NULL, sub_thread, &args_sub)!=0){perror("pthread_create sub"); return 1;}

    pthread_join(t_add, NULL);
    pthread_join(t_sub, NULL);

    matmul(PROD, SUM, DIFF);

    long long t1 = now_ns();


    printf("checksum(SUM)=%lld  checksum(DIFF)=%lld  checksum(PROD)=%lld\n",
           checksum(SUM), checksum(DIFF), checksum(PROD));
    double ms = (t1 - t0)/1e6;
    printf("Tiempo (pthreads): %.3f ms\n", ms);
    return 0;
}
