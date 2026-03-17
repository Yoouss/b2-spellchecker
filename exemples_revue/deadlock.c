#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    int err = pthread_mutex_lock(&mutex_a);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }

    err = pthread_mutex_lock(&mutex_b);  // attend que thread2 libère mutex_b
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }

    // section critique
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2(void *arg) {
    int err = pthread_mutex_lock(&mutex_b);
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }

    err = pthread_mutex_lock(&mutex_a);  // attend que thread1 libère mutex_a
    if (err != 0) { fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err)); return NULL; }

    // section critique
    pthread_mutex_unlock(&mutex_a);
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int err;

    err = pthread_create(&t1, NULL, thread1, NULL);
    if (err != 0) { fprintf(stderr, "pthread_create: %s\n", strerror(err)); return EXIT_FAILURE; }
    err = pthread_create(&t2, NULL, thread2, NULL);
    if (err != 0) { fprintf(stderr, "pthread_create: %s\n", strerror(err)); return EXIT_FAILURE; }

    err = pthread_join(t1, NULL);
    if (err != 0) { fprintf(stderr, "pthread_join: %s\n", strerror(err)); return EXIT_FAILURE; }
    err = pthread_join(t2, NULL);
    if (err != 0) { fprintf(stderr, "pthread_join: %s\n", strerror(err)); return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
