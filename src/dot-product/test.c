#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char *filename = "data.bin";  // Test file name
    float sequential_result;      // Sequential dot product result
    float concurrent_result;      // Concurrent dot product result
    long int size_of_vector;      // Size of vector
    float relative_variance;      // Relative Variance
    char command[256];            // Command to the system

    // Verifies if the program arguments are being passed
    if(argc < 2){
        printf("Error: there's an argument [size_of_vector] missing.\n");
        exit(EXIT_FAILURE);
    }

    // Initializes size_of_vector
    size_of_vector = atol(argv[1]);

    // Executes the vector creator
    snprintf(command, sizeof(command), "./create-vector.out %ld %s", size_of_vector, filename);
    if(system(command) != 0){
        printf("Error: something happening while executing create-vector.\n\n");
        exit(EXIT_FAILURE);
    }
    printf("Created vector.\n");

    // Executes the sequential dot product
    if(system("./seq-dotp.out data.bin") != 0){
        printf("Error: something happening while executing seq-dotp.\n\n");
        exit(EXIT_FAILURE);
    }
    printf("Sequential dot-product executed.\n");

    // Reads sequential result (last float in the file)
    FILE *file = fopen(filename, "rb");                 // Opens file
    fseek(file, -((long)sizeof(float)), SEEK_END);      // Jumps to the last float
    fread(&sequential_result, sizeof(float), 1, file);  // Reads the last float
    fclose(file);                                       // Closes file

    // Executes the concurrent dot product
    if(system("./conc-dotp.out 7 data.bin") != 0){
        printf("Error: something happening while executing conc-dotp.\n\n");
        exit(EXIT_FAILURE);
    }
    printf("Concurrent dot-product executed.\n");

    // Reads concurrent result (last float in the file)
    file = fopen(filename, "rb");                       // Opens file
    fseek(file, -((long)sizeof(float)), SEEK_END);      // Jumps to the last float
    fread(&concurrent_result, sizeof(float), 1, file);  // Reads the last float
    fclose(file);                                       // Closes file

    // Computes relative variance
    relative_variance = (sequential_result - concurrent_result) / sequential_result;

    // Prints the obtained results
    printf("Sequential result: %f\n", sequential_result);
    printf("Concurrent result: %f\n", concurrent_result);
    printf("Relative Variance: %f\n", relative_variance);

    return 0;
}
