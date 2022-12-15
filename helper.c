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
# include <time.h>

// 100000000
#define FILE_SIZE 100000000
#define BUFFER_SIZE 128



char* print_time() 
{

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("time %s", asctime(timeinfo));
    return asctime(timeinfo);
}


/**
 * Function to generate "bigfile.txt" file.
 * this file size is 100MB and full of random bits.
 */
void generate_random_file()
{
    // yes this is a test file abcdefghijklmnopqrstuvwxyz | head -c 100MB > bigfile.txt
    // found this command in: https://www.computernetworkingnotes.com/linux-tutorials/generate-or-create-a-large-text-file-in-linux.html
    char str[512] = "yes a1b2c3d4e5 | head -c 100MB > bigfile.txt";
    if (system(str) != 0)
    {
        printf("ERROR creating file!\n");;
    }
}


long calculateCheckSum(char *data) 
{
    long sum = 0;
    int length = strlen(data);
    for (int i = 0; i < length; i++)
    {
        if (data[i] == '\n' || data[i] == '\t' || data[i] == ' ' || data[i] == 32 || data[i] == 9 || data[i] == 11 || data[i] == 10 || data[i] == 12 || data[i] == 8)
            continue;
        sum += data[i];
    }
    return sum;
}


long calculateTotalCheckSum(char * file)
{    
    int first = open(file, O_RDONLY);
    if(first == -1)
    {
        perror("error opening first file: ");
        return -1;
    }
    char data[1000];
    int bytesRead;
    long sum = 0; 
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
// //     // generate_random_file();
// //     // printf("%d\n", calculateTotalCheckSum("bigfile.txt"));
// //     // printf("%d\n", calculateTotalCheckSum("bigfile.txt"));
//     // char* start = print_time();
//     long start = ReturnTimeNs();
//     sleep(10);
//     long end = ReturnTimeNs();
//     printf("%f\n", (double)((end - start) / 1000000000));
//     // char* end = print_time();

// }













