#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include "semaphore.h"

Semaphore sem;

#define NUMT 20
#define NUM_RESOURCES 5

int nThreadsAccessing = 0;

int getFromDatabase() {
    nThreadsAccessing++;
    printf("Number of Threads accessing the DB = %d\n", nThreadsAccessing);
    if (nThreadsAccessing > NUM_RESOURCES) _exit(1);
    usleep(rand()%10000);
    nThreadsAccessing--;
    return rand();
}


void * thrfunc(void* arg){
    int id  = *(int*) arg;
    free(arg);
    printf("Im thread %d", id);

    sem_wait(&sem);
    getFromDatabase();
    sem_post(&sem);
    return NULL;
}

int main(){
    pthread_t tid[NUMT];

    sem_init(&sem, NUM_RESOURCES);

    for(int i = 0; i < NUMT; i++){
        int * id = malloc(sizeof(int));
        *id = i;
        pthread_create(&tid[i], NULL, thrfunc, id);
    }
    for(int i = 0; i < NUMT; i++){
        pthread_join(tid[i], NULL);
    }
    sem_destroy(&sem);
    return 0;
}