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

// Argument being passed to concurrent
typedef struct {
    float *seg_base1;
    float *seg_base2;
    float *accumulator_base;
    float seg_len;
} t_args2;

// Add function that will be passed to fold
float add(float a, float b){
    return a + b;
}

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
    float *accumulator = malloc(sizeof(float)); // Obs: this malloc is being freed in concurrent_fold
    *accumulator = args.init_value;

    // Combining each element on the vector
    for (int i=0 ; i<args.seg_len ; i++){
        *accumulator = args.func(args.seg_base[i], *accumulator);
    }

    // Returns the accumulator
    pthread_exit((void *) accumulator);
    return NULL;
}

// Initializes, launches and waits for threads to apply a function on a vector
float concurrent_fold(float vector[], long int len, float (*func)(float, float), int n_threads, float init_value){
    t_args *vector_of_args = malloc(n_threads * sizeof(t_args)); // Thread elements
    pthread_t *tids = malloc(n_threads * sizeof(pthread_t)); // Thread identifiers
    float *returned_value;
    float accumulator = init_value;

    for(int i=0 ; i<n_threads ; i++){
        // Initializes the threads arguments
        vector_of_args[i].seg_base = vector + (i * (len / n_threads));
        vector_of_args[i].seg_len = len / n_threads;
        if (i == n_threads - 1){ // Selects the last remaining elements of vector to the last thread
            vector_of_args[i].seg_len += len % n_threads;
        }
        vector_of_args[i].func = func;
        vector_of_args[i].init_value=init_value;

        // Creates and launches each thread
        if(pthread_create(&tids[i], NULL, thread_fold, &vector_of_args[i])){
            printf("Error: pthread_create didn't return 0.\n\n");
        }
    }

    // Waits for all threads to return
    for(int i=0 ; i<n_threads ; i++){
        if(pthread_join(tids[i], (void **) &returned_value)){
            printf("Error: pthread_join didn't return 0.\n\n");
            exit(EXIT_FAILURE);
        }
        accumulator = func(*returned_value, accumulator);
        free(returned_value); // Frees space allocated in thread_fold
    }

    // Frees the allocated space
    free(vector_of_args);
    free(tids);
    return accumulator;
}

// Function executed by each thread invoked by concurrent_dot_product
void *thread_dot_product(void *arguments){
    t_args2 args = *(t_args2 *) arguments; // Type-casting void* to t_args2

    // Computing each product for latter
    for(int i=0 ; i<args.seg_len ; i++){
        args.accumulator_base[i] = args.seg_base1[i] * args.seg_base2[i];
    }

    pthread_exit(NULL);
    return NULL;
}

// Initializes, launches and waits for threads to compute the dot product of two given vectors
float concurrent_dot_product(float vector1[], float vector2[], long int len, int n_threads){
    t_args2 *vector_of_args = malloc(n_threads * sizeof(t_args2)); // Thread elements
    pthread_t *tids = malloc(n_threads * sizeof(pthread_t));       // Thread identifiers
    float *auxiliar_vector = malloc(len * sizeof(float));          // Auxiliar vector
    float result;

    for(int i=0 ; i<n_threads ; i++){
        // Initializes the threads arguments
        int offset = (i * (len / n_threads));
        vector_of_args[i].seg_base1 = vector1 + offset;
        vector_of_args[i].seg_base2 = vector2 + offset;
        vector_of_args[i].accumulator_base = auxiliar_vector + offset;
        vector_of_args[i].seg_len = len / n_threads;
        if(i == n_threads - 1){ // Selects the last remaining elements of vector to the last thread
            vector_of_args[i].seg_len += len % n_threads;
        }

        // Creates and launches each thread
        if(pthread_create(&tids[i], NULL, thread_dot_product, &vector_of_args[i])){
            printf("Error: pthread_create didn't return 0.\n\n");
        }
    }
    // Waits for all threads to return
    for(int i=0 ; i<n_threads ; i++){
        if(pthread_join(tids[i], NULL)){
            printf("Error: pthread_join didn't return 0.\n\n");
            exit(EXIT_FAILURE);
        }
    }

    // Frees some of the allocated space
    free(vector_of_args);
    free(tids);

    // Computes the sum of the elements on the product c such that c[i] = a[i] * b[i]
    result = concurrent_fold(auxiliar_vector, len, add, n_threads, 0);

    // Frees the remaining allocated space
    free(auxiliar_vector);
    return result;
}

// Initializes a given vector with random values between 1 and 2
void init_vector(float vector[], long int len){
    for(int i=0 ; i<len ; i++){
        vector[i] = sample_float(1,2);
    }
}

// Prints a vector
void print_vector(float vector[], long int len){
    printf("[");
    for(int i=0 ; i<len ; i++){
        printf(" %f ", vector[i]);
    }
    printf("]");
}

// Main function
int main(int argc, char* argv[]){
    short int n_threads;      // Number of threads
    long int size_of_vector;  // Size of vector
    float *vector1;           // Vector 1
    float *vector2;           // Vector 2
    float result;             // Dot-product result
    char *filename;           // File name
    srand(time(0));           // Seed

    // Verifies if the program arguments are being passed
    if(argc < 3){
        printf("Error: there's an argument [n_threads] [filename] missing.\n\n");
        exit(EXIT_FAILURE);
    }

    // Captures program's input
    n_threads = atoi(argv[1]); // Initializes n_threads
    filename = argv[2];        // Initializes filename

    // Opens the file for reading and writing
    FILE *file = fopen(filename, "r+b");
    if(file == NULL){
        perror("Error: something happened when opening file.\n\n");
        exit(EXIT_FAILURE);
    }

    // Reads size_of_vector
    if(fread(&size_of_vector, sizeof(long int), 1, file) != 1){
        printf("Error: something happened when reading vector size from file.\n\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads <= 0
    if(n_threads <= 0){
        printf("Error: the number of threads needs to be positive.\n\n");
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads > size_of_vector
    if(size_of_vector < n_threads){
        printf("\nWarning: Number of threads limited to %ld.", size_of_vector);
        n_threads = size_of_vector;
    }

    vector1 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector1
    vector2 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector2

    // Reads vector1 and vector2 from file
    if(fread(vector1, sizeof(float), size_of_vector, file) != size_of_vector ||
       fread(vector2, sizeof(float), size_of_vector, file) != size_of_vector){
        printf("Error: something happened when reading vectors from file.\n\n");
        free(vector1);
        free(vector2);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Applies dot-product to both vectors
    result = concurrent_dot_product(vector1, vector2, size_of_vector, n_threads);

    // Prints the dot-product result
    // printf("\n\nDot product result: %f\n\n", result);

    // Writes the result back to the file (overwrite previous result)
    fseek(file, sizeof(long int) + 2 * size_of_vector * sizeof(float), SEEK_SET);
    fwrite(&result, sizeof(float), 1, file);

    // Frees the allocated space
    fclose(file);   // Closes file
    free(vector1);  // Frees the space allocated to vector1
    free(vector2);  // Frees the space allocated to vector2

    return 0;
}
