#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
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
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "helper.c"
#define SIZE 64


/**
 * Send file file from client to server
 * @param fp
 * @param sockfd
 * @param addr
 */
void send_file_data(FILE* fp, int sockfd, struct sockaddr_in addr)
{
    int n;
    char buffer[SIZE];
    // Sending the data
    while (fgets(buffer, SIZE, fp) != NULL)
    {
        //printf("[SENDING] Data: %s", buffer);

        n = sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
        if (n == -1)
        {
            perror("[ERROR] sending data to the server.");
            exit(1);
        }
        bzero(buffer, SIZE);
    }

    // Sending the 'END'
    strcpy(buffer, "END");
    sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
    fclose(fp);
}



/**
 * Function to run client from the new procces
 */
void client_run()
{
    sleep(3);
    // Defining the IP and Port
    char *ip = "127.0.0.1";
    const int port = 8080;

    // Defining variables
    int server_sockfd;
    struct sockaddr_in server_addr;
    char *filename = "bigfile.txt";
    FILE *fp = fopen(filename, "r");

    // Creating a UDP socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0)
    {
        perror("[ERROR] socket error");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Reading the text file
    if (fp == NULL)
    {
        perror("[ERROR] reading the file");
        exit(1);
    }
    // Sending the file data to the server
    send_file_data(fp, server_sockfd, server_addr);
    close(server_sockfd);

    exit(0);
}



/**
 * Server get the file and write it to server.txt
 * @param sockfd
 * @param addr
 */
void write_file(int sockfd, struct sockaddr_in addr)
{
    int data = 0;
    char* filename = "server.txt";
    int n;
    char buffer[SIZE];
    socklen_t addr_size;

    // Creating a file.
    FILE* fp = fp = fopen(filename, "wb");
    // Receiving the data and writing it into the file.
    while (1)
    {
        addr_size = sizeof(addr);
        n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);

        data+=n;
        if (strcmp(buffer, "END") == 0)
            break;

        //printf("[RECEIVING] Data: %s", buffer);
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }

    fclose(fp);
}



/**
 * Define the server , then open new procces with client.
 * @return
 */
int main() {
    long before = 0;
    long after = 0;
    clock_t curr_time;

    generate_random_file();
    before = calculateTotalCheckSum("bigfile.txt");
    // printf("\nRandom file have been created. \n1st CHECKSUM = %ld\n", before);
    printf("\n________________ START UDP/IPv6 ___________________\n");
    printf("Starting ");
    print_time();
    curr_time = clock();

    // Defining the IP and Port
    char *ip = "127.0.0.1";
    const int port = 8080;

    // Defining variables
    int server_sockfd;
    struct sockaddr_in server_addr, client_addr;
    int e;

    // Creating a UDP socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sockfd < 0) {
        perror("[ERROR] socket error");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (e < 0) {
        perror("[ERROR] bind error");
        exit(1);
    }

    /**
     * Create new process and call run client from it
     */
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        /* This is the child process.  Execute the shell command. */
        client_run();

    } else if (pid < 0)
        /* The fork failed.  Report failure.  */
        printf("FORK FAILED");

    // printf("[STARTING] UDP File Server started. \n");
    write_file(server_sockfd, client_addr);
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
    printf("UDP/IPv6 time -> %f SEC\n", time_taken);
    }
    else
    {
        printf("The checksums are not the same -1\n");
        printf("before-> %ld, after-> %ld -- DIFF-> %ld\n", before, after, (long)(after - before));
    }
    printf("________________  END UDP/IPv6 ___________________\n\n");
    close(server_sockfd);
    return 0;
}