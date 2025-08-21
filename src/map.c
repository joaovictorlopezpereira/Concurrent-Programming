#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Argument being passed to thread_map
typedef struct{
    int *seg_base;
    int seg_len;
    int (*func)(int);
} t_args;

// Function executed by each thread created by concurrent_map
void *thread_map(void *arguments){
    t_args args = *(t_args *) arguments; // Type-casting void* to t_args

    // Applying f to each element in the segment
    for (int i=0 ; i<args.seg_len ; i++){
        args.seg_base[i] = args.func(args.seg_base[i]);
    }

    pthread_exit(NULL);
}

// Applies a function to each element of an array (concurrently)
void concurrent_map(int vector[], int len, int (*func)(int), int n_threads){
    t_args *vector_of_args = malloc(n_threads * sizeof(t_args)); // Thread elements
    pthread_t *tids = malloc(n_threads * sizeof(pthread_t)); // Thread identifiers

    for (int i=0 ; i<n_threads ; i++){
        // Initializes the threads arguments
        vector_of_args[i].seg_base = vector + (i * len / n_threads);
        vector_of_args[i].seg_len = len / n_threads;
        if (i == n_threads - 1){ // Selects the last remaining elements of vector to the last thread
            vector_of_args[i].seg_len += len % n_threads;
        }
        vector_of_args[i].func = func;

        // Creates and launches each thread
        if(pthread_create(&tids[i], NULL, thread_map, &vector_of_args[i])){
            printf("Error: pthread_create didn't return 0.\n\n");
            free(tids);
            free(vector_of_args);
            exit(EXIT_FAILURE);
        }
    }

    // Waits for all threads to return
    for (int i=0 ; i<n_threads; i++){
        if(pthread_join(tids[i], NULL)){
            printf("Error: pthread_join didn't return 0.\n\n");
            free(tids);
            free(vector_of_args);
            exit(EXIT_FAILURE);
        }
    }

    // Frees the allocated space
    free(vector_of_args);
    free(tids);
}

// Initializes a given vector as an enumeration
void init_vector(int vector[], int len){
    for (int i=0 ; i<len ; i++){
        vector[i] = i+1;
    }
}

// Prints a vector
void print_vector(int vector[], int len){
    printf("[");
    for (int i=0 ; i<len ; i++){
        printf(" %d ", vector[i]);
    }
    printf("]");
}

// Argument function applied by thread_map
int func(int x){
    // return (x <= 2) ? x : func(x-1) + func(x-2);
    return x + 1;
}

// Main function
int main(int argc, char* argv[]){
    short int n_threads; // Number of threads
    int size_of_vector; // Size of vector
    int *vector; // Vector
    int print; // Bool

    // Verifies if the program arguments are being passed
    if (argc < 4){
        printf("Error: there's an argument [n_threads] or [size_of_vector] or [print?] missing.\n\n");
        exit(EXIT_FAILURE);
    }

    // Captures program's input
    n_threads = atoi(argv[1]); // Initializes n_threads
    size_of_vector = atoi(argv[2]); // Initializes size_of_vector
    print = atoi(argv[3]);

    // Defends the code in case size_of_vector < 0
    if (size_of_vector < 0){
        printf("Error: the size of vector needs to be non-negative.\n\n");
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads <= 0
    if (n_threads <= 0){
        printf("Error: the number of threads needs to be positive.\n\n");
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads > size_of_vector
    if (size_of_vector < n_threads){
        printf("\nNumber of threads limited to %d.", size_of_vector);
        n_threads = size_of_vector;
    }

    // Defends the code in case print != 0 and print != 1
    if (print != 0 && print != 1){
        printf("\nprint? set to 1.");
        print = 1;
    }

    // allocates and initializes vector
    vector = malloc(size_of_vector * sizeof(int)); // Allocates space for vector
    init_vector(vector, size_of_vector); // Initializes vector

    // Prints original vector
    if (print){
        printf("\n\nOriginal vector: ");
        print_vector(vector, size_of_vector);
    }

    // Applies a function on each element of vector
    concurrent_map(vector, size_of_vector, func, n_threads);

    // Prints mapped vector
    if (print){
        printf("\n\nMapped vector: ");
        print_vector(vector, size_of_vector);
        printf("\n\n");
    }

    // Frees the allocated space
    free(vector);

    return 0;
}