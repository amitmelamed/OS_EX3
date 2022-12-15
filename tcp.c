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

void send_file(FILE *fp, int sockfd){
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, SIZE);
    }
}


/**
 * Function to run client from the new procces
 */
void client_run()
{
    sleep(3);
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;
    char *filename = "bigfile.txt";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    // printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("[-]Error in socket");
        exit(1);
    }
    // printf("[+]Connected to Server.\n");

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }

    send_file(fp, sockfd);
    // printf("[+]File data sent successfully.\n");
    // printf("[+]Closing the connection.\n");
    close(sockfd);
    exit(0);
}



/**
 * Server get the file and write it to server.txt
 * @param sockfd
 * @param addr
 */
void write_file(int sockfd){
    int n;
    FILE *fp;
    char *filename = "server.txt";
    char buffer[SIZE];

    fp = fopen(filename, "wb");
    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0){
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
}



/**
 * Define the server , then open new procces with client.
 * @return
 */
int main()
{
    long before = 0;
    long after = 0;
    clock_t curr_time;

    generate_random_file();
    before = calculateTotalCheckSum("bigfile.txt");
    // printf("\nRandom file have been created. \n1st CHECKSUM = %ld\n", before);
    printf("\n________________ START TCP/IPv4 ___________________\n");
    printf("Starting ");
    print_time();
    curr_time = clock();

    // Defining the IP and Port
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    // printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e < 0) {
        perror("[-]Error in bind");
        exit(1);
    }

    // printf("[+]Binding successfull.\n");

    if(listen(sockfd, 10) != 0){
        perror("[-]Error in listening");
        exit(1);
    }

    /**
     * Create new process and call run client from it
     */
    pid_t pid;

    pid = fork ();
    if (pid == 0)
    {
        /* This is the child process.  Execute the shell command. */
        client_run();

    }
    else if (pid < 0)
        /* The fork failed.  Report failure.  */
        printf("FORK FAILED");

    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    write_file(new_sock);
    after = calculateTotalCheckSum("bigfile.txt");

    // printf("[RECEIVED] File received to server. \n2nd CHECKSUM = %ld\n", after);
    // printf("[SUCCESS] Data transfer complete.\n");
    // printf("Difference between checksum is -> %ld\n", (long)(abs(after - before)));
    printf("Ending ");
    print_time();
    if (after == before)
    {
    curr_time = clock() - curr_time;
    double time_taken = ((double)curr_time) / CLOCKS_PER_SEC;
    printf("TCP/IPv4 time -> %f SEC\n", time_taken);
    }
    else
    {
        printf("The checksums are not the same -1\n");
        printf("before-> %ld, after-> %ld -- DIFF-> %ld\n", before, after, (long)(after - before)); 
    }
    printf("________________ END TCP/IPv4 ___________________\n\n");
    return 0;
}