#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Samples a float from 0 to 1
float rand_float(){
    return (float) rand() / (float) RAND_MAX;
}

// Samples a float from a to b
float sample_float(float a, float b){
    return rand_float() * (b - a) + a;
}

// Initializes a given vector with random values between 1 and 2
void init_vector(float vector[], long int len){
    for(long int i=0 ; i<len ; i++){
        vector[i] = sample_float(1,2);
    }
}

// Main function
int main(int argc, char* argv[]){
    // Verifies if the program arguments are being passed
    if(argc < 3){
        printf("Error: there's an argument [size_of_vector] [filename] missing.\n\n");
        exit(EXIT_FAILURE);
    }

    long int size_of_vector = atol(argv[1]);                  // Initializes size_of_vector
    char *filename = argv[2];                                 // Initializes filename
    float *vector1 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector1
    float *vector2 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector1
    init_vector(vector1, size_of_vector);                     // Initializes vector1
    init_vector(vector2, size_of_vector);                     // Initializes vector2
    srand(time(0));                                           // Seed

    // Opens the file by its name
    FILE *file = fopen(filename, "wb");
    if(file == NULL){
        printf("Error: something happened when opening the file.\n\n");
        free(vector1);
        free(vector2);
        exit(EXIT_FAILURE);
    }

    fwrite(&size_of_vector, sizeof(long int), 1, file);    // Writes size_of_vector
    fwrite(vector1, sizeof(float), size_of_vector, file);  // Writes vector1
    fwrite(vector2, sizeof(float), size_of_vector, file);  // Writes vector2

    fclose(file);   // Closes file
    free(vector1);  // Frees the space allocated to vector1
    free(vector2);  // Frees the space allocated to vector2

    return 0;
}
