// Headers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

// Global variables
long long N;                       // Number of generated numbers
int M;                             // Size of buffer
int C;                             // Number of ``consumer'' threads
int *buffer;                       // Buffer
int pos_read = 0;                  // Control variable
long long consumed = 0;            // Consumed itens counter
sem_t empty_spaces;                // Empty spaces in buffer counter
sem_t available_items;             // Available itens in buffer counter
sem_t mutex;                       // Mutex
long long *primes_per_consumer;    // Array for counting how many primes per thread
long long total_found_primes = 0;  // Number of primes

// Verifies if a number is prime
int is_prime(long long int n){
  if (n <= 1) return 0;
  if (n == 2) return 1;
  if (n % 2 == 0) return 0;
  for (int i=3 ; i <= sqrt(n) ; i+=2){
    if (n % i == 0) return 0;
  }
  return 1;
}

// Producer thread function
void *producer_func(void *arg) {
  long long generated_numbers = 0;

  while(generated_numbers < N){
    // Waits until the buffer is empty
    for(int i=0 ; i < M ; i++){
      sem_wait(&empty_spaces);
    }

    // Fills the buffer with new numbers
    for(int i=0 ; i < M ; i++){
      if(generated_numbers < N){
        buffer[i] = generated_numbers;
        generated_numbers++;
      } 
      else{
        // Fills the rest of the buffer with a ``flag'' value in case N % M != 0
        buffer[i] = -1; 
      }
    }
    
    // Free the itens for the consumers
    for(int i=0 ; i < M ; i++){
      sem_post(&available_items);
    }
  }

  pthread_exit(NULL);
}

// Consumer thread function
void *consumer_func(void *arg){
  long id = (long)arg;
  long long t_total_primes = 0;

  while(1){
    // Waits for an available iten in the buffer
    sem_wait(&available_items);

    sem_wait(&mutex);
    
    // Ending condition
    if(consumed >= N){
      sem_post(&mutex);
      sem_post(&available_items); 
      break;
    }

    // Reads the buffer and updates counters
    long long read_number = buffer[pos_read];
    pos_read = (pos_read + 1) % M;
    consumed++;

    sem_post(&mutex);

    // Frees an empty space in the buffer
    sem_post(&empty_spaces);

    // Process the item
    if (read_number != -1 && is_prime(read_number)) {
      t_total_primes++;
    }
  }

  primes_per_consumer[id] = t_total_primes;
  printf("Consumer %ld found %lld primes.\n", id, t_total_primes);
  pthread_exit(NULL);
}

// Main function
int main(int argc, char *argv[]){

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

  // Defends the code in case the user is trying to be funny
  if (N <= 0 || M <= 0 || C <= 0) {
    printf("N, M and C must all the positive integers.\n");
    return 1;
  }

  // Allocs memory
  buffer = (int *)malloc(M * sizeof(int));
  primes_per_consumer = (long long *)calloc(C, sizeof(long long));
  pthread_t consumer_threads[C];
  pthread_t producer_thread;

  // Initializes semaphores
  sem_init(&empty_spaces, 0, M);     // Starts with M available spaces
  sem_init(&available_items, 0, 0);  // Starts with no available itens
  sem_init(&mutex, 0, 1);            // Starts with 1 available signal

  // Creates consumer threads
  for (long i=0 ; i < C ; i++){
    pthread_create(&consumer_threads[i], NULL, consumer_func, (void *)i);
  }

  // Creates the producer thread
  pthread_create(&producer_thread, NULL, producer_func, NULL);

  // Waits for all threads to finish executing
  pthread_join(producer_thread, NULL);
  for (int i=0 ; i < C ; i++){
    pthread_join(consumer_threads[i], NULL);
  }
   
  // Initializes the result variables as -1 since it will always be smaller than 0
  long long winner_id = -1;
  long long max_primes = -1;

  for(int i=0 ; i < C ; i++){
    total_found_primes += primes_per_consumer[i];
    if (primes_per_consumer[i] > max_primes){
      max_primes = primes_per_consumer[i];
      winner_id = i;
    }
  }

  // Prints final results
  printf("\n--- Final Result ---\n");
  printf("Total number of primes found: %lld\n", total_found_primes);
  printf("            Winner thread ID: %lld (found %lld primes)\n", winner_id, max_primes);
  
  // Destroys semaphores and frees allocated memory
  sem_destroy(&empty_spaces);
  sem_destroy(&available_items);
  sem_destroy(&mutex);
  free(buffer);
  free(primes_per_consumer);

  return 0;
}