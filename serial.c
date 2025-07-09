#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filename> <substring>\n", argv[0]);
        return -1;
    }

    // Opens file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", argv[1]);
        return -1;
    }

    // Finds file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    // Allocates memory for s1
    char *s1 = malloc(file_size + 1);
    if (s1 == NULL) {
        printf("Memory allocation failed\n");
        fclose(fp);
        return -1;
    }

    // Reads file into s1
    fread(s1, 1, file_size, fp);
    s1[file_size] = '\0';

    fclose(fp);

    char *s2 = argv[2];
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int count = 0;

    // Serial search for s2 in s1
    for (int i = 0; i <= len1 - len2; i++) {
        if (strncmp(s1 + i, s2, len2) == 0) {
            count++;
        }
    }

    printf("Total occurrences: %d\n", count);

    free(s1);
    return 0;
}
