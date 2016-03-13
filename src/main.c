#include <stdio.h>

#include "estimator.h"


int main(int argc, char **argv)
{
    char *SEQ_FILE_NAME = "file1024_seq";
    char *RANDOM_FILE_NAME = "file1024_random";
    long NUM_PAGES = 100000L;

    Estimator *seq_estimator = create_estimator(sequential_strategy, SEQ_FILE_NAME, NUM_PAGES);

    printf("Sequential read started...\n");
    run_test(seq_estimator);
    double seq_time = get_result(seq_estimator);
    printf("Sequential read total time = %f seconds\n", seq_time);


    Estimator *random_estimator = create_estimator(random_strategy, RANDOM_FILE_NAME, NUM_PAGES);
    run_test(random_estimator);
    double random_time = get_result(random_estimator);

    printf("Random read total time = %f seconds\n", random_time);

    printf("Random time is %f slower than Sequential time:\n", random_time / seq_time);

    free(seq_estimator);
    free(random_estimator);

    return 0;
}