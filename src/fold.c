#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Argument being passed to thread_fold
typedef struct{
    float *seg_base;
    int seg_len;
    float (*func)(float, float);
    float init_value;
} t_args;

// Samples a float from 0 to 1
float rand_float(){
    return (float) rand() / (float) RAND_MAX;
}

// Samples a float from a to b
float sample_float(float a, float b){
    return rand_float() * (b - a) + a;
}

// Applies a function to each element of a given vector
void *thread_fold(void *arguments){
    t_args args = *(t_args *) arguments; // Type-casting void* to t_args
    float *accumulator = malloc(sizeof(float));
    *accumulator = args.init_value;

    // Combining each element on the vector
    for (int i=0 ; i<args.seg_len ; i++){
        *accumulator = args.func(args.seg_base[i], *accumulator);
    }

    // Returns the accumulator
    pthread_exit((void *) accumulator);
}

// Initializes, launches and waits for threads to apply a function on a vector
float concurrent_fold(float vector[], int len, float (*func)(float, float), int n_threads, float init_value){
    t_args *vector_of_args = malloc(n_threads * sizeof(t_args)); // Thread elements
    pthread_t *tids = malloc(n_threads * sizeof(pthread_t)); // Thread identifiers
    float *returned_value;
    float accumulator = init_value;

    for (int i=0 ; i<n_threads ; i++){
        // Initializes the threads arguments
        vector_of_args[i].seg_base = vector + (i * len / n_threads);
        vector_of_args[i].seg_len = len / n_threads;
        if (i == n_threads - 1){ // Selects the last remaining elements of vector to the last thread
            vector_of_args[i].seg_len += len % n_threads;
        }
        vector_of_args[i].func = func;
        vector_of_args[i].init_value = init_value;

        // Creates and launches each thread
        if(pthread_create(&tids[i], NULL, thread_fold, &vector_of_args[i])){
            printf("Error: pthread_create didn't return 0.\n\n");
        }
    }

    // Waits for all threads to return
    for (int i=0 ; i<n_threads; i++){
        if(pthread_join(tids[i], (void **) &returned_value)){
            printf("Error: pthread_join didn't return 0.\n\n");
            // Obs: this ``dead-end'' does not free the allocated memory
            exit(EXIT_FAILURE);
        }
        accumulator = func(*returned_value, accumulator);
        free(returned_value);
    }

    // Frees the allocated space
    free(vector_of_args);
    free(tids);
    return accumulator;
}

// Initializes a given vector as an enumeration
void init_vector(float vector[], int len){
    for (int i=0 ; i<len ; i++){
        vector[i] = sample_float(1,2);
    }
}

// Prints a vector
void print_vector(float vector[], int len){
    printf("[");
    for (int i=0 ; i<len ; i++){
        printf(" %f ", vector[i]);
    }
    printf("]");
}

// Add function that will be passed to fold
float add(float a, float b){
    return a + b;
}

// Product function that will be passed to fold
float mul(float a, float b){
    return a * b;
}

// Main function
int main(int argc, char* argv[]){
    short int n_threads; // Number of threads
    int size_of_vector; // Size of vector
    float *vector; // Vector
    int print; // Bool
    float result; // Result
    srand(time(0)); // Seed

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

    // Defendes the code in case print != 0 and print != 1
    if (print != 0 && print != 1){
        printf("\nprint? set to 1.");
        print = 1;
    }

    // allocates and initializes vector
    vector = malloc(size_of_vector * sizeof(float)); // Allocates space for vector
    init_vector(vector, size_of_vector); // Initializes vector

    // Prints original vector
    if (print){
        printf("\n\nOriginal vector: ");
        print_vector(vector, size_of_vector);
    }

    // Applies fold to the vector
    result = concurrent_fold(vector, size_of_vector, mul, n_threads, 1);

    // Prints the fold result
    if (print){
        printf("\n\nresult: %f\n\n", result);
    }

    // Frees the allocated space
    free(vector);

    return 0;
}
