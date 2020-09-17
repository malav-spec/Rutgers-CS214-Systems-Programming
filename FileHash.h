#ifndef FILEHASH_H
#define FILEHASH_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<openssl/md5.h>
#include<fcntl.h>

#define HASH_LEN 2*MD5_DIGEST_LENGTH

void getFileHash(char* filePath, unsigned char hash[HASH_LEN]){


int fd = open(filePath, O_RDONLY, 00600);

if(fd == -1){
 printf("Error: Cannot open file for hash computation\n");
 exit(0);
}

MD5_CTX mdContext;
ssize_t bytes;

unsigned char data[1024];

MD5_Init(&mdContext);

while((bytes = read(fd,data, 1024)) != 0){

MD5_Update(&mdContext, data, bytes);

}

unsigned char temp[MD5_DIGEST_LENGTH];

MD5_Final(temp, &mdContext);

int i;

for(i=0; i<MD5_DIGEST_LENGTH; i++){

sprintf(hash+(2*i), "%02x", temp[i]);

}
close(fd);

}

#endif
