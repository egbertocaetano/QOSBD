#include "util.h"

void fill_indexes(long long arr[], long long n)
{
    long long i;
    for (i = 0; i < n; i++)
        arr[i] = i;
}
void shuffle(long long *array, long long n)
{
    srand(10);
    if (n > 1) {
        long long i;
        for (i = 0; i < n - 1; i++) {
            long long j = i + rand() / (RAND_MAX / (n - i) + 1);
            long long t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
void shuffle_page(char *array, int n)
{
    srand(time(NULL));
    if (n > 1) {
        int i;
        for (i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void preallocate_test_file(char filename[], long long file_size)
{
    FILE *fp;

    char *page = malloc(PAGE_SIZE);

    printf("creating file of size: %lld\n", file_size);

    if(access(filename, F_OK ) != -1 ) {
        printf("file: %s already exists\n", filename);
    } else {
        printf("creating a new file: %s...\n", filename);
        int i = 0;
        for (i = 0; i < PAGE_SIZE; i++) {
            page[i] = ((i + 65) % 122);
        }

        if ((fp = fopen(filename, "w+")) == NULL) {
            printf("Couldn't create new file\n");
            exit(1);
        } 

        long long j;
        for (j = 0; j < file_size; j += PAGE_SIZE) {
            shuffle_page(page, PAGE_SIZE);
            fwrite(page, PAGE_SIZE, 1, fp);
        }

        fclose(fp);     
    }

    free(page);
}

void handle(const char *string, int error)
{
    if (error) {
        perror(string);
        exit(EXIT_FAILURE);
    }
}

char *filename_format(char *filename, char *end)
{
    char *new_filename = malloc(strlen(filename) + strlen(end) + 1);

    strcpy(new_filename,  filename);
    strcat(new_filename, end);

    return new_filename;
}
