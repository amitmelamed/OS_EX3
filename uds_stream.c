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

void send_file(FILE *fp, int sockfd){
    int n;
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, SIZE);
    }
}
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
    for (int i = 0; i < 1000000; i++) {
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
 * Function to run client from the new procces
 */
void client_run()
{
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;

    generate_random_file();
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp;
    char *filename = "client.txt";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Connected to Server.\n");

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("[-]Error in reading file.");
        exit(1);
    }

    send_file(fp, sockfd);
    printf("[+]File data sent successfully.\n");

    printf("[+]Closing the connection.\n");
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

    fp = fopen(filename, "w");
    while (1) {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0){
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    printf("[GENERATE]  File have been recived for server. CHECKSUM %d", calculateTotalCheckSum("client.txt"));

    return;
}

/**
 * Define the server , then open new procces with client.
 * @return
 */
int main()
{
    print_time();
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("[-]Error in socket");
        exit(1);
    }
    printf("[+]Server socket created successfully.\n");


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);





    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e < 0) {
        perror("[-]Error in bind");
        exit(1);
    }

    printf("[+]Binding successfull.\n");

    if(listen(sockfd, 10) == 0){
        printf("[+]Listening....\n");
    }else{
        perror("[-]Error in listening");
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









    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    write_file(new_sock);
    printf("[+]Data written in the file successfully.\n");
    print_time();


    return 0;
}