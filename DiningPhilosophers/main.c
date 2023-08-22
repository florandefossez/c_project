#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


#define NUM_PHILOSOPHERS 3

time_t start_time;

typedef enum philosopher_ {
    THINKING,
    HUNGRY,
    EATING
} philosopher_t;

// define philosophers by there state and there place in the array
philosopher_t philosophers[NUM_PHILOSOPHERS];

// forks are repesented with semaphores, when one is blocked the fork is not available
sem_t forks[NUM_PHILOSOPHERS];

// functions
void print_timestamp();
void* philosopher(void* arg);
void take_forks(int philosopher_id);
void put_forks(int philosopher_id);

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    
    // initialize semaphores
    for (int i=0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&forks[i], 0, 1);
    }

    time(&start_time);

    // philosopher threads
    for (int i=0; i < NUM_PHILOSOPHERS; i++) {
        int* philosopher_id = (int*) malloc(sizeof(int));
        *philosopher_id = i;
        pthread_create(&philosophers[i], NULL, philosopher, (void*) philosopher_id);
    }

    for (int i=0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // clean up
    for (int i=0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&forks[i]);
    }

    return 0;
}

void* philosopher(void* arg) {
    int philosopher_id = *(int*) arg;

    while (1) {
        print_timestamp();
        printf("Philosopher %d is thinking.\n", philosopher_id);
        sleep(1);

        print_timestamp();
        printf("Philosopher %d is hungry.\n", philosopher_id);
        take_forks(philosopher_id);
        
        print_timestamp();
        printf("Philosopher %d is eating.\n", philosopher_id);
        sleep(1);

        put_forks(philosopher_id);
    }
}

void take_forks(int philosopher_id) {
    philosophers[philosopher_id] = HUNGRY;
    // lock the semaphores
    sem_wait(&forks[philosopher_id]);
    sem_wait(&forks[(philosopher_id + 1) % NUM_PHILOSOPHERS]);
    philosophers[philosopher_id] = EATING;
}

void put_forks(int philosopher_id) {
    philosophers[philosopher_id] = THINKING;
    // release the semaphores
    sem_post(&forks[philosopher_id]);
    sem_post(&forks[(philosopher_id + 1) % NUM_PHILOSOPHERS]);
}

void print_timestamp() {
    time_t current_time;
    time(&current_time);
    printf("[%ld] ", (current_time - start_time));
}
