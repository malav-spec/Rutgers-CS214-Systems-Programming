#ifndef COMMIT_H
#define COMMIT_H

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include"socket.h"

struct commitNode{
char cmd;
char* version;
char* filePath;
char* hash;
struct commitNode* next;
};

struct commitFileList{
struct commitNode* head;
int numCmd;
};

void addToCommitList(struct commitFileList* list, char cmd, char* filePath, char* hash, char* version){

struct commitNode* ptr = malloc(sizeof(struct commitNode));

ptr->cmd = cmd;
ptr->filePath = filePath;
ptr->hash = hash;
ptr->version = version;

struct commitNode* node = list->head;

if(node == NULL){

node = ptr;
node->next = NULL;
list->head = node;
return;
}

while(node->next != NULL){
node = node->next;
}

node->next = ptr;
ptr->next = NULL;
} 

struct commitFileList* readFromCommit(int fd, int lines){
int len = 0;
char ch;

struct commitFileList* Commitlist = malloc(sizeof(struct commitFileList));
Commitlist->head = NULL;

while(len < lines){

read(fd, &ch, 1);
struct socketList* list_n = createList();
struct socketList* list_v = createList();
struct socketList* list_h = createList();

char cmd;
char* version;
char* file;
char* hash;

cmd = ch;
read(fd, &ch, 1);

tokenizeString(list_v, fd, ' ');
version = readList(list_v);

tokenizeString(list_n, fd, ' ');
file = readList(list_n);

tokenizeString(list_h, fd, '\n');
hash = readList(list_h);
len++;
addToCommitList(Commitlist, cmd, file, hash, version);
}
return Commitlist;
}

struct commitNode* searchCommit(struct commitFileList* list, struct commitNode* node){

struct commitNode* ptr = list->head;

while(ptr != NULL){

 if(strcmp(node->filePath, ptr->filePath) == 0){
   if((node->cmd == ptr->cmd) && (strcmp(node->hash, ptr->hash)==0)){
     return ptr;
   }
   return NULL;
 }
 ptr = ptr->next;
}
return NULL;
}

int compareCommit(struct commitFileList* clientCommit, struct commitFileList* serverCommit){

struct commitNode* clientNode = clientCommit->head;
struct commitNode* serverNode = serverCommit->head;

while(clientNode != NULL){

serverNode = searchCommit(serverCommit, clientNode);

if(serverNode == NULL){
return 0;
}

clientNode = clientNode->next;
}
return 1;
}


#endif
