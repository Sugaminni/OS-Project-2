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

    // Loop through the thread's chunk of s1. Only go up to (end - len2 + 1) to avoid overflow
    for (int i = data->start; i <= data->end - len2 + 1; i++) {
        // Compares substring of s1 starting at i to s2
        if (strncmp(s1 + i, s2, len2) == 0) {
            local_count++;
        }
    }

    // Locks mutex to safely update the shared global count
    pthread_mutex_lock(&count_mutex);
    count += local_count;
    pthread_mutex_unlock(&count_mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Checks number of command-line arguments
    if (argc != 4) {
        printf("Usage: %s <string1> <string2> <num_threads>\n", argv[0]);
        return -1;
    }

    // Reads command-line arguments
    s1 = argv[1];
    s2 = argv[2];
    int num_threads = atoi(argv[3]);

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
    return 0;
}
