#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER1_SIZE 10
#define BUFFER2_SIZE 10
#define MAX_BINARY_LENGTH 32
#define INPUT_BUFFER_SIZE 256

char buffer1[BUFFER1_SIZE][MAX_BINARY_LENGTH];
int buffer1_in = 0;
int buffer1_out = 0;

int buffer2[BUFFER2_SIZE];
int buffer2_in = 0;
int buffer2_out = 0;

sem_t sem_buffer1_filled;
sem_t sem_buffer1_empty;
sem_t sem_buffer2_filled;
sem_t sem_buffer2_empty;

pthread_mutex_t mutex_buffer1;
pthread_mutex_t mutex_buffer2;

volatile int exit_flag = 0;

int binary_to_int(const char* bin_str) {
    int num = 0;
    while (*bin_str) {
        num = (num << 1) + (*bin_str - '0');
        bin_str++;
    }
    return num;
}

void int_to_binary(int num, char* bin_str) {
    int start = 0;
    int index = 0;
    for (int i = 31; i >= 0; i--) {
        if ((num >> i) & 1) {
            start = 1;
        }
        if (start) {
            bin_str[index++] = ((num >> i) & 1) ? '1' : '0';
        }
    }
    if (!start) {
        bin_str[index++] = '0';
    }
    bin_str[index] = '\0';
}

int fib(int n){
    if (n == 0)
        return 0;
    else if (n == 1)
        return 1;
    int fib_i_1 = 1;
    int fib_i_2 = 0;
    int fib_i = 1;
    for (int i = 1; i < n; ++i){
        int aux = fib_i_1 + fib_i_2;
        fib_i_2 = fib_i_1;
        fib_i_1 = fib_i;
        fib_i = aux;
    }
    return fib_i;
}

void* producer(void* arg) {
    char input[INPUT_BUFFER_SIZE];
    write(STDOUT_FILENO, "Enter numbers (or type 'exit' to quit): ", 40);
    while (!exit_flag) {
        ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytes_read <= 0) {
            continue;
        }
        input[bytes_read] = '\0';
        char* newline = strchr(input, '\n');
        if (newline) {
            *newline = '\0';
        }
        if (strncmp(input, "exit", 4) == 0) {
            exit_flag = 1;
            for (int i = 0; i < BUFFER1_SIZE; i++) {
                sem_post(&sem_buffer1_filled);
            }
            for (int i = 0; i < BUFFER2_SIZE; i++) {
                sem_post(&sem_buffer2_filled);
            }
            return NULL;
        }
        char* token = strtok(input, " ");
        while (token != NULL) {
            if (!isdigit(token[0])) {
                token = strtok(NULL, " ");
                continue;
            }
            int num = atoi(token);
            char binary_str[MAX_BINARY_LENGTH];
            int_to_binary(num, binary_str);
            sem_wait(&sem_buffer1_empty);
            pthread_mutex_lock(&mutex_buffer1);
            strcpy(buffer1[buffer1_in], binary_str);
            buffer1_in = (buffer1_in + 1) % BUFFER1_SIZE;
            pthread_mutex_unlock(&mutex_buffer1);
            sem_post(&sem_buffer1_filled);
            token = strtok(NULL, " ");
        }
    }
    return NULL;
}

void* consumer_producer(void* arg) {
    while (!exit_flag) {
        sem_wait(&sem_buffer1_filled);
        if (exit_flag) break;
        pthread_mutex_lock(&mutex_buffer1);
        char temp[MAX_BINARY_LENGTH];
        strcpy(temp, buffer1[buffer1_out]);
        buffer1_out = (buffer1_out + 1) % BUFFER1_SIZE;
        pthread_mutex_unlock(&mutex_buffer1);
        sem_post(&sem_buffer1_empty);
        int num = binary_to_int(temp);
        int fib_value = fib(num);
        sem_wait(&sem_buffer2_empty);
        pthread_mutex_lock(&mutex_buffer2);
        buffer2[buffer2_in] = fib_value;
        buffer2_in = (buffer2_in + 1) % BUFFER2_SIZE;
        pthread_mutex_unlock(&mutex_buffer2);
        sem_post(&sem_buffer2_filled);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (!exit_flag) {
        sem_wait(&sem_buffer2_filled);
        if (exit_flag) break;
        pthread_mutex_lock(&mutex_buffer2);
        int result = buffer2[buffer2_out];
        buffer2_out = (buffer2_out + 1) % BUFFER2_SIZE;
        pthread_mutex_unlock(&mutex_buffer2);
        sem_post(&sem_buffer2_empty);
        printf("Fibonacci: %d\n", result);
    }
    return NULL;
}

int main(){
    sem_init(&sem_buffer1_filled, 0, 0);
    sem_init(&sem_buffer1_empty, 0, BUFFER1_SIZE);
    sem_init(&sem_buffer2_filled, 0, 0);
    sem_init(&sem_buffer2_empty, 0, BUFFER2_SIZE);
    pthread_mutex_init(&mutex_buffer1, NULL);
    pthread_mutex_init(&mutex_buffer2, NULL);
    pthread_t tid_producer, tid_consumer_producer, tid_consumer;
    pthread_create(&tid_producer, NULL, producer, NULL);
    pthread_create(&tid_consumer_producer, NULL, consumer_producer, NULL);
    pthread_create(&tid_consumer, NULL, consumer, NULL);
    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer_producer, NULL);
    pthread_join(tid_consumer, NULL);
    sem_destroy(&sem_buffer1_filled);
    sem_destroy(&sem_buffer1_empty);
    sem_destroy(&sem_buffer2_filled);
    sem_destroy(&sem_buffer2_empty);
    pthread_mutex_destroy(&mutex_buffer1);
    pthread_mutex_destroy(&mutex_buffer2);
    return 0;
}
