//
// Created by amitmelamed on 12/8/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SIZE 1024

/**
 * Check Sum Function.
 * Input-> String of file location.
 * Output-> checksum of file.
 * @param file_location
 * @return
 */
unsigned int checksum(char * file_location)
{
    FILE * fp= fopen(file_location,"rb");
    unsigned char checksum = 0;
    while (!feof(fp) && !ferror(fp))
    {
        checksum ^= fgetc(fp);
    }
    fclose(fp);
    return checksum;
}
/**
 * Function to receive our file to socket
 * @param sockfd
 */
void write_file(int sockfd,char * filename){
    int n;
    FILE *fp;
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
    return;
}



/**
 * Function to send our file in socket.
 * @param fp
 * @param sockfd
 */
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

/**
 * Run client
 */
void* run_client()
{

    sleep(10);
    char *ip = "127.0.0.1";
    int port = 8080;
    int e;

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

}

/**
 * Run server
 */
void run_server()
{
    //server
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

    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    write_file(new_sock,"server.txt");
    printf("[+]Data written in the file successfully.\n");


}

/**
 * Function to compare between our generated file, and transformed file.
 */
void compare_files(){
    unsigned int client_checksum=checksum("client.txt");
    unsigned int server_checksum=checksum("server.txt");

    if(client_checksum== server_checksum)
    {
        printf("Files transform successfully");
    }
    else
    {
        printf("Files not transform successfully\n");
        printf("Client Checksum file: %u\n",client_checksum);
        printf("Server Checksum file: %u\n",server_checksum);

    }
}


/**
 * Our main algorithm:
 * 1.Generate random 100MB of bits into file.
 * 2.Open new thread.
 * 3.Create client from new thread.
 * 4.Create Server from main thread.
 * 5.Transform generated 100MB file from client to server.
 * 6.Terminate our created new thread.
 * 7.Compare original file and transformed file using checksum.
 * @return
 */
int main(){
    //File to open
    FILE *fp= fopen("client.txt","wb");
    if(fp==NULL)
    {
        perror("Error Opening the file");
        exit(1);
    }
    //100MB of randomly generated file
    long random_file_size = 100000000;
    //Generate file
    for (int i = 0; i < random_file_size; ++i) {
        int num=(rand()%2);
        fputc(num+48,fp);
    }

    pthread_t ptid;
    // Creating a new thread and run client function from him.
    pthread_create(&ptid, NULL, &run_client, NULL);

    //run server after, the client should connect from other thread
    run_server();
    // Compare the two threads created
    if(pthread_equal(ptid, pthread_self()))
        printf("Threads are equal\n");
            else
        printf("Threads are not equal\n");

    // Waiting for the created thread to terminate
    pthread_join(ptid, NULL);

    printf("This line will be printed"
           " after thread ends\n");

    //After transforming the file from client to server -> we check if they are equal using our checksum function
    compare_files();
    pthread_exit(NULL);
}