#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include <sys/time.h>
#include <sys/stat.h>

typedef unsigned long long int u_long_long;

#define PAGE_SIZE 1000
#define N_PAGES 100000L


char BLANK_PAGE[PAGE_SIZE];

u_long_long NUM_OF_PAGES = N_PAGES;
u_long_long FILE_SIZE = 0;

void fill_indexes(u_long_long arr[], u_long_long n);
void read_sequentially(int fd, char page[], u_long_long offsets[]);
void read_random(int fd, char page[], u_long_long offsets[]);
void shuffle(u_long_long *array, u_long_long n);
char *preallocate_test_file(char filename[], char end[]);
void shuffle_page(char *array, int n);
void handle(const char *string, int error)
{
    if (error) {
        perror(string);
        exit(EXIT_FAILURE);
    }
}
off64_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}
int main(int argc, char **argv)
{
    if (argc != 3) {
        // ./teste /path/to/file/file500.dat 500000
        // ./teste /path/to/file/file1000.dat 1000000
        printf("Usage: teste <path/to/filename> <page_size>\n");
        exit(EXIT_SUCCESS);
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    printf("factor: %d\n", atoi(argv[2]) / PAGE_SIZE);
    NUM_OF_PAGES *= (atoi(argv[2]) / PAGE_SIZE);
    FILE_SIZE = PAGE_SIZE * NUM_OF_PAGES;


    char *filename = argv[1];

    u_long_long *offsets = malloc(NUM_OF_PAGES * sizeof(u_long_long));

    fill_indexes(offsets, NUM_OF_PAGES);

    printf("NUM_OF_PAGES: %llu\n", NUM_OF_PAGES);
    printf("FILE_SIZE: %llu\n", FILE_SIZE);
    printf("PAGE_SIZE: %i\n", PAGE_SIZE);

    shuffle(offsets, NUM_OF_PAGES);

//  printf("%llu\n", offsets[0]);

    char *page = malloc(PAGE_SIZE);

    char *seq_file = preallocate_test_file(argv[1], "_seq");
    char *random_file = preallocate_test_file(argv[1], "_random");
 
    int fd_seq, fd_random, retval;

    fd_seq = open(seq_file, O_RDONLY);
    handle("open seq file", fd_seq < 0);

    //printf("size: %li\n", (off64_t) fsize(argv[1]));
 
    struct timeval  tv1, tv2;

    printf("fd seq: %d\n", fd_seq);

    gettimeofday(&tv1, NULL);
    printf("Sequential read started...\n");
    read_sequentially(fd_seq, page, offsets);
    gettimeofday(&tv2, NULL);

    double seq_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Sequential read total time = %f seconds\n", seq_time);

    fd_random = open(random_file, O_RDONLY);
    handle("open seq file", fd_random < 0);

    //printf("size: %li\n", (off64_t) fsize(argv[1]));
 
    printf("fd random: %d\n", fd_random);

    gettimeofday(&tv1, NULL);
    printf("Random read started...\n");
    read_random(fd_random, page, offsets);
    gettimeofday(&tv2, NULL);

    double rand_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Random read total time = %f seconds\n", rand_time);



    printf("Random time is %f slower than Sequential time:\n", rand_time / seq_time);

    unlink(seq_file);
    unlink(random_file);

/*    unlink(data_file);
    printf("deleting the file: %s...\n", data_file);
*/
    free(offsets);
    free(seq_file);
    free(random_file);
    
    free(page);

    close(fd_seq);
    close(fd_random);

    return 0;
}
void fill_indexes(u_long_long arr[], u_long_long n)
{
    u_long_long i;
    for (i = 0; i < n; i++)
        arr[i] = i;
}
void read_sequentially(int fd, char page[], u_long_long offsets[])
{
    u_long_long i;
    u_long_long retval = 0;
    
    lseek64(fd, 0, SEEK_SET);

    for (i = 0; i < NUM_OF_PAGES; i++) {
        // retval = lseek64(fd, i * PAGE_SIZE, SEEK_SET);
        // printf("pos: %llu\n", (u_long_long) lseek(fd, 0, SEEK_CUR));
        //printf("i: %llu\n", i);
        handle("lseek64", retval == (off_t) - 1);
        retval = read(fd, page, PAGE_SIZE);
        handle("read", retval < 0);
    }
}
void read_random(int fd, char page[], u_long_long offsets[])
{
    u_long_long i;
    u_long_long retval = 0;

    lseek64(fd, 0, SEEK_SET);

    for (i = 0; i < NUM_OF_PAGES; i++) {
        //printf("pos: %llu\n", (u_long_long) lseek(fd, 0, SEEK_CUR));
        //printf("i: %llu\n", i);
        retval = lseek64(fd, offsets[i] * PAGE_SIZE, SEEK_SET);
        // printf("i: %llu\n", offsets[i]);
        handle("lseek64", retval == (off_t) - 1);
        retval = read(fd, page, PAGE_SIZE);
        handle("read", retval < 0);
    }
}
void shuffle(u_long_long *array, u_long_long n)
{
    srand(time(NULL));
    if (n > 1) {
        u_long_long i;
        for (i = 0; i < n - 1; i++) {
            u_long_long j = i + rand() / (RAND_MAX / (n - i) + 1);
            u_long_long t = array[j];
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

char *preallocate_test_file(char filename[], char end[])
{
    FILE *fp;

    char *page = malloc(PAGE_SIZE);

    char *new_filename = malloc(strlen(filename) + strlen(end) + 1);

    strcpy(new_filename,  filename);
    strcat(new_filename, end);

    printf("%s\n", new_filename);

    if(access(new_filename, F_OK ) != -1 ) {
        printf("file: %s already exists\n", new_filename);
    } else {
        printf("creating a new file: %s...\n", new_filename);
        int i = 0;
        for (i = 0; i < PAGE_SIZE; i++) {
            page[i] = ((i + 65) % 122);
        }

        if ((fp = fopen(new_filename, "w+")) == NULL) {
            printf("Couldn't create new file\n");
            exit(1);
        } 

        u_long_long j;
        for (j = 0; j < FILE_SIZE; j += PAGE_SIZE) {
            shuffle_page(page, PAGE_SIZE);
            fwrite(page, PAGE_SIZE, 1, fp);
        }

        fclose(fp);     
    }

    free(page);

    return new_filename;
}