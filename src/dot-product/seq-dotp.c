#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Prints a vector
void print_vector(float vector[], int len){
    printf("[");
    for(int i=0 ; i<len ; i++){
        printf(" %f ", vector[i]);
    }
    printf("]");
}

// Samples a float from 0 to 1
float rand_float(){
    return (float) rand() / (float) RAND_MAX;
}

// Samples a float from a to b
float sample_float(float a, float b){
    return rand_float() * (b - a) + a;
}

// Computes the dot product between two given vectors
float dot_product(float vector1[], float vector2[], int size_of_vector){
    float accumulator = 0;
    for(int i=0 ; i<size_of_vector ; i++){
        accumulator += vector1[i] * vector2[i];
    }
    return accumulator;
}

// Initializes a given vector with random values between 1 and 2
void init_vector(float vector[], int len){
    for(int i=0 ; i<len ; i++){
        vector[i] = sample_float(1,2);
    }
}

// Main function
int main(int argc, char* argv[]){
    short int n_threads; // Number of threads
    int size_of_vector;  // Size of vectors 1 and 2
    int print;           // Boolean
    float *vector1;      // Pointer to vector 1
    float *vector2;      // Pointer to vector 2
    float result;        // Dot product result
    srand(time(0));      // Seed

    // Verifies if the program arguments are being passed
    if(argc < 4){
        printf("Error: there's an argument [n_threads] or [size_of_vector] or [print?] missing.\n\n");
        exit(EXIT_FAILURE);
    }

    n_threads = atoi(argv[1]);      // Initializes n_threads
    size_of_vector = atoi(argv[2]); // Initializes size_of_vector
    print = atoi(argv[3]);          // Initializes print

    // Defends the code in case size_of_vector < 0
    if(size_of_vector < 0){
        printf("Error: the size of vector needs to be non-negative.\n\n");
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads <= 0
    if(n_threads <= 0){
        printf("Error: the number of threads needs to be positive.\n\n");
        exit(EXIT_FAILURE);
    }

    // Defends the code in case n_threads > size_of_vector
    if(size_of_vector < n_threads){
        printf("\nNumber of threads limited to %d.", size_of_vector);
        n_threads = size_of_vector;
    }

    // Defendes the code in case print != 0 and print != 1
    if(print != 0 && print != 1){
        printf("\nprint? set to 1.");
        print = 1;
    }

    vector1 = malloc(size_of_vector * sizeof(float)); // Allocates space for vector1
    vector2 = malloc(size_of_vector * sizeof(float)); // Allocates space for vector1
    init_vector(vector1, size_of_vector);             // Initializes vector1
    init_vector(vector2, size_of_vector);             // Initializes vector2

    // Prints vector1 and vector2
    if(print){
        printf("\n\nOriginal vectors: ");
        print_vector(vector1, size_of_vector);
        printf("\n");
        print_vector(vector2, size_of_vector);
    }

    // Computes the dot_product
    result = dot_product(vector1, vector2, size_of_vector);

    // Prints the dot-product result
    if(print){
        printf("\n\nresult: %f\n\n", result);
    }

    // Frees the allocated space
    free(vector1);
    free(vector2);

    return 0;
}