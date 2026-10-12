#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


#define CELLS 10
#define NT 5

pthread_mutex_t lock;

typedef struct{
    int i;
    int j;
} Indexes;

int A[CELLS];

void *move(void * arg){
    Indexes index = *(Indexes*)arg;
    int cell = index.i;
    int particles = index.j;
    free(arg);
    pthread_mutex_lock(&lock);
    if(particles > 0 && cell < 9){
        int n = rand()%particles;
        A[cell] -= n;
        A[cell + 1] += n;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(){
    pthread_t tid[NT];
    pthread_mutex_init(&lock, NULL);

    for(int i = 0; i<CELLS; i++){
        A[i]= rand()%3;
        printf("pA[%d] = %d\n",i, A[i]);
    }
    printf("\n");
    
    for(int i = 0; i<NT; i++){
        Indexes *index = malloc(sizeof(Indexes));
        index->i = i;
        index->j = A[i];
        pthread_create(&tid[i], NULL, move, index);
    }

    for(int i = 0; i<NT; i++){
        pthread_join(tid[i], NULL);
    }

    for(int i = 0; i<CELLS; i++){
        printf("nA[%d] = %d\n",i, A[i]);
    }
    return 0;
}