#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int buffer[BUFFER_SIZE] = {0};  // 0 = empty, 1 = filled
int in = 0;
int out = 0;

sem_t empty_slots; // شمارنده تعداد جفت‌مکان‌های خالی (حداقل دو لازم داریم)
sem_t full_slots;  // شمارنده تعداد جفت‌مکان‌های پر
pthread_mutex_t mutex;

void* producer(void* arg) {
    while (1) {
        sem_wait(&empty_slots);  // منتظر فضای خالی جفتی
        pthread_mutex_lock(&mutex);

        // فرض: in و in+1 فضای خالی هستند
        buffer[in] = 1;
        buffer[(in + 1) % BUFFER_SIZE] = 1;
        printf("✅ تولیدکننده یک جفت در [%d,%d] تولید کرد\n", in, (in + 1) % BUFFER_SIZE);
        in = (in + 2) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full_slots);  // افزایش جفت پر
        sleep(rand() % 2);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&full_slots);  // منتظر جفت پر
        pthread_mutex_lock(&mutex);

        // فرض: out و out+1 پر هستند
        buffer[out] = 0;
        buffer[(out + 1) % BUFFER_SIZE] = 0;
        printf("🛒 مصرف‌کننده یک جفت از [%d,%d] برداشت\n", out, (out + 1) % BUFFER_SIZE);
        out = (out + 2) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty_slots);  // افزایش جفت خالی
        sleep(rand() % 3);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    sem_init(&empty_slots, 0, BUFFER_SIZE / 2);  // هر جفت دو مکان می‌گیره
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return 0;
}
