#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int sum = 0;                      // Sum (accumulator)
int n_threads;                         // Number of threads
int active_n_threads;                  // Number of active threads
int blocked_n_threads = 0;             // Number of blocked threads
pthread_mutex_t mutex;                 // Mutex for safely accessing sum
pthread_mutex_t mutex_active_threads;  // Mutex for safely accessing active_threads
pthread_cond_t cond_print;             // Cond for the extra thread
pthread_cond_t cond_exec;              // Cond for the main threads

// Worker thread
void *ExecutaTarefa (void *arg) {

  for (int i=0; i<100001; i++) {
    pthread_mutex_lock(&mutex); // Locks mutex so that sum cannot be overwritten

    // If sum has reached its goal, than the thread can wait for the other threads to get here
    if (sum % 1000 == 0) {
      blocked_n_threads++;

      // If all threads are blocked, so they reached 1000*k and the extra thread can execute
      if (blocked_n_threads == active_n_threads) {
        pthread_cond_signal(&cond_print);
      }

      // Makes the active thread wait
      pthread_cond_wait(&cond_exec, &mutex);
    }

    sum++; // After possibly printing sum, sum can be incremented
    pthread_mutex_unlock(&mutex); // Unlocks mutex after sum is modified
  }

  // Decrements the number of active threads so that the finalizes thread
  // will no longer be considered for the counting of threads that need to
  // be waited for
  pthread_mutex_lock(&mutex_active_threads);
  active_n_threads -= 1;
  pthread_mutex_unlock(&mutex_active_threads);

  pthread_exit(NULL);  // Finalizes the thread execution
  return NULL;         // Makes Windows stop complaining about a void function with no return
}

// Printer thread
void *extra (void *args) {
  long int local_sum = 1; // Local control variable
  const long int MAX_VAL = n_threads*100000; // Maximum value

  // While sum hasn't reached its maximum value
  while (local_sum < MAX_VAL){          // Locks mutex so that sum cannot be overwritten
    pthread_mutex_lock(&mutex);         // Assumes sum is a multiple of 1000
    printf("sum = %ld \n", sum);        // Assumes sum is a multiple of 1000
    local_sum = sum;                    // Updates local_sum
    blocked_n_threads = 0;              // Update blocked threads counter
    pthread_cond_broadcast(&cond_exec); // Unblocks all threads

    if(sum < MAX_VAL){
      pthread_cond_wait(&cond_print, &mutex); // Unlocks mutex after sum is modified
    }
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);  // Finalizes the thread execution
  return NULL;         // Makes Windows stop complaining about a void function with no return
}

// Main function
int main(int argc, char *argv[]) {
  // Defends in case the user forgot an argument
  if (argc < 2) {
    printf("Error: Expected %s <n_threads>\n", argv[0]);
    return 1;
  }
  n_threads = atoi(argv[1]);  // Initializes n_threads
  active_n_threads = n_threads;  // Initializes active_n_threads as n_threads
  pthread_t *tids = malloc(sizeof(pthread_t) * (n_threads + 1)); // Mallocs space for t-ids

  // Initializes all mutexes and conds
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex_active_threads, NULL);
  pthread_cond_init(&cond_exec, NULL);
  pthread_cond_init(&cond_print, NULL);

  // Creates all threads
  for(long int t = 0; t < n_threads; t++) {
    pthread_create(&tids[t], NULL, ExecutaTarefa, NULL);
  }
  pthread_create(&tids[n_threads], NULL, extra, NULL);

  // Waits for all threads to finish executing
  for(int t = 0; t < n_threads + 1; t++) {
    pthread_join(tids[t], NULL);
  }

  // Finalizes all mutexes and conds
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&mutex_active_threads);
  pthread_cond_destroy(&cond_exec);
  pthread_cond_destroy(&cond_print);

  // Frees the space malloced to t-ids
  free(tids);

  return 0;
}