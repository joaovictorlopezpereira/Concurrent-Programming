#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __linux__
#   define COMMAND_PREFIX "./"
#else
#   define COMMAND_PREFIX ""
#endif // __linux__

// Computes the time elapsed given a start and a end time
double elapsed(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1e9;
}


// Main function
int main(int argc, char* argv[]) {
    char *filename = "data.bin";  // Test file name
    float sequential_result;      // Sequential dot product result
    float concurrent_result;      // Concurrent dot product result
    long int size_of_vector;      // Size of vector
    float relative_variance;      // Relative Variance
    char command[256];            // Command to the system
    double sequential_time;       // Sequential dot product time elapsed
    double concurrent_time;       // Concurrent dot product time elapsed
    struct timespec t1;           // Start time
    struct timespec t2;           // End time

    // Verifies if the program arguments are being passed
    if(argc < 2){
        printf("Error: there's an argument [size_of_vector] missing.\n");
        exit(EXIT_FAILURE);
    }

    // Initializes size_of_vector
    size_of_vector = atol(argv[1]);

    // Executes the vector creator
    snprintf(command, sizeof(command), COMMAND_PREFIX "create-vector %ld %s", size_of_vector, filename);
    if(system(command) != 0){
        printf("Error: something happening while executing create-vector.\n\n");
        exit(EXIT_FAILURE);
    }
    printf("Created vector.\n");

    // Saves the current time (before sequential dot product)
    clock_gettime(CLOCK_MONOTONIC, &t1);

    // Executes the sequential dot product
    if(system(COMMAND_PREFIX "seq-dotp data.bin") != 0){
        printf("Error: something happening while executing seq-dotp.\n\n");
        exit(EXIT_FAILURE);
    }

    // Saves current time (after sequential dot product)
    clock_gettime(CLOCK_MONOTONIC, &t2);

    // Computes the elapsed time by sequential dot product
    sequential_time = elapsed(t1, t2);

    // Tells the user that the sequential dot product executed
    printf("Sequential dot-product executed.\n");

    // Reads sequential result (last float in the file)
    FILE *file = fopen(filename, "rb");                 // Opens file
    fseek(file, -((long)sizeof(float)), SEEK_END);      // Jumps to the last float
    fread(&sequential_result, sizeof(float), 1, file);  // Reads the last float
    fclose(file);                                       // Closes file

    // Saves current time (before concurrent dot product)
    clock_gettime(CLOCK_MONOTONIC, &t1);

    // Executes the concurrent dot product
    if(system(COMMAND_PREFIX "conc-dotp 8 data.bin") != 0){
        printf("Error: something happening while executing conc-dotp.\n\n");
        exit(EXIT_FAILURE);
    }

    // Saves current time (after concurrent dot product)
    clock_gettime(CLOCK_MONOTONIC, &t2);

    // Computes the elapsed time by concurrent dot product
    concurrent_time = elapsed(t1, t2);

    // Tells the user that the concurrent dot product executed
    printf("Concurrent dot-product executed.\n\n");

    // Reads concurrent result (last float in the file)
    file = fopen(filename, "rb");                       // Opens file
    fseek(file, -((long)sizeof(float)), SEEK_END);      // Jumps to the last float
    fread(&concurrent_result, sizeof(float), 1, file);  // Reads the last float
    fclose(file);                                       // Closes file

    // Computes relative variance
    relative_variance = (sequential_result - concurrent_result) / sequential_result;

    // Prints the obtained results
    printf("Sequential result: %f\n", sequential_result);
    printf("Concurrent result: %f\n\n", concurrent_result);
    printf("Relative Variance: %f\n\n", relative_variance);
    printf("Sequential time: %g s\n", sequential_time);
    printf("Concurrent time: %g s\n\n", concurrent_time);

    return 0;
}
