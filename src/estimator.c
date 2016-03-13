#include "estimator.h"

Estimator *create_estimator(Strategy strategy, char *file_name, long num_of_pages)
{
	Estimator *new_estimator = malloc(sizeof(Estimator));

	new_estimator->elapsed_time = 0.0;
	new_estimator->strategy = strategy;

	return new_estimator;
}

void run_test(Estimator *estimator)
{
	setvbuf(stdout, NULL, _IONBF, 0);

    long file_size = PAGE_SIZE * estimator->num_of_pages;

    long long *offsets = malloc(estimator->num_of_pages * sizeof(long long));

    fill_indexes(offsets, estimator->num_of_pages);

    printf("NUM_OF_PAGES: %ld\n", estimator->num_of_pages);
    printf("FILE_SIZE: %ld\n", file_size);
    printf("PAGE_SIZE: %i\n", PAGE_SIZE);

    shuffle(offsets, estimator->num_of_pages);

    char *file = preallocate_test_file(estimator->file_name, file_size);
 
    int fd, retval;

    fd = open(file, O_RDONLY);
    handle("open file", fd < 0);

	struct timeval  tv1, tv2;

    gettimeofday(&tv1, NULL);
    estimator->strategy(fd, estimator->num_of_pages, offsets);
    gettimeofday(&tv2, NULL);

    double seq_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
             (double) (tv2.tv_sec - tv1.tv_sec);

    estimator->elapsed_time = seq_time;
}

double get_result(Estimator *estimator)
{
	return estimator->elapsed_time;
}

void sequential_strategy(int fd, long long num_of_pages, long long offsets[])
{
    char *page = malloc(PAGE_SIZE);
	long long i;
    long long retval = 0;
    
    lseek64(fd, 0, SEEK_SET);

    for (i = 0; i < num_of_pages; i++) {
        retval = lseek64(fd, i * PAGE_SIZE, SEEK_SET);
        // printf("pos: %llu\n", (u_long_long) lseek(fd, 0, SEEK_CUR));
        //printf("i: %llu\n", i);
        handle("lseek64", retval == (off_t) - 1);
        retval = read(fd, page, PAGE_SIZE);
        handle("read", retval < 0);
    }
    free(page);
}

void random_strategy(int fd, long long num_of_pages, long long offsets[])
{
    char *page = malloc(PAGE_SIZE);
	long long i;
    long long retval = 0;

    lseek64(fd, 0, SEEK_SET);

    for (i = 0; i < num_of_pages; i++) {
        //printf("pos: %llu\n", (u_long_long) lseek(fd, 0, SEEK_CUR));
        //printf("i: %llu\n", i);
        retval = lseek64(fd, offsets[i] * PAGE_SIZE, SEEK_SET);
        // printf("i: %llu\n", offsets[i]);
        handle("lseek64", retval == (off_t) - 1);
        retval = read(fd, page, PAGE_SIZE);
        handle("read", retval < 0);
    }
    free(page);
}