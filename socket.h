#ifndef SOCK_H
#define SOCK_H

#include<stdio.h>
#include<stdlib.h>

struct socketNode{
char  chr;
struct socketNode* next;
};

struct socketList{
struct socketNode* head;
int size;
};


static struct socketList* createList(){

struct socketList* list = malloc(sizeof(struct socketList));
list->head = NULL;
list->size = 0;
return list;

}

static void addToList(struct socketList* list, char chr){

struct socketNode* ptr = malloc(sizeof(struct socketNode));
struct socketNode* node = list->head;
ptr->chr = chr;

if(list->head == NULL){
 
 list->head = ptr;
 ptr->next = NULL;
 list->size++;
 return;
}
while(node->next != NULL){
node = node->next;
}
node->next = ptr;
ptr->next = NULL;
list->size++;
}

static void tokenizeString(struct socketList* list, int fd, char delim){

char chr;
//printf("tokenizeString\n");
while(1){

read(fd,&chr,1);

if(chr == '\0'){
break;
}
if(chr == delim){
break;
}

addToList(list,chr);
}

}

void freeList(struct socketList* list){

struct socketNode* ptr = list->head;

while(ptr != NULL){
//printf("freeList\n");
  struct socketNode* temp = ptr;
  ptr = ptr->next;
  free(temp);
}

}

static char* readList(struct socketList* list){
//printf("readList\n");
struct socketNode* ptr = list->head;
char* string = malloc(sizeof(char)*(list->size+1));

int i = 0;

while(ptr!=NULL){
string[i] = ptr->chr;
ptr = ptr->next;
i++;
}

string[i] = '\0';
//freeList(list);
return string;
}


#endif
