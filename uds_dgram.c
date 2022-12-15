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
#define SIZE 1024


int calculateTotalCheckSum(char * file);
int calculateCheckSum(char *data);

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

    // Seed the random number generator with the current time
    srand(time(NULL));

    // Open the file for writing
    FILE* file = fopen("client.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Write random 1s and 0s to the file
    for (int i = 0; i < 10; i++) {
        int num = rand() % 2;
        fprintf(file, "%d", num);
    }

    // Close the file
    fclose(file);
    printf("[GENERATE] Random file have been creasted. CHECKSUM %d", calculateTotalCheckSum("client.txt"));
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



int calculateTotalCheckSum_2(char * file)
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
    sleep(2);
    // Defining the IP and Port
    char *ip = "127.0.0.1";
    const int port = 8080;

    // Defining variables
    int server_sockfd;
    struct sockaddr_in server_addr;
    generate_random_file();
    char *filename = "client.txt";
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

    printf("[SUCCESS] Data transfer complete.\n");
    printf("[CLOSING] Disconnecting from the server.\n");

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

    int data=0;
    char* filename = "server.txt";
    int n;
    char buffer[SIZE];
    socklen_t addr_size;

    // Creating a file.
    FILE* fp = fp = fopen(filename, "w");

    // Receiving the data and writing it into the file.
    while (1)
    {
        addr_size = sizeof(addr);
        n = recvfrom(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, &addr_size);

        data+=n;

        if (strcmp(buffer, "END") == 0)
        {
            break;
        }

        //printf("[RECEIVING] Data: %s", buffer);
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }




    printf("[RECEIVED] File received to server. Checksum: %d", calculateTotalCheckSum_2("client.txt"));


    fclose(fp);
}

/**
 * Define the server , then open new procces with client.
 * @return
 */
int main()
{



    print_time();
    // Defining the IP and Port
    char* ip = "127.0.0.1";
    const int port = 8080;

    // Defining variables
    int server_sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[SIZE];
    int e;





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




    e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0)
    {
        perror("[ERROR] bind error");
        exit(1);
    }



    /**
     * Create new process and call run client from it
     */
    int status;
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
    status = -1;



    printf("[STARTING] UDP File Server started. \n");
    write_file(server_sockfd, client_addr);







    printf("[SUCCESS] Data transfer complete.\n");
    printf("[CLOSING] Closing the server.\n");
    print_time();

    close(server_sockfd);

    return 0;
}