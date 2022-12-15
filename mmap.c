#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include "helper.c"



int main() {
    long before = 0;
    long after = 0;
    clock_t curr_time;

    generate_random_file();
    before = calculateTotalCheckSum("bigfile.txt");
    // printf("\nRandom file have been created. \n1st CHECKSUM = %ld\n", before);
    printf("\n________________ START MMAP ____________________\n");
    printf("Starting ");
    print_time();
    curr_time = clock();

    // Open the text file for reading
    FILE* file = fopen("bigfile.txt", "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }
    // printf("File opened\n");

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
        after = calculateTotalCheckSum("server.txt");

        // printf("[RECEIVED] File received to server. \n2nd CHECKSUM = %ld\n", after);
        // printf("[SUCCESS] Data transfer complete.\n");
        // printf("Difference between checksum is -> %ld\n", (long)(abs(after - before)));
        printf("Ending ");
        print_time();
        if (after == before)
        {
        curr_time = clock() - curr_time;
        double time_taken = ((double)curr_time) / CLOCKS_PER_SEC;
        printf("MMAP time -> %f SEC\n", time_taken);
        }
        else
        {
            printf("The checksums are not the same -1\n");
            printf("before-> %ld, after-> %ld -- DIFF-> %ld\n", before, after, (long)(after - before));
        }
        printf("________________  END MMAP ____________________\n\n");
        close(fd[0]); // close the read end of the pipe
        exit(0);
    }
}