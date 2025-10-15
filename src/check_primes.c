// Headers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

// Global variables
long long N;                       // Number of generated numbers
int M;                             // Size of buffer
int C;                             // Number of consumer threads
int *buffer;                       // Buffer
int pos_read = 0;                  // Read position in buffer
int pos_write = 0;                 // Write position in buffer
sem_t empty_spaces;                // Empty spaces in buffer
sem_t available_items;             // Available items in buffer
sem_t mutex;                       // Mutex
long long *primes_per_consumer;    // Array for counting primes per thread
long long total_found_primes = 0;  // Total number of primes

// Checks if a number is prime
int is_prime(long long int n) {
  if (n <= 1) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;
  for (long long i = 3; i * i <= n; i += 2) {
    if (n % i == 0) return 0;
  }
  return 1;
}

// Producer thread
void *producer_func(void *arg) {
  for (long long i = 0; i < N; i++) {
    sem_wait(&empty_spaces);
    sem_wait(&mutex);
    
    buffer[pos_write] = i;
    pos_write = (pos_write + 1) % M;
     
    sem_post(&mutex);
    sem_post(&available_items);
  }

  // Sends one termination flag (-1) for each consumer
  for (int i = 0; i < C; i++) {
    sem_wait(&empty_spaces);
    sem_wait(&mutex);

    buffer[pos_write] = -1;
    pos_write = (pos_write + 1) % M;

    sem_post(&mutex);
    sem_post(&available_items);
  }

  pthread_exit(NULL);
}

// Consumer thread
void *consumer_func(void *arg) {
  long id = (long)arg;
  long long local_primes = 0;

  while (1) {
    sem_wait(&available_items);
    sem_wait(&mutex);

    int value = buffer[pos_read];
    pos_read = (pos_read + 1) % M;

    sem_post(&mutex);
    sem_post(&empty_spaces);

    if (value == -1) break;
    if (is_prime(value)) local_primes++;
  }
    
  primes_per_consumer[id] = local_primes;
  printf("Consumer %ld found %lld primes.\n", id, local_primes);
  pthread_exit(NULL);
}

// Main
int main(int argc, char *argv[]) {

  // Defends the code in case the user doesn't know its parameters
  if(argc != 4){
    printf("Error: correct use is %s <N> <M> <C>\n", argv[0]);
    printf("where:\n");
    printf("    N: Number of numbers to be generated.\n");
    printf("    M: Size of the buffer.\n");
    printf("    C: Number of consumer threads.\n");
    return 1;
  }

  N = atoll(argv[1]); // Number of numbers
  M = atoi(argv[2]);  // Size of the buffer
  C = atoi(argv[3]);  // Number of consumer threasd

  if (N <= 0 || M <= 0 || C <= 0) {
    printf("N, M and C must be positive integers.\n");
    return 1;
  }

  buffer = malloc(M * sizeof(int));   
  primes_per_consumer = calloc(C, sizeof(long long));
  pthread_t producer_thread;
  pthread_t consumers[C];

  sem_init(&empty_spaces, 0, M);
  sem_init(&available_items, 0, 0);
  sem_init(&mutex, 0, 1);

    pthread_create(&producer_thread, NULL, producer_func, NULL);
  for (long i = 0; i < C; i++) {
    pthread_create(&consumers[i], NULL, consumer_func, (void *)i);   
  }

  pthread_join(producer_thread, NULL);
  for(int i = 0; i < C; i++){
    pthread_join(consumers[i], NULL);
  }

  long long max_primes = -1;
  long long winner_id = -1;

  for(int i = 0 ; i < C ; i++){
    total_found_primes += primes_per_consumer[i];
    if(primes_per_consumer[i] > max_primes){
      max_primes = primes_per_consumer[i];
      winner_id = i;
    }
  }

  printf("\n--- Final Result ---\n");
  printf("Total primes found: %lld\n", total_found_primes);
  printf("Winner thread ID: %lld (found %lld primes)\n", winner_id, max_primes);

  sem_destroy(&empty_spaces);
  sem_destroy(&available_items);
  sem_destroy(&mutex);
  free(buffer);
  free(primes_per_consumer);

  return 0;
}