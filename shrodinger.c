#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int buffer[BUFFER_SIZE] = {0};  // 0 = empty, 1 = filled
int in = 0;
int out = 0;

sem_t empty_slots; // counts the number of available pairs of empty slots
sem_t full_slots;  // counts the number of available pairs of full slots
pthread_mutex_t mutex;

void* producer(void* arg) {
    while (1) {
        sem_wait(&empty_slots);  // wait until there is a pair of empty slots
        pthread_mutex_lock(&mutex);

        // assume in and in+1 are both empty
        buffer[in] = 1;
        buffer[(in + 1) % BUFFER_SIZE] = 1;
        printf(" Producer created a pair at [%d, %d]\n", in, (in + 1) % BUFFER_SIZE);
        in = (in + 2) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full_slots);  // signal that a new pair is available
        sleep(rand() % 2);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&full_slots);  // wait until there is a pair of filled slots
        pthread_mutex_lock(&mutex);

        // assume out and out+1 are both full
        buffer[out] = 0;
        buffer[(out + 1) % BUFFER_SIZE] = 0;
        printf(" Consumer took a pair from [%d, %d]\n", out, (out + 1) % BUFFER_SIZE);
        out = (out + 2) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty_slots);  // signal that a new pair of empty slots is available
        sleep(rand() % 3);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    sem_init(&empty_slots, 0, BUFFER_SIZE / 2);  // each pair occupies two slots
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
