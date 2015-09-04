#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>	
 
#define ARR_MAX 14

ssize_t write_fully(int fd, unsigned char *buf, size_t nb);
ssize_t read_fully(int fd, unsigned char *buf, size_t nb);

int write_completely(int fd, void *buf, size_t nb);
int read_completely(int fd, void *buf, size_t nb);

int main() 
{
	double arr[ARR_MAX];
	double res_arr[ARR_MAX];
    int narr = ARR_MAX;
    int i;
    size_t tam_nb;

    int fdOut;
    int fdIn;
    char const *index_file_name = "my-index";

    tam_nb = narr*sizeof arr[0];
    printf("Size : %ld\n",  tam_nb);

    for(i = 0; i < ARR_MAX; i++)
    {    
        arr[i] = i + 1.0;
        //printf("%f\n", arr[i]);
    }
    
    
    if(open(index_file_name, O_RDWR, 0) < 0) 
    {
        printf("Creating my-index.bin\n");
        int fd;
        fd = creat(index_file_name, 0744);
    }

    fdOut = open(index_file_name, O_RDWR, 0);
    if (write_completely(fdOut, &arr[0], narr * (sizeof(arr[0]))) < 0) 
    {
    	abort();
    	exit(EXIT_FAILURE);

    }else
    {
    	printf("Escrita feita com sucesso\n");
    }   
    
    if (close(fdOut) < 0) 
    {
        perror("closing a file");
        exit(EXIT_FAILURE);
    }
    
   fdIn = open(index_file_name, O_RDWR, 0); 
   if (read_completely(fdIn, &res_arr[0], narr * (sizeof(arr[0]))) < 0) 
    {
    	printf("Errrou\n");
    	abort();
    	exit(EXIT_FAILURE);

    }else
    {
    	for(i = 0; i < ARR_MAX; i++)
    		printf("pos[%d] = %f\n", i, res_arr[i]);
    }

    if (close(fdIn) < 0) 
    {
        perror("closing a file");
        exit(EXIT_FAILURE);
    }

    return 0;
}
    
ssize_t write_fully(int fd, unsigned char *buf, size_t nb) 
{
    
    ssize_t n, r;
        
    r = 0;
    while (nb != 0) {
        n = write(fd, buf, nb);
        if (n < 0)                  /* error */
            return n;
        if (n == 0)                 /* end of file */
            return r;
        nb -= n;                    /* 0 < n <= nb */
        buf += n;
    }

    return n;
 }

ssize_t read_fully(int fd, unsigned char *buf, size_t nb)
{
   
    ssize_t n, r;
    
    r = 0;
    while (nb != 0) {
        n = read(fd, buf, nb);
        if (n < 0)                      /* error */  
          return n;          
        if (n == 0)                     /* end of file */
          return r;       
        nb -= n;                        /* 0 < n <= nb */
        buf += n;
    }
    return r;
}


int read_completely(int fd, void *buf, size_t nb) 
{
    ssize_t n = read_fully(fd, buf, nb);
    if (n < 0) return -1;
    //if (n == nb) { return -1; }
    return 0;
}

int write_completely(int fd, void *buf, size_t nb) 
{
    ssize_t n = write_fully(fd, buf, nb);

    if (n < 0) return -1;
    if (n != nb) { return -1; }
    return 0;
}