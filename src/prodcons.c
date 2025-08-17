// prodcons.c (versión portable sin uintptr_t/pthread_self casts)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int *buf;               
    int n;                  
    int producers_alive;    
    int tmax;               
    int scale_ms;           
    pthread_mutex_t mtx;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} pc_t;

static inline void sleep_units(int units, int scale_ms){
    if (units <= 0) return;
    struct timespec ts;
    ts.tv_sec  = (units * scale_ms) / 1000;
    ts.tv_nsec = ((units * scale_ms) % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static bool is_full(pc_t *pc){
    for (int i=0;i<pc->n;i++) if (pc->buf[i]==0) return false;
    return true;
}

static bool is_empty(pc_t *pc){
    for (int i=0;i<pc->n;i++) if (pc->buf[i]!=0) return false;
    return true;
}

static void* producer(void *arg){
    pc_t *pc = (pc_t*)arg;

    unsigned seed = (unsigned)time(NULL) ^ (unsigned)clock();

    for (;;){
        pthread_mutex_lock(&pc->mtx);

        if (is_full(pc)) {
            pc->producers_alive--;
            pthread_cond_broadcast(&pc->not_empty); 
            pthread_mutex_unlock(&pc->mtx);
            return NULL;
        }

        for (int i=0;i<pc->n;i++){
            if (pc->buf[i]==0){
                int k = (rand_r(&seed) % 9) + 1; // 1..9
                pc->buf[i] = k;
                pthread_cond_signal(&pc->not_empty);
                break;
            }
        }

        pthread_mutex_unlock(&pc->mtx);

        int t = pc->tmax>0 ? ((rand_r(&seed)%pc->tmax)+1) : 1;
        sleep_units(t, pc->scale_ms);
    }
    return NULL;
}

static void* consumer(void *arg){
    pc_t *pc = (pc_t*)arg;

    for (;;){
        pthread_mutex_lock(&pc->mtx);

        while (is_empty(pc) && pc->producers_alive > 0){
            pthread_cond_wait(&pc->not_empty, &pc->mtx);
        }

        if (is_empty(pc) && pc->producers_alive == 0) {
            pthread_mutex_unlock(&pc->mtx);
            return NULL;
        }

        int k = 0;
        for (int i=0;i<pc->n;i++){
            if (pc->buf[i]!=0){
                k = pc->buf[i];
                pc->buf[i] = 0;
                break;
            }
        }

        pthread_cond_signal(&pc->not_full);
        pthread_mutex_unlock(&pc->mtx);

        sleep_units(k, pc->scale_ms);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc < 6){
        fprintf(stderr, "Uso: %s <n> <P> <C> <tmax> <scale_ms>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 20 2 3 5 100\n", argv[0]);
        return 1;
    }
    int n        = atoi(argv[1]);
    int P        = atoi(argv[2]);
    int C        = atoi(argv[3]);
    int tmax     = atoi(argv[4]);
    int scale_ms = atoi(argv[5]);

    if (n<=0 || P<=0 || C<=0 || tmax<0 || scale_ms<=0){
        fprintf(stderr, "Parámetros inválidos.\n");
        return 1;
    }

    pc_t pc;
    pc.n = n;
    pc.buf = calloc(n, sizeof(int)); 
    pc.producers_alive = P;
    pc.tmax = tmax;
    pc.scale_ms = scale_ms;
    pthread_mutex_init(&pc.mtx, NULL);
    pthread_cond_init(&pc.not_empty, NULL);
    pthread_cond_init(&pc.not_full, NULL);

    srand((unsigned)time(NULL));

    pthread_t *prod = malloc(P*sizeof(pthread_t));
    pthread_t *cons = malloc(C*sizeof(pthread_t));

    for (int i=0;i<P;i++) pthread_create(&prod[i], NULL, producer, &pc);
    for (int j=0;j<C;j++) pthread_create(&cons[j], NULL, consumer, &pc);

    for (int i=0;i<P;i++) pthread_join(prod[i], NULL);

    pthread_mutex_lock(&pc.mtx);
    pthread_cond_broadcast(&pc.not_empty);
    pthread_mutex_unlock(&pc.mtx);

    for (int j=0;j<C;j++) pthread_join(cons[j], NULL);

    printf("Estado final del buffer:\n[");
    for (int i=0;i<n;i++){
        printf("%d", pc.buf[i]);
        if (i<n-1) printf(" ");
    }
    printf("]\n");

    free(prod); free(cons);
    pthread_cond_destroy(&pc.not_empty);
    pthread_cond_destroy(&pc.not_full);
    pthread_mutex_destroy(&pc.mtx);
    free(pc.buf);
    return 0;
}
