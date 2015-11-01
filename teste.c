#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include <sys/time.h>
#include <sys/stat.h>

typedef unsigned long u_long;

#define PAGE_SIZE 512
#define NUM_OF_PAGES (2000 * 1000L)
#define FILE_SIZE (PAGE_SIZE * NUM_OF_PAGES)

char BLANK_PAGE[PAGE_SIZE];

void fill_indexes(u_long arr[], u_long n);
void read_sequentially(int fd, char page[], u_long offsets[]);
void read_random(int fd, char page[], u_long offsets[]);
void shuffle(u_long *array, u_long n);
void preallocate_test_file(char filename[]);
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
	u_long *offsets = malloc(NUM_OF_PAGES * sizeof(u_long));

	fill_indexes(offsets, NUM_OF_PAGES);

	//printf("%lu\n", offsets[NUM_OF_PAGES - 1]);

	shuffle(offsets, NUM_OF_PAGES);

	//printf("%lu\n", offsets[0]);

	//printf("rand: %ld\n", rand() / (RAND_MAX / (NUM_OF_PAGES - 0) + 1));

	char page[PAGE_SIZE];

	if (argc != 2) {
		printf("Usage: teste <path/to/filename>\n");
		exit(EXIT_SUCCESS);
	}

	//preallocate_test_file(argv[1]);
 
	int fd, retval;

	fd = open(argv[1], O_RDONLY);
	handle("open", fd < 0);

	printf("size: %ld\n", (off64_t) fsize(argv[1]));
 
    struct timeval  tv1, tv2;

    gettimeofday(&tv1, NULL);
	printf("Random read started...\n");
	read_random(fd, page, offsets);
	gettimeofday(&tv2, NULL);

	printf("Random read total time = %f seconds\n",
	         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	         (double) (tv2.tv_sec - tv1.tv_sec));

	gettimeofday(&tv1, NULL);
	printf("Sequential read started...\n");
	read_sequentially(fd, page, offsets);
	gettimeofday(&tv2, NULL);

	printf("Sequential read total time = %f seconds\n",
	         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	         (double) (tv2.tv_sec - tv1.tv_sec));

    free(offsets);
    close(fd);

	return 0;
}
void fill_indexes(u_long arr[], u_long n)
{
	u_long i;
	for (i = 0; i < n; i++)
		arr[i] = i;
}
void read_sequentially(int fd, char page[], u_long offsets[])
{
	u_long i;
	u_long retval = 0;
	off_t offset;
	
	lseek64(fd, 0, SEEK_SET);

	for (i = 0; i < NUM_OF_PAGES; i++) {
		retval = lseek64(fd, i * PAGE_SIZE, SEEK_SET);
		//printf("pos: %ld\n", retval);
		handle("lseek64", retval == (off_t) - 1);
		retval = read(fd, page, PAGE_SIZE);
		handle("read", retval < 0);
	}
}
void read_random(int fd, char page[], u_long offsets[])
{
	u_long i;
	u_long retval = 0;
	off_t offset;

	lseek64(fd, 0, SEEK_SET);

	for (i = 0; i < NUM_OF_PAGES; i++) {
		//printf("pos: %d\n", (int) lseek(fd, retval, SEEK_CUR));
		retval = lseek64(fd, offsets[i] * PAGE_SIZE, SEEK_SET);
		handle("lseek64", retval == (off_t) - 1);
		retval = read(fd, page, PAGE_SIZE);
		handle("read", retval < 0);
	}
}
void shuffle(u_long *array, u_long n)
{
	srand(time(NULL));
	if (n > 1) {
		u_long i;
		for (i = 0; i < n - 1; i++) {
			u_long j = i + rand() / (RAND_MAX / (n - i) + 1);
			u_long t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

void preallocate_test_file(char filename[])
{
	FILE *fp;

    if ((fp = fopen(filename, "w+")) == NULL) {
        printf("Couldn't create new file\n");
        exit(1);
    } 
    u_long i;
	for (i = 0; i < FILE_SIZE; i += PAGE_SIZE) {
		fwrite(BLANK_PAGE, PAGE_SIZE, 1, fp);
	}

	fclose(fp);
}