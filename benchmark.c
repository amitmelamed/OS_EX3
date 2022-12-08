//
// Created by amitmelamed on 12/8/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


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




int main(){
    //File to open

    FILE *fp= fopen("text.txt","wb");
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
    printf("%d", checksum("text.txt"));
}