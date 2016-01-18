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
#define BLOCKSIZE 512

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
void run_seq(int fd, unsigned long numblocks, off64_t offset);
void run_random(int fd, unsigned long numblocks, off64_t offset);
int main(int argc, char **argv)
{
    int fd, retval;
    unsigned long numblocks;
    off64_t offset;

    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc != 2) {
        printf("Usage: seeker <raw disk device>\n");
        exit(EXIT_SUCCESS);
    }

    fd = open(argv[1], O_RDONLY);
    handle("open", fd < 0);

    retval = ioctl(fd, BLKGETSIZE64, &numblocks);
    handle("ioctl", retval == -1);


    struct timeval  tv1, tv2;

    gettimeofday(&tv1, NULL);
    printf("Sequential read started...\n");
    run_seq(fd, numblocks, offset);
    gettimeofday(&tv2, NULL);

    double seq_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Sequential read total time = %f seconds\n", seq_time);



    gettimeofday(&tv1, NULL);
    printf("Random read started...\n");
    run_random(fd, numblocks, offset);
    gettimeofday(&tv2, NULL);

    double rand_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Random read total time = %f seconds\n", rand_time);

    return 0;
}

void run_seq(int fd, unsigned long numblocks, off64_t offset) 
{
    int retval;
    char buffer[BLOCKSIZE];
    u_long_long i = 0;
    lseek64(fd, 0, SEEK_SET);
    for (i = 0; i < 1000000; i++) {
        //printf("pos[%llu]: %llu\n", i, (u_long_long) lseek64(fd, 0, SEEK_CUR));

        handle("lseek64", retval == (off64_t) -1);
        retval = read(fd, buffer, BLOCKSIZE);
        handle("read", retval < 0);
    }
}

void run_random(int fd, unsigned long numblocks, off64_t offset) 
{
    int retval;
    char buffer[BLOCKSIZE];
    u_long_long i = 0;
    lseek64(fd, 0, SEEK_SET);
    for (i = 0; i < 1000000; i++) {
        //printf("pos[%llu]: %llu\n", i, (u_long_long) lseek64(fd, 0, SEEK_CUR));
        // printf("num of blocks %llu\n", numblocks);
        offset = (off64_t) (numblocks/1024) * random() / RAND_MAX;
        // printf("pos[%llu]: %llu\n", offset);
        retval = lseek64(fd, BLOCKSIZE * offset, SEEK_SET);
        handle("lseek64", retval == (off64_t) -1);
        retval = read(fd, buffer, BLOCKSIZE);
        handle("read", retval < 0);
    }
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