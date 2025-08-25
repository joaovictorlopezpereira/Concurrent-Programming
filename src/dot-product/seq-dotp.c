#include <stdio.h>
#include <stdlib.h>

// Prints a vector
void print_vector(float vector[], long int len){
    printf("[");
    for(long int i=0 ; i<len ; i++){
        printf(" %f ", vector[i]);
    }
    printf("]");
}

// Computes the dot product between two given vectors
float dot_product(float vector1[], float vector2[], long int size_of_vector){
    float accumulator = 0;
    for(long int i=0 ; i<size_of_vector ; i++){
        accumulator += vector1[i] * vector2[i];
    }
    return accumulator;
}

// Main function
int main(int argc, char* argv[]){
    long int size_of_vector;  // Size of vectors 1 and 2
    float *vector1;           // Pointer to vector 1
    float *vector2;           // Pointer to vector 2
    float result;             // Dot product result
    char *filename;           // File name

    // Verifies if the program arguments are being passed
    if(argc < 2){
        printf("Error: there's an argument [filename] missing.\n\n");
        exit(EXIT_FAILURE);
    }

    filename = argv[1];  // Initializes filename

    // Opens the file by its name
    FILE *file = fopen(filename, "rb+");
    if(file == NULL){
        printf("Error: something happening when opening file.\n\n");
        exit(EXIT_FAILURE);
    }

    // Reads size_of_vector
    fread(&size_of_vector, sizeof(long int), 1, file);

    vector1 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector1
    vector2 = malloc(size_of_vector * sizeof(float));  // Allocates space for vector2

    // Reads vectors
    fread(vector1, sizeof(float), size_of_vector, file);
    fread(vector2, sizeof(float), size_of_vector, file);

    // Computes the dot_product
    result = dot_product(vector1, vector2, size_of_vector);

    // Prints the dot_product result
    // printf("\n\nDot product result: %f\n\n", result);

    // Writes the dot-product result at the end of the file
    fseek(file, 0, SEEK_END);
    fwrite(&result, sizeof(float), 1, file);

    fclose(file);   // Closes file
    free(vector1);  // Frees the space allocated to vector1
    free(vector2);  // Frees the space allocated to vector2

    return 0;
}
