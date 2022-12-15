#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024



void print_time()
{

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("time %s", asctime(timeinfo));

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


void generate_random_file() {
    FILE *fp = fopen("client.txt", "wb");
    if (fp == NULL) {
        perror("Error Opening the file");
        exit(1);
    }
    //100MB of randomly generated file
    long random_file_size = 1024*1000;
    //Generate file
    for (int i = 0; i < random_file_size; ++i) {
        int num = (rand() % 2);
        fputc(num + 48, fp);
    }
    printf("Random File Generated\nChecksum of client.txt: %d\n",calculateTotalCheckSum("client.txt"));
}


int main() {
    printf("Shared memory\n");
// Check for correct number of arguments
    print_time();
    generate_random_file();
// Open the text file for reading
    FILE* file = fopen("client.txt", "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }
    printf("File opened\n");

// Create a pipe to send the file through
    int fd[2];
    if (pipe(fd) < 0) {
        perror("Error");
        exit(1);
    }

// Fork a child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error forking process");
        exit(1);
    }


// In the child process, read the text file and write it to the pipe
    if (pid == 0) {
        close(fd[0]); // close the read end of the pipe
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, file)) {
            if (write(fd[1], buffer, BUFFER_SIZE) < 0) {
                perror("Error ");
                exit(1);
            }
        }
        close(fd[1]); // close the write end of the pipe
        exit(0);
    }

// In the parent process, read from the pipe and print the text file
    else {
        close(fd[1]); // close the write end of the pipe

        FILE *fp = fopen("server.txt","wb");

        // Return if could not open file
        if (fp == NULL)
            return 0;

        char buffer[BUFFER_SIZE];
        while (read(fd[0], buffer, BUFFER_SIZE) > 0) {
            fputs(buffer, fp);
        }
        fclose(fp);
        printf("File transformed\n Checksum of server.txt: %d\n",calculateTotalCheckSum("server.txt"));
        print_time();

        close(fd[0]); // close the read end of the pipe
        exit(0);
    }
}