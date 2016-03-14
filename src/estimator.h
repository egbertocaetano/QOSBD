#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include "../utils/util.h"

typedef struct _estimator Estimator;

typedef void (*Strategy)(int , long long, long long []);

struct _estimator {
    Strategy strategy;
    double elapsed_time;
    char *file_name;
    long num_of_pages;
};

Estimator *create_estimator(Strategy , char *, long );

void run_test(Estimator *);

double get_result(Estimator *);

void sequential_strategy(int fd, long long num_of_pages, long long offsets[]);

void random_strategy(int fd, long long num_of_pages, long long offsets[]);

#endif
