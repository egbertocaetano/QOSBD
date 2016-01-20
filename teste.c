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

#define BLOCKSIZE 8192

u_long_long NUM_OF_BLOCKS = 1000000L;
u_long_long FILE_SIZE = 0;

void fill_indexes(u_long_long arr[], u_long_long n);
void shuffle(u_long_long *array, u_long_long n);
char *preallocate_test_file(char filename[], char end[]);
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
void run_seq(int fd, u_long_long offsets[]);
void run_random(int fd, u_long_long offsets[]);
int main(int argc, char **argv)
{
    int fd, retval;
    unsigned long numblocks;
    off64_t offset;


    u_long_long *offsets = malloc(NUM_OF_BLOCKS * sizeof(u_long_long));

    fill_indexes(offsets, NUM_OF_BLOCKS);
    shuffle(offsets, NUM_OF_BLOCKS);

    printf("NUM_OF_BLOCKS: %llu\n", NUM_OF_BLOCKS);
    printf("FILE_SIZE: %llu\n", NUM_OF_BLOCKS * BLOCKSIZE);
    printf("BLOCKSIZE: %i\n", BLOCKSIZE);
    printf("First offset index: %llu\n", offsets[0]);



    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc != 2) {
        printf("Usage: sudo teste <raw disk device>\n");
        exit(EXIT_SUCCESS);
    }

    fd = open(argv[1], O_RDONLY);
    handle("open", fd < 0);

    retval = ioctl(fd, BLKGETSIZE64, &numblocks);
    handle("ioctl", retval == -1);


    struct timeval  tv1, tv2;

    gettimeofday(&tv1, NULL);
    printf("Sequential read started...\n");
    run_seq(fd, offsets);
    gettimeofday(&tv2, NULL);

    double seq_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Sequential read total time = %f seconds\n\n", seq_time);



    gettimeofday(&tv1, NULL);
    printf("Random read started...\n");
    run_random(fd, offsets);
    gettimeofday(&tv2, NULL);

    double rand_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    printf("Random read total time = %f seconds\n", rand_time);

    printf("Factor: %.4f\n", rand_time / seq_time);


    free(offsets);

    return 0;
}

void run_seq(int fd, u_long_long offsets[]) 
{
    int retval;
    char buffer[BLOCKSIZE];
    u_long_long i = 0;
    lseek64(fd, 0, SEEK_SET);
    for (i = 0; i < NUM_OF_BLOCKS; i++) {
        // printf("pos[%llu]: %llu\n", i, (u_long_long) lseek64(fd, 0, SEEK_CUR));
        handle("lseek64", retval == (off64_t) -1);
        retval = read(fd, buffer, BLOCKSIZE);
        handle("read", retval < 0);
    }
}

void run_random(int fd, u_long_long offsets[]) 
{
    int retval;
    char buffer[BLOCKSIZE];
    u_long_long i = 0;
    lseek64(fd, 0, SEEK_SET);
    for (i = 0; i < NUM_OF_BLOCKS; i++) {
        // printf("pos[%llu]: %llu\n", i, (u_long_long) lseek64(fd, 0, SEEK_CUR));
        // printf("num of blocks %llu\n", numblocks);
        // printf("pos[%llu]: %llu\n", offset);
        retval = lseek64(fd, BLOCKSIZE * offsets[i], SEEK_SET);
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

void shuffle(u_long_long *array, u_long_long n)
{
    srand(10);
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
