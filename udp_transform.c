#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>



#include <unistd.h>
// 100000000
#define FILE_SIZE 100000
#define BUFFER_SIZE 1024
void print_time()
{

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "time %s", asctime (timeinfo) );

}
void send_file_data(FILE* fp, int sockfd, struct sockaddr_in addr)
{
    int n;
    char buffer[BUFFER_SIZE];

    // Sending the data
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        // printf("[SENDING] Data: %s", buffer);

        n = sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
        if (n == -1)
        {
            perror("[ERROR] sending data to the server.");
            exit(1);
        }
        bzero(buffer, BUFFER_SIZE);
    }

    // Sending the 'END'
    strcpy(buffer, "END");
    sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));

    fclose(fp);
}

void write_file(int sockfd, struct sockaddr_in addr)
{

    char* filename = "server.txt";
    int n;
    char buffer[BUFFER_SIZE];
    socklen_t addr_size;

    // Creating a file.
    FILE* fp = fopen(filename, "w");

    // Receiving the data and writing it into the file.
    while (1)
    {
        addr_size = sizeof(addr);
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addr_size);

        if (strcmp(buffer, "END") == 0)
        {
            break;
        }

        // printf("[RECEVING] Data: %s", buffer);
        fprintf(fp, "%s", buffer);
        fputc(n,fp);
        bzero(buffer, BUFFER_SIZE);
    }

    fclose(fp);
}
void* server_code() {
    // Defining the IP and Port
    char* ip = "127.0.0.1";
    const int port = 8080;

    // Defining variables
    int server_sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
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

    //printf("[STARTING] UDP File Server started. \n");
    write_file(server_sockfd, client_addr);

    //printf("[SUCCESS] Data transfer complete.\n");
    //printf("[CLOSING] Closing the server.\n");

    close(server_sockfd);
}
void  run_client()
{

    sleep(1.5);
    // Defining the IP and Port
    char *ip = "127.0.0.1";
    const int port = 8080;
    int check_error;

    // Defining variables
    int sockfd;
    struct sockaddr_in server_addr;
    char *filename = "client.txt";
    FILE *fp;
    // FILE *fp = fopen(filename, "r");

    // Creating a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("[ERROR] socket error");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    check_error = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (check_error == -1) {
        perror("[-]Error in socket");
        exit(1);
    }

    // Reading the text file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("[ERROR] reading the file");
        exit(1);
    }

    // Sending the file data to the server
    send_file_data(fp, sockfd, server_addr);

    printf("[SUCCESS] Data transfer complete.\n");
    //printf("[CLOSING] Disconnecting from the server.\n");

    close(sockfd);

}





/**
 * Function to generate "client.txt" file.
 * this file size is 100MB and full of random bits.
 */
void generate_random_file() {
    FILE *fp = fopen("client.txt", "wb");
    if (fp == NULL) {
        perror("Error Opening the file");
        exit(1);
    }
    //100MB of randomly generated file
    long random_file_size = FILE_SIZE;
    //Generate file
    for (int i = 0; i < random_file_size; ++i) {
        int num = (rand() % 2);
        fputc(num + 48, fp);
    }
}


/**
 * Check Sum Function.
 * Input-> String of file location.
 * Output-> checksum of file.
 * @param file_location
 * @return
 */
unsigned int checksum(char *file_location) {
    FILE *fp = fopen(file_location, "rb");
    unsigned char checksum = 0;
    while (!feof(fp) && !ferror(fp)) {
        checksum ^= fgetc(fp);
    }
    fclose(fp);
    return checksum;
}

void compare_files_by_chars()
{
    FILE* fp1= fopen("server.txt","rb");
    FILE* fp2= fopen("client.txt","rb");

    // fetching character of two file
    // in two variable ch1 and ch2
    char ch1 = getc(fp1);
    char ch2 = getc(fp2);

    // error keeps track of number of errors
    // pos keeps track of position of errors
    // line keeps track of error line
    int error = 0, pos = 0, line = 1;

    // iterate loop till end of file
    while (ch1 != EOF || ch2 != EOF)
    {
        pos++;

        // if both variable encounters new
        // line then line variable is incremented
        // and pos variable is set to 0
        if (ch1 == '\n' && ch2 == '\n')
        {
            line++;
            pos = 0;
        }

        // if fetched data is not equal then
        // error is incremented
        if (ch1 != ch2)
        {
            error++;
        }

        // fetching character until end of file
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }

    if (error == 0 && ch1 == EOF && ch2 == EOF)
    {
        printf("Identical by chars\n");
    }
    else
    {
        printf("Not Identical by chars\n");
    }
}
/**
 * Function to compare between our generated file, and transformed file.
 */
void compare_files_by_checksum() {
    unsigned int client_checksum = checksum("client.txt");
    unsigned int server_checksum = checksum("server.txt");

    if (client_checksum == server_checksum) {
        printf("Files are identical by checksum\n");

    } else {
        printf("Files not transform successfully - not identical by checksum\n");
        printf("Client Checksum file: %u\n", client_checksum);
        printf("Server Checksum file: %u\n", server_checksum);
    }
}


int main() {
    printf("\n|UDP/IPv6 Socket| Starting ");
    print_time();
    //Generate random 100MB of bits into file.
    generate_random_file();
    //Open new thread.
    pthread_t ptid;
    // Creating a new thread and run client function from him.
    pthread_create(&ptid, NULL, &server_code, NULL);
    //run server after, the client should connect from other thread
    sleep(1);
    run_client();
    // Compare the two threads created
//    if(pthread_equal(ptid, pthread_self()))
//        printf("Threads are equal\n");
//    else
//        printf("Threads are not equal\n");

    // Waiting for the created thread to terminate
    pthread_join(ptid, NULL);

    //After transforming the file from client to server -> we check if they are equal using our checksum function

    compare_files_by_chars();
    compare_files_by_checksum();
    printf("|UDP/IPv6 Socket| Ending ");
    print_time();
    pthread_exit(NULL);
}