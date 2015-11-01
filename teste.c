#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

typedef unsigned long u_long;

#define PAGE_SIZE 512
#define NUM_OF_PAGES (2000 * 10000L)
#define FILE_SIZE (PAGE_SIZE * NUM_OF_PAGES)

char BLANK_PAGE[PAGE_SIZE];

void fill_indexes(u_long arr[], u_long n);
void read_sequentially(FILE* fp, char page[], u_long offsets[]);
void read_random(FILE* fp, char page[], u_long offsets[]);
void shuffle(u_long *array, u_long n);

int main(void)
{
	u_long *offsets = malloc(NUM_OF_PAGES * sizeof(u_long));
	fill_indexes(offsets, NUM_OF_PAGES);

	//printf("%lu\n", offsets[NUM_OF_PAGES - 1]);

	shuffle(offsets, NUM_OF_PAGES);

	//printf("%lu\n", offsets[0]);

	//printf("rand: %ld\n", rand() / (RAND_MAX / (NUM_OF_PAGES - 0) + 1));


	char page[PAGE_SIZE];
	char filename[] = "/media/miguel/Novo HD/test.dat";
    FILE *fp;

    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Couldn't open test1.dat for reading\n");
        exit(1);
    } 

/*    clock_t tic, toc;
    tic = clock();

    read_sequentially(fp, page, offsets);

    toc = clock();

    printf("Sequential read elapsed time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();

    read_random(fp, page, offsets);

    toc = clock();

    printf("Random read elapsed time: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    */

    struct timeval  tv1, tv2;

	gettimeofday(&tv1, NULL);
	printf("Sequential read started...\n");
	read_sequentially(fp, page, offsets);
	gettimeofday(&tv2, NULL);

	printf ("Sequential read total time = %f seconds\n",
	         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	         (double) (tv2.tv_sec - tv1.tv_sec));

	gettimeofday(&tv1, NULL);
	printf("Random read started...\n");
	read_random(fp, page, offsets);
	gettimeofday(&tv2, NULL);

	printf ("Random read total time = %f seconds\n",
	         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	         (double) (tv2.tv_sec - tv1.tv_sec));




    free(offsets);
    fclose(fp);

	return 0;
}
void fill_indexes(u_long arr[], u_long n)
{
	u_long i;
	for (i = 0; i < n; i++)
		arr[i] = i;
}
void read_sequentially(FILE* fp, char page[], u_long offsets[])
{
	int i = 0;
	fpos_t pos;

	fgetpos(fp, &pos);
	while (i < NUM_OF_PAGES) {
		
		fgetpos(fp, &pos);
	//	printf("%ld\n", pos.__pos);
		fread(page, PAGE_SIZE, 1, fp);
		i++;
	}
}
void read_random(FILE* fp, char page[], u_long offsets[])
{
	u_long i;
	fpos_t pos;

	rewind(fp);

	for (i = 0; i < NUM_OF_PAGES; i++) {
		fseek(fp, offsets[i] * PAGE_SIZE, SEEK_SET);
		fgetpos(fp, &pos);
		//printf("fp: %ld\n", pos.__pos);
		//printf("offset: %ld\n", offsets[i] * PAGE_SIZE);
		fread(page, PAGE_SIZE, 1, fp);
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

	for (long i = 0; i < FILE_SIZE; i += PAGE_SIZE) {
		fwrite(BLANK_PAGE, PAGE_SIZE, 1, fp);
	}

	fclose(fp);
}