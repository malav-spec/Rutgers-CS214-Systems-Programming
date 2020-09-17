#ifndef MANIFEST_H
#define MANIFEST_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"socket.h"

struct manifestNode{

char* filePath;
char* hash;
char* version;
struct manifestNode* next;
};

struct ManifestFileList{

struct manifestNode* head;
char* projectName;
char* versionNumber;
int numFiles;
};


char* getPath(char* filePath){

char* ch = strchr(filePath, '/');
ch++;
char* ch1 = strchr(ch, '/');
ch1++;
char* ch2 = strchr(ch1, '/');
return ch2;
}

static void addToManifestList(struct ManifestFileList* list, char* filePath, char* hash, char* version){

struct manifestNode* ptr = malloc(sizeof(struct manifestNode));

ptr->filePath = filePath;
ptr->hash = hash;
ptr->version = version;


struct manifestNode* node = list->head;

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

static void writeToManifestFileList(struct ManifestFileList* list, int fd){

write(fd, list->projectName, strlen(list->projectName));
write(fd, "\n", 1);
write(fd, list->versionNumber, strlen(list->versionNumber));
write(fd ,"\n", 1);
//list->numFiles += 1;
char num[8];
sprintf(num,"%d",list->numFiles);

write(fd, num, strlen(num));
write(fd ,"\n", 1);

struct manifestNode* ptr = list->head;

while(ptr!= NULL){

write(fd, ptr->version, strlen(ptr->version));
write(fd, " ", 1);
write(fd, ptr->filePath, strlen(ptr->filePath));
write(fd, " ", 1);
write(fd, ptr->hash, strlen(ptr->hash));
write(fd, "\n", 1);
ptr = ptr->next;
}

}


static struct ManifestFileList* readFromManifest(int fd){

struct ManifestFileList* list = malloc(sizeof(struct ManifestFileList));

list->head = NULL;
list->numFiles = 0;

struct socketList* name_list = createList();
struct socketList* f_list = createList();
struct socketList* v_list = createList();

tokenizeString(name_list, fd, '\n');
list->projectName = readList(name_list);

tokenizeString(v_list, fd, '\n');
list->versionNumber = readList(v_list);

tokenizeString(f_list, fd, '\n');
char* file = readList(f_list);

list->numFiles = atoi(file);

int i = 0;

while(i < list->numFiles){

struct socketList* list_v = createList();
struct socketList* list_n = createList();
struct socketList* list_h = createList();

char* file;
char* hash;
char* version;

tokenizeString(list_v, fd, ' ');
version = readList(list_v);

tokenizeString(list_n, fd, ' ');
file = readList(list_n);

tokenizeString(list_h, fd, '\n');
hash = readList(list_h);

addToManifestList(list, file, hash, version);
i++;
}


return list;

}

void removeFromManifestList(struct ManifestFileList* list, struct manifestNode* node){

if(node == NULL){

printf("Error: File not added to .Manifest\n");
exit(0);
}

struct manifestNode* ptr = list->head;
struct manifestNode* prev = NULL;

if(strcmp(ptr->filePath, node->filePath) == 0){
   
   if(ptr->next == NULL){
   
      list->head = NULL;
      list->numFiles -= 1;
      return;
   }
    
    ptr = ptr->next;   
   list->head = ptr;
   list->numFiles -= 1;
   return;
}
prev = ptr;
while(ptr != NULL){

  if(strcmp(ptr->filePath, node->filePath) == 0){
  
     if(ptr->next == NULL){
      prev->next = NULL;
      list->numFiles -= 1;
      return;
     }
     
     prev->next = ptr->next;
     ptr->next = NULL;
     list->numFiles -= 1;
     return;
  }
 prev = ptr;
 ptr = ptr->next;
}

}

 struct manifestNode* searchFileInManifest(struct ManifestFileList* list, char* file){

struct manifestNode* ptr = list->head;

while(ptr != NULL){
 
  if(strcmp(file, ptr->filePath) == 0){
   return ptr;
  }
 ptr = ptr->next;
}
return NULL;
}

struct manifestNode* searchFile(struct ManifestFileList* list, struct manifestNode* node){

struct manifestNode* ptr = list->head;

while(ptr != NULL){
 
  if(strcmp(node->filePath, ptr->filePath) == 0){
   return ptr;
  }
 ptr = ptr->next;
}
return NULL;
}


void updateServerManifest(struct ManifestFileList* mainList, struct ManifestFileList* newList){

struct manifestNode* mainNode = mainList->head;
struct manifestNode* newNode = newList->head;
struct manifestNode* ptr = newNode;

mainList->numFiles = newList->numFiles;
strcpy(mainList->versionNumber,newList->versionNumber);

if(newNode->next == NULL){
char* newPath = getPath(newNode->filePath);
sprintf(newNode->filePath, "./%s%s", mainList->projectName, newPath);
mainList->head = newNode;
return;
}
mainList->head = newNode;
mainNode = mainList->head;

while(mainNode != NULL){
char* newPath = getPath(mainNode->filePath);
sprintf(mainNode->filePath, "./%s%s", mainList->projectName, newPath);
mainNode = mainNode->next;
}
return;
}

void freeManifest(struct ManifestFileList* list){

struct manifestNode* ptr = list->head;

while(ptr != NULL){
//printf("freeList\n");
  struct manifestNode* temp = ptr;
  ptr = ptr->next;
  free(temp);
}


}


#endif
