//
// Created by amitmelamed on 12/8/22.
//
#include <stdio.h>
#include <stdlib.h>


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
        int num=(rand()%126);
        fputc(num,fp);
    }
}