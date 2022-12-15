#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

// 100000000
#define FILE_SIZE 100000000
#define BUFFER_SIZE 1024



void print_time() 
{

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("time %s", asctime(timeinfo));

}

/**
 * Function to generate "client.txt" file.
 * this file size is 100MB and full of random bits.
 */
void generate_random_file()
{
    // yes this is a test file abcdefghijklmnopqrstuvwxyz | head -c 100MB > bigfile.txt
    // found this command in: https://www.computernetworkingnotes.com/linux-tutorials/generate-or-create-a-large-text-file-in-linux.html
    char str[512] = "yes this is a test file abcdefghijklmnopqrstuvwxyz | head -c 1MB > bigfile.txt";
    if (system(str) != 0)
    {
        printf("ERROR creating file!\n");;
    }
    // FILE *fp = fopen("client.txt", "wb");
    // if (fp == NULL) {
    //     perror("Error Opening the file");
    //     exit(1);
    // }
    // //100MB of randomly generated file
    // // long random_file_size = FILE_SIZE;
    // //Generate file
    // for (int i = 0; i < FILE_SIZE; ++i) {
    //     int num = (rand() % 2);
    //     fputc(num + 48, fp);
    // }
}


int calculateCheckSum(char *data) 
{
    int sum = 0;
    int length = strlen(data);
    for (int i = 0; i < length; i++)
        sum += data[i];
    int checksum = sum;    //1's complement of sum
    return checksum;
}

int calculateTotalCheckSum(char * file)
{    
    int first = open(file, O_RDONLY);
    if(first == -1)
    {
        perror("error opening first file: ");
        return -1;
    }
    char data[1000];
    int bytesRead, sum = 0; 
    while(1)
    {
        bzero(data, 1000);
        //reading the data and checking validity
        bytesRead = read(first,data,1000);
        if(bytesRead < 0)
        {
            close(first);
            perror("Error while reading: ");
            break;
        }
        else if(bytesRead == 0)
        {
            break;
        }
        sum += calculateCheckSum(data);
    }
    close(first);
    return sum;
}

// int main()
// {
//     // generate_random_file();
//     // printf("%d\n", calculateTotalCheckSum("bigfile.txt"));
//     // printf("%d\n", calculateTotalCheckSum("bigfile.txt"));

// }













