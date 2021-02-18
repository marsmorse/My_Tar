#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFERSIZE 512
/*
    this program takes the first byte from a cmp -l and prints data from that byte from both input .tar files
*/
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("incorrect number of arguments\n"); 
        return 0;
    }
    int fd;
    char cmpOutput[200];
    strcpy(cmpOutput, argv[1]);
    char my_tar[50];
    strcpy(my_tar, argv[2]);
    char tar[50];
    strcpy(tar, argv[3]);

    if ((fd = open(cmpOutput, O_RDONLY)) == -1)
    {
        printf("couldn't open fileby name %s\n", cmpOutput); 
        return 0;
    }
    char buf[BUFFERSIZE];
    if (read(fd, buf, BUFFERSIZE) == -1)
    {
        printf("read for %s failed\n", cmpOutput); 
        return 0;
    }
    char* tok = strtok(buf, " ");
    size_t byte_number = atoi(tok);
    printf("byte_number = %lu\n", byte_number);
    
    int tarfd;
    int mytarfd;
    if ((mytarfd = open(my_tar, O_RDONLY)) == -1 || (tarfd = open(tar, O_RDONLY)) == -1)
    {
        printf("couldn't open fileby name %s\n", my_tar); 
        return 0;
    }
    lseek(mytarfd, byte_number-3, SEEK_SET);
    lseek(tarfd, byte_number-3, SEEK_SET);
    printf("\nmy_tar output: \n");
    read(mytarfd, buf, BUFFERSIZE);
    write(1, buf, BUFFERSIZE);

    printf("\ntar output: \n");
    read(tarfd, buf, BUFFERSIZE);
    write(1, buf, BUFFERSIZE);
    close(tarfd);
    close(mytarfd);
    close(fd);
    return 1;

}