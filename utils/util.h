#ifndef UTIL_H
#define UTIL_H

#define PAGE_SIZE 512
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

void fill_indexes(long long arr[], long long n);

void read_sequentially(int fd, char page[], long long offsets[]);

void read_random(int fd, char page[], long long offsets[]);

void shuffle(long long *array, long long n);

void shuffle_page(char *array, int n);

char *preallocate_test_file(char filename[], long long);

void handle(const char *string, int error);

#endif