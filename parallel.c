#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

char *s1; 
char *s2;
int count = 0; 
pthread_mutex_t count_mutex;

// Structure for passing start and end indices to each thread
typedef struct {
    int start; 
    int end;  
} ThreadData;

// The function executed by each thread
void *thread_func(void *arg) {
    ThreadData *data = (ThreadData*) arg;
    int local_count = 0;
    int len2 = strlen(s2);

    // Calculates last index the thread should check as a start position
    int last_i = data->end - len2 + 1;
    int len1 = strlen(s1);
    if (last_i > len1 - len2) {
        last_i = len1 - len2;
    }

    // Loops through the thread's chunk of s1. Only go up to valid last start index
    for (int i = data->start; i <= last_i; i++) {
        // Compares substring of s1 starting at i to s2
        if (strncmp(s1 + i, s2, len2) == 0) {
            local_count++;
        }
    }

    // Locks mutex to safely update the shared global count
    pthread_mutex_lock(&count_mutex);
    count += local_count;
    pthread_mutex_unlock(&count_mutex);

    printf("The number of substrings thread %d find is %d\n", data->thread_id, local_count);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Checks number of command-line arguments
    if (argc != 3) {
        printf("Usage: %s <filename> <string2>\n", argv[0]);
        return -1;
    }

    // Opens the file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return -1;
    }

    // Determines file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    // Allocates memory for s1
    s1 = malloc(file_size + 1);
    if (s1 == NULL) {
        printf("Memory allocation failed\n");
        fclose(fp);
        return -1;
    }

    // Reads the entire file into s1
    fread(s1, 1, file_size, fp);
    s1[file_size] = '\0';

    fclose(fp);

    s2 = argv[2];
    int num_threads = 10; // Hard-coded to 10 threads as required

    pthread_mutex_init(&count_mutex, NULL);

    int len1 = strlen(s1);
    int len2 = strlen(s2);

    // Divides s1 into roughly equal-sized chunks for each thread
    int chunk_size = len1 / num_threads;

    pthread_t threads[num_threads];
    ThreadData data[num_threads];

    for (int i = 0; i < num_threads; i++) {
        // Calculates start index for this thread
        data[i].start = i * chunk_size;

        // Calculates end index for this thread
        if (i == num_threads - 1) {
            // Last thread goes to end of string
            data[i].end = len1 - 1;
        } else {
            // Other threads extend their chunk slightly to allow for possible overlapping substrings between chunks
            data[i].end = (i + 1) * chunk_size + len2 - 2;

            if (data[i].end >= len1) {
                data[i].end = len1 - 1;
            }
        }

        // Creates thread
        pthread_create(&threads[i], NULL, thread_func, &data[i]);
    }

    // Waits for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Prints the total number of times s2 was found in s1
    printf("Total occurrences: %d\n", count);

    pthread_mutex_destroy(&count_mutex);
    free(s1);
    return 0;
}
