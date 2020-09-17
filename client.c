#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <stdbool.h>

#include "socket.h"
#include "manifest.h"
#include "commit.h"
#include "FileHash.h"

 
#define SA struct sockaddr 


void createDirectory(char* path){

struct stat buffer;

 if(stat(path,&buffer) == -1){
   mkdir(path, 0777);
 }
 
}

int checkDirectory(char* path){

struct stat buffer;
stat(path,&buffer);

if(S_ISDIR(buffer.st_mode)){
return 1;
}
return 0;
}

bool checkFileExist(char* fileName){
struct stat buffer;
return (stat(fileName, &buffer) == 0);
}

int getLines(char* file){

int fd = open(file, O_RDONLY, 00600);

char chr;
int i = 0;

while(read(fd, &chr, 1) != 0){
 if(chr == '\n'){
  i++;
 }
}
close(fd);
return i;
}

long int getFileSize(char* file){

struct stat buffer;

if(stat(file, &buffer) == 0){
 return buffer.st_size;
}
return -1;
}

char* replaceRepository(char* filePath){

char* ch = strchr(filePath, '/');
ch++;
char* ch1 = strchr(ch, '/');
char* newPath = malloc(sizeof(char)*(strlen(filePath)));
sprintf(newPath, ".%s", ch1);
return newPath;
}

struct commitFileList* getClientCommit(char* project, int lines){

char* path = malloc(sizeof(char)*(strlen(project) + 20));

sprintf(path, "./%s/%s", project, ".Commit");

if(!checkFileExist(path)){
printf("Error: .Commit not found in client. First commit then push.\n");
exit(0);
}

int cfd = open(path, O_RDONLY, 00600);

if(cfd == -1){
printf("Error: Cannot open client .Commit\n");
exit(0);
}

struct commitFileList* list = readFromCommit(cfd, lines);

close(cfd);

return list;
}

struct commitFileList* getClientUpdate(char* project, int lines){

char* path = malloc(sizeof(char)*(strlen(project) + 20));

sprintf(path, "./%s/%s", project, ".Update");

int ufd = open(path, O_RDONLY, 00600);

if(ufd == -1){
printf("Error: Cannot open client .Update\n");
close(ufd);
exit(0);
}

struct commitFileList* list = readFromCommit(ufd, lines);

close(ufd);

return list;
}

void configure(char* IP, char* port,int fd){

if(fd == -1){
printf("Error: cannot open configure file\n");
exit(0);
}

write(fd,IP,strlen(IP));
write(fd," ", 1);
write(fd,port,strlen(port));

}

struct ManifestFileList* receiveServerManifest(int sfd, char* project){

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();

tokenizeString(list,sfd,':');

tokenizeString(list1,sfd,':');

tokenizeString(list2,sfd,':');

struct ManifestFileList* list3 = readFromManifest(sfd);
return list3;
}

struct ManifestFileList* receiveClientManifest(char* project){

char* path = malloc(sizeof(char)*(strlen(project) + 20));

sprintf(path, "./%s/%s", project, ".Manifest");

if(!checkFileExist(path)){
printf("Error: .Manifest not found\n");
exit(0);
}

int mfd = open(path, O_RDONLY, 00600);

if(mfd == -1){
printf("Error: Cannot open client .Manifest\n");
exit(0);
}

struct ManifestFileList* list = readFromManifest(mfd);

close(mfd);

return list;
}

void sendFileToServer(int sfd, char* path, char* projectName){

char* filePath = malloc(sizeof(char)*(strlen(projectName) + strlen(path) + 50));

int size = getFileSize(path);

sprintf(filePath, "%d:%s:%ld:", strlen(path), path, size);
write(sfd, filePath, strlen(filePath));

char chr;

int fd = open(path, O_RDONLY, 00600);
while(read(fd,&chr,1) != 0){
write(sfd, &chr, 1);
}

close(fd);
}

int checkFileOrDirectory(char* token){

if(strchr(token, '.') == NULL){
return 1; //Directory
}
return 2; //File
}

void createDirectoryIfNeeded(char* newPath, char* projectName){
 
char* token = strtok(newPath, "/");
char* path = malloc(sizeof(char)*(strlen(newPath)));
int c = 0;
sprintf(path, "./%s",projectName);

while(token != NULL){

if(c<=1){
c++;
token = strtok(NULL, "/");
continue;
}
 
 strcat(path,"/");
 strcat(path,token);
 
 int x = checkFileOrDirectory(token);
 
 if(x == 2){
 
   int fd = open(path, O_CREAT | O_RDWR, 00600);
   close(fd);
   
 }
 
  else if(x == 1){
  
   createDirectory(path);
  }
 c++;
 token = strtok(NULL, "/");
}

}

void receiveServerFile(int sfd, char* project, char* newPath){

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();


tokenizeString(list,sfd,':');
char* fileLen = readList(list);
int len = atoi(fileLen);

tokenizeString(list1,sfd,':');
char* filePath = readList(list1);
char* newFilePath = replaceRepository(filePath);

tokenizeString(list2,sfd,':');
char* fSize = readList(list2);
int fileSize = atoi(fSize);


int fd = open(newPath, O_RDWR | O_CREAT | O_TRUNC, 00600);

if(fd == -1){
printf("Error: Cannot open file.\n");
exit(0); 
}

char chr; 
int i = 0;
while(i < fileSize){

 read(sfd, &chr, 1);
 if(chr == '\0'){
 
  break;
 }
 write(fd, &chr, 1);
 i++;
}
close(fd);
}


void receiveFile(int sfd, char* project){


createDirectory(project);

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();


tokenizeString(list,sfd,':');
char* fileLen = readList(list);
int len = atoi(fileLen);

tokenizeString(list1,sfd,':');
char* filePath = readList(list1);
char* newFilePath = replaceRepository(filePath);

tokenizeString(list2,sfd,':');
char* fSize = readList(list2);
int fileSize = atoi(fSize);

char* path = malloc(sizeof(char)*(strlen(project) + 10));
sprintf(path, "./%s/%s", project, ".Manifest");

int fd = open(path, O_RDWR | O_CREAT, 00600);

char chr; 
int i = 0;
while(i < fileSize){

 read(sfd, &chr, 1);
 if(chr == '\0'){
 
  break;
 }
 write(fd, &chr, 1);
 i++;
}
close(fd);
}

void sendCommandToServer(int sfd,char* command, char* projectName){

int len = strlen(command);
int plen = strlen(projectName);
char* cmd = malloc(sizeof(char)*(len+plen+2));

if(checkDirectory(projectName)){
 printf("Error: Project exists on client side\n");
 exit(0);
}

sprintf(cmd,"%s:%s\n",command,projectName);
write(sfd,cmd,strlen(cmd));
free(cmd);

receiveFile(sfd,projectName);
printf("Project created. \n");
}

void destroy(int sfd, char* command, char* projectName){

int len = strlen(command);
int plen = strlen(projectName);
char* cmd = malloc(sizeof(char)*(len+plen+2));

sprintf(cmd,"%s:%s\n",command,projectName);
write(sfd,cmd,strlen(cmd));
free(cmd);

struct socketList* list = createList();
tokenizeString(list,sfd,'.');
char* msg = readList(list);

if(strcmp(msg, "success") == 0){
 printf("Project succesfully destroyed.\n");
 exit(0);
}
else{
 printf("Error: Destroy failed\n");
 exit(0);
}

}

void add(int sfd, char* command, char* projectName, char* fileName){

if(!checkDirectory(projectName)){
printf("Error: Project does not exist on the client side\n");
exit(0);
}

char* path = malloc(sizeof(char)*(strlen(projectName) + 50));

if(!checkFileExist(fileName)){
sprintf(path, "./%s/%s", projectName, fileName);

if(!checkFileExist(path)){
printf("Error: File does not exist\n");
return;
}

}
else{
strcpy(path, fileName);
}

char* man_path = malloc(sizeof(char)*(strlen(projectName) + 15));
sprintf(man_path, "./%s/%s", projectName, ".Manifest");

int mfd = open(man_path, O_RDWR, 00600);
struct ManifestFileList* list = readFromManifest(mfd);
struct manifestNode* node = searchFileInManifest(list, path);
close(mfd);

char buf[100];
getFileHash(path,buf);
buf[HASH_LEN] = '\0';
char* hash = strdup(buf);

char* version = "0";

if(node == NULL){

addToManifestList(list, path, hash, version);
list->numFiles += 1;
mfd = open(man_path, O_RDWR | O_TRUNC, 00600);
writeToManifestFileList(list, mfd);
close(mfd);

printf("File added to .Manifest\n");
}
else{
 printf("Error: File already in .Manifest\n");
 return;
}

}

void removeFile(int sfd, char* command, char* projectName, char* fileName){

if(!checkDirectory(projectName)){
 printf("Error: Project does not exist on the client side.\n");
 exit(0);
}

char* path = malloc(sizeof(char)*(strlen(projectName) + strlen(fileName) + 5));


if(!checkFileExist(fileName)){
sprintf(path, "./%s/%s", projectName, fileName);

if(!checkFileExist(path)){
printf("Error: File does not exist\n");
return;
}

}
else{
strcpy(path, fileName);
}

if(!checkFileExist(path)){
 printf("Error: File does not exist.\n");
 exit(0);
}

char* man_path = malloc(sizeof(char)*(strlen(projectName) + 15));

sprintf(man_path, "./%s/%s", projectName, ".Manifest");

int mfd = open(man_path, O_RDWR, 00600);
struct ManifestFileList* list = readFromManifest(mfd);
struct manifestNode* node = searchFileInManifest(list, path);
close(mfd);

removeFromManifestList(list, node);

mfd = open(man_path, O_RDWR | O_TRUNC, 00600);
writeToManifestFileList(list, mfd);
close(mfd);

unlink(path);

printf("File removed.\n");
}

void currentVersion(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + strlen(command) + 5));

sprintf(cmd, "%s:%s\n", command, projectName);

write(sfd, cmd, strlen(cmd));
free(cmd);


struct ManifestFileList* serverManifest = receiveServerManifest(sfd, projectName);

struct manifestNode* node = serverManifest->head;

while(node != NULL){

printf("%s %s\n", node->version, node->filePath);
node = node->next; 
}

}

void commit(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + 20));

char* conflict = malloc(sizeof(char)*(strlen(projectName) + 20));
char* update = malloc(sizeof(char)*(strlen(projectName) + 20));

sprintf(cmd, "%s:%s\n", command, projectName);

sprintf(conflict, "./%s/%s", projectName, ".Conflict");
sprintf(update, "./%s/%s", projectName, ".Update");

if(checkFileExist(conflict)){
 printf("Error: Found .Conflict file\n");
 return;
}

if(checkFileExist(update)){

int ufd = open(update, O_RDONLY, 00600);
char chr;
int i;
if(read(ufd, &chr, 1) != 0){
printf("Error: Found .Update file that isn't empty\n");
close(ufd);
 return;
}
 close(ufd);
}

write(sfd, cmd, strlen(cmd));

struct ManifestFileList* serverManifest = receiveServerManifest(sfd,projectName);
struct ManifestFileList* clientManifest = receiveClientManifest(projectName);

if(strcmp(serverManifest->versionNumber, clientManifest->versionNumber) != 0){
printf("Error: Version of .Manifests not same. Please update before commit\n"); 
exit(0);
}

struct manifestNode* serverNode = serverManifest->head;
struct manifestNode* clientNode = clientManifest->head;


char* path = malloc(sizeof(char)*(strlen(projectName)  + 20));
sprintf(path, "./%s/%s", projectName, ".Commit");

while(clientNode != NULL){

serverNode = searchFile(serverManifest, clientNode);

if(serverNode != NULL && strcmp(serverNode->hash, clientNode->hash) != 0){
printf("Error: Sync with server repository before commit.\n"); 
exit(0);
}

if(serverNode == NULL){

int cfd = open(path, O_RDWR | O_APPEND | O_CREAT, 00600);

int v = atoi(clientNode->version);
v+=1;

char* vr = malloc(sizeof(char)*(strlen(clientNode->version) + 1));

sprintf(vr, "%d", v);

write(cfd, "A", 1);
write(cfd, " ", 1);
write(cfd, vr, strlen(vr));
write(cfd, " ", 1); 
write(cfd, clientNode->filePath, strlen(clientNode->filePath));
write(cfd, " ", 1);
write(cfd, clientNode->hash, strlen(clientNode->hash));
write(cfd, "\n", 1);
printf("A %s\n", clientNode->filePath);
close(cfd);
clientNode = clientNode->next;
continue;
}

char buf[100];
getFileHash(clientNode->filePath,buf);
buf[HASH_LEN] = '\0';
char* liveHash = strdup(buf);

if(strcmp(clientNode->hash, liveHash) != 0){

int cfd = open(path, O_RDWR | O_APPEND | O_CREAT, 00600);

int v = atoi(clientNode->version);
v+=1;

char* vr = malloc(sizeof(char)*(strlen(clientNode->version) + 1));

sprintf(vr, "%d", v);

write(cfd, "M", 1);
write(cfd, " ", 1);
write(cfd, vr, strlen(vr));
write(cfd, " ", 1); 
write(cfd, clientNode->filePath, strlen(clientNode->filePath));
write(cfd, " ", 1);
write(cfd, liveHash, strlen(liveHash));
write(cfd, "\n", 1);
printf("M %s\n", clientNode->filePath);
close(cfd);
clientNode = clientNode->next;
continue;
}
clientNode = clientNode->next;
}

clientNode = clientManifest->head;
serverNode = serverManifest->head;

while(serverNode != NULL){

clientNode = searchFile(clientManifest, serverNode);

if(clientNode != NULL && strcmp(serverNode->hash, clientNode->hash) != 0){
printf("Error: Sync with server repository before commit.\n"); 
exit(0);
}

if(clientNode == NULL){

int cfd = open(path, O_RDWR | O_APPEND | O_CREAT, 00600);

int v = atoi(serverNode->version);
v+=1;

char* vr = malloc(sizeof(char)*(strlen(serverNode->version) + 1));

sprintf(vr, "%d", v);

write(cfd, "D", 1);
write(cfd, " ", 1);
write(cfd, vr, strlen(vr));
write(cfd, " ", 1); 
write(cfd, serverNode->filePath, strlen(serverNode->filePath));
write(cfd, " ", 1);
write(cfd, serverNode->hash, strlen(serverNode->hash));
write(cfd, "\n", 1);
printf("D %s\n", serverNode->filePath);
close(cfd);
}
serverNode = serverNode->next;
}

sendFileToServer(sfd, path, projectName);
int x;
read(sfd, &x, sizeof(int));

if(x == 1){
 printf(".Commit file succesfully sent.\n");
}
else{

 printf("Error: Server failed. \n");
 close(sfd);
 exit(0);
}

}

void push(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + 20));

char* commit = malloc(sizeof(char)*(strlen(projectName) + 20));


sprintf(cmd, "%s:%s\n", command, projectName);

sprintf(commit, "./%s/%s", projectName, ".Commit");


if(!checkFileExist(commit)){
 printf("Error: Not found .Conflict file. First commit then push\n");
 close(sfd);
 free(cmd);
 free(commit);
 exit(0);
}

write(sfd, cmd, strlen(cmd));

int cfd = open(commit, O_RDONLY, 00600);
if(cfd == -1){
printf("Error: Cannot open .commit file\n");
exit(0);
}
close(cfd);

int lines = getLines(commit);
write(sfd, &lines, sizeof(int));
sendFileToServer(sfd,commit,projectName);

struct commitFileList* clientCommit = getClientCommit(projectName, lines);

struct commitNode* clientNode = clientCommit->head;
struct commitNode* node = clientNode;

char a;

while(node != NULL){

if(node->cmd == 'D'){
node = node->next;
continue;
}

read(sfd,&a,1);

if(a == 'r'){
 sendFileToServer(sfd, node->filePath, projectName);
}
node = node->next;
}

struct ManifestFileList* list = receiveClientManifest(projectName);
char* version = malloc(sizeof(char)*2);
int v = atoi(list->versionNumber);
v+=1;
sprintf(version, "%d", v);
strcpy(list->versionNumber, version);
char* path = malloc(sizeof(char)*(strlen(projectName) + 20));
sprintf(path, "./%s/%s", projectName, ".Manifest");
int mfd = open(path, O_RDWR, 00600);
writeToManifestFileList(list, mfd);
close(mfd);
free(path);
free(version);

int msg;
read(sfd, &msg, sizeof(int));
 
if(msg == 1){
printf("Push successful\n");
unlink(commit);
}
else{
printf("Push unsuccessful\n");
unlink(commit);
}

}

void update(int sfd, char* command, char* projectName){

if(!checkDirectory(projectName)){
printf("Error: Project does not exist on the client side\n");
return;
}

char* cmd = malloc(sizeof(char)*(strlen(projectName) + 20));
sprintf(cmd, "%s:%s\n", command, projectName); 
write(sfd, cmd, strlen(cmd));

struct ManifestFileList* serverList = receiveServerManifest(sfd, projectName);

struct ManifestFileList* clientList = receiveClientManifest(projectName);

char* path = malloc(sizeof(char)*(strlen(projectName) + 20));
sprintf(path, "./%s/%s", projectName, ".Update");

if(strcmp(clientList->versionNumber, serverList->versionNumber) == 0){
int msg = 1;
write(sfd, &msg, sizeof(int));
printf("No update required.\n");
int ufd = open(path, O_CREAT | O_RDWR, 00600);
close(ufd);
return;
}
else{
int msg = 0;
write(sfd, &msg, sizeof(int));
}

struct manifestNode* ptr = serverList->head;
struct manifestNode* clientNode;

while(ptr != NULL){

clientNode = searchFile(clientList, ptr);

if(clientNode == NULL){

int fd = open(path, O_CREAT | O_RDWR | O_APPEND, 00600);

int v = atoi(ptr->version);
v+=1;

char* vr = malloc(sizeof(char)*(strlen(ptr->version) + 1));

sprintf(vr, "%d", v);

write(fd, "A", 1);
write(fd, " ", 1);
write(fd, vr, strlen(vr));
write(fd, " ", 1); 
write(fd, ptr->filePath, strlen(ptr->filePath));
write(fd, " ", 1);
write(fd, ptr->hash, strlen(ptr->hash));
write(fd, "\n", 1);
printf("A %s\n", ptr->filePath);

close(fd);
}

else if((strcmp(ptr->hash, clientNode->hash) != 0) && (strcmp(ptr->version, clientNode->version) != 0)){
  //compute live hash
  char buf[100];
getFileHash(clientNode->filePath,buf);
buf[HASH_LEN] = '\0';
char* liveHash = strdup(buf);

  if(strcmp(liveHash, clientNode->hash) == 0){
    
    int fd = open(path, O_CREAT | O_RDWR | O_APPEND, 00600);

    int v = atoi(ptr->version);
    v+=1;

    char* vr = malloc(sizeof(char)*(strlen(ptr->version) + 1));

    sprintf(vr, "%d", v);

    write(fd, "M", 1);
    write(fd, " ", 1);
    write(fd, vr, strlen(vr));
    write(fd, " ", 1); 
    write(fd, ptr->filePath, strlen(ptr->filePath));
    write(fd, " ", 1);
    write(fd, ptr->hash, strlen(ptr->hash));
    write(fd, "\n", 1);
    printf("M %s\n", ptr->filePath);

close(fd);
  
  }

}

else if(strcmp(ptr->hash, clientNode->hash) != 0){

//compute live hash
char buf[100];
getFileHash(path,buf);
buf[HASH_LEN] = '\0';
char* liveHash = strdup(buf);

 if(strcmp(liveHash, clientNode->hash) != 0){
 
      char* conflict = malloc(sizeof(char)*(strlen(projectName) + 20));
      sprintf(conflict, "./%s/%s", projectName, ".Conflict"); 
      
    int fd = open(conflict, O_CREAT | O_RDWR | O_APPEND, 00600);

    int v = atoi(ptr->version);
    v+=1;

    char* vr = malloc(sizeof(char)*(strlen(ptr->version) + 1));

    sprintf(vr, "%d", v);

    write(fd, "C", 1);
    write(fd, " ", 1);
    write(fd, vr, strlen(vr));
    write(fd, " ", 1); 
    write(fd, ptr->filePath, strlen(ptr->filePath));
    write(fd, " ", 1);
    write(fd, liveHash, strlen(liveHash));
    write(fd, "\n", 1);
    printf("C %s\n", ptr->filePath);

close(fd);
 }
}
ptr = ptr->next;
}

clientNode = clientList->head;

while(clientNode != NULL){

ptr = searchFile(serverList, clientNode);

if(ptr == NULL){

int fd = open(path, O_CREAT | O_RDWR | O_APPEND, 00600);

int v = atoi(clientNode->version);
v+=1;

char* vr = malloc(sizeof(char)*(strlen(clientNode->version) + 1));

sprintf(vr, "%d", v);

write(fd, "D", 1);
write(fd, " ", 1);
write(fd, vr, strlen(vr));
write(fd, " ", 1); 
write(fd, clientNode->filePath, strlen(clientNode->filePath));
write(fd, " ", 1);
write(fd, clientNode->hash, strlen(clientNode->hash));
write(fd, "\n", 1);
printf("D %s\n", clientNode->filePath);

close(fd);

}
 clientNode = clientNode->next;
}

clientNode = clientList->head;
ptr = serverList->head;


int msg = 1;
write(sfd, &msg, sizeof(int));
printf("Update successfull. \n");
return;
}

void upgrade(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + 20));

char* update = malloc(sizeof(char)*(strlen(projectName) + 20));

char* conflict = malloc(sizeof(char)*(strlen(projectName) + 20));

sprintf(cmd, "%s:%s\n", command, projectName);

sprintf(update, "./%s/%s", projectName, ".Update");
sprintf(conflict, "./%s/%s", projectName, ".Conflict");
write(sfd, cmd, strlen(cmd));

int msg = 0;

if(!checkFileExist(update)){
 printf("Error: No .Update file. First update then upgrade. \n");
 write(sfd, &msg, sizeof(int));
 free(cmd);
 free(update);
 free(conflict);
 return;
}

if(checkFileExist(conflict)){
 printf("Error: Found .Conflict file. First resolve the conflicts. \n");
 write(sfd, &msg, sizeof(int));
 free(cmd);
 free(update);
 free(conflict);
 return;
}
msg = 1;
write(sfd, &msg, sizeof(int));


int lines = getLines(update);
write(sfd, &lines, sizeof(int));

if(lines == 0){
printf("No update required. \n");
return;

}

struct commitFileList* clientUpdate = getClientUpdate(projectName, lines);

struct ManifestFileList* clientManifest = receiveClientManifest(projectName);

struct ManifestFileList* serverManifest = receiveServerManifest(sfd, projectName);
struct commitNode* clientNode = clientUpdate->head;
struct commitNode* node = clientNode;

char a;

while(node != NULL){

if(node->cmd == 'D'){

a = 'd';
write(sfd, &a, sizeof(char));

struct manifestNode* ptr;

ptr = searchFileInManifest(clientManifest, node->filePath);

removeFromManifestList(clientManifest, ptr);
unlink(node->filePath);
node = node->next;
continue;
}

else if(node->cmd == 'A'){

struct manifestNode* ptr = clientManifest->head;

a = 's';
write(sfd, &a, sizeof(char)); 
char* dup = strdup(node->filePath);
createDirectoryIfNeeded(node->filePath, projectName);
strcpy(node->filePath, dup);

addToManifestList(clientManifest, node->filePath, node->hash, node->version);
clientManifest->numFiles++;

int size = strlen(node->filePath);
write(sfd, &size, sizeof(int));
write(sfd, node->filePath, size);
receiveServerFile(sfd, projectName, node->filePath);
node = node->next;
continue;
}

else if(node->cmd == 'M'){

struct manifestNode* ptr = clientManifest->head;

a = 's';
write(sfd, &a, sizeof(char));
ptr = clientManifest->head;

  while(ptr != NULL){
   
   if(strcmp(ptr->filePath, node->filePath) == 0){
     strcpy(ptr->hash, node->hash);
    break;
   }
   ptr = ptr->next;
  }

int size = strlen(node->filePath);
write(sfd, &size, sizeof(int));
write(sfd, node->filePath, size);
receiveServerFile(sfd, projectName, node->filePath);
node = node->next;
continue;
}

}

char* man = malloc(sizeof(char)*(strlen(projectName) + 20));
sprintf(man, "./%s/%s", projectName, ".Manifest");
int mfd = open(man, O_TRUNC | O_RDWR, 00600);
int v = atoi(clientManifest->versionNumber);

sprintf(clientManifest->versionNumber, "%s", serverManifest->versionNumber);

writeToManifestFileList(clientManifest, mfd);
close(mfd);

unlink(update);
free(man);

msg = 1;
write(sfd, &msg, sizeof(int));

printf("Upgrade done\n"); 
}

void checkout(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + strlen(command) + 5));

sprintf(cmd, "%s:%s\n", command, projectName);

write(sfd, cmd, strlen(cmd));
free(cmd);

int msg = 0;

if(checkDirectory(projectName)){
write(sfd, &msg, sizeof(int));
printf("Error: Project already in client directory. \n");
return;
}

msg = 1;

write(sfd, &msg, sizeof(int));

read(sfd, &msg, sizeof(int));

if(msg == 0){
printf("Error: project does not exist in server directory\n");
return;
}

msg = 1;

createDirectory(projectName);

char* version;

struct ManifestFileList* serverList = receiveServerManifest(sfd, projectName);


char* man_path = malloc(sizeof(char)*strlen(projectName) + 30);
sprintf(man_path, "./%s/%s", projectName, ".Manifest");

int mfd = open(man_path, O_CREAT | O_RDWR, 00600);
writeToManifestFileList(serverList, mfd);
close(mfd);

if(serverList->numFiles == 0){
 msg = 2;
 write(sfd, &msg, sizeof(int));
 printf("Checkout done. \n"); 
 return;
}

msg = 1;
write(sfd, &msg, sizeof(int));

int i;
struct manifestNode* ptr = serverList->head;

while(ptr != NULL){

char* dup = strdup(ptr->filePath);
createDirectoryIfNeeded(ptr->filePath, projectName);
strcpy(ptr->filePath, dup);
receiveServerFile(sfd, projectName, ptr->filePath);
ptr = ptr->next;
}

free(man_path);

write(sfd, &msg, sizeof(int));

printf("Checkout done. \n");
}

void history(int sfd, char* command, char* projectName){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + strlen(command) + 5));

sprintf(cmd, "%s:%s\n", command, projectName);

write(sfd, cmd, strlen(cmd));
free(cmd);

if(!checkDirectory(projectName)){
createDirectory(projectName);
}

char* path = malloc(sizeof(char)*(strlen(projectName) + 16));
sprintf(path, "./%s/%s", projectName, ".History");

receiveServerFile(sfd, projectName, path);

int msg = 1;
write(sfd, &msg, sizeof(int));

printf(".History received. \n");
}

void rollback(int sfd, char* command, char* projectName, char* version){

char* cmd = malloc(sizeof(char)*(strlen(projectName) + strlen(command) + 5));

sprintf(cmd, "%s:%s\n", command, projectName);

write(sfd, cmd, strlen(cmd));
free(cmd);
int v = atoi(version);
write(sfd, &v, sizeof(int));

int msg;
read(sfd, &msg, sizeof(int));

if(msg == 1){
printf("Rollback successful. \n");
}
else{
printf("Error: Rollback unsuccessful. \n");
}

}

int connectToServer(char* IP, int port){

int sfd,connfd;
struct sockaddr_in servaddr, cli;


sfd = socket(AF_INET,SOCK_STREAM,0);

 if (sfd == -1) { 
        printf("Error: socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(port);    

 if (connect(sfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("Error: connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("Connected to the server..\n"); 
        
        return sfd;
        
}

int main(int argc, char* argv[]){


if(argc<2){
printf("Error: No command found\n");
exit(0);
}

if((strcmp(argv[1], "configure") != 0) && (strcmp(argv[1], "checkout") != 0) && (strcmp(argv[1], "update") != 0) &&
   (strcmp(argv[1], "upgrade") != 0) && (strcmp(argv[1], "commit") != 0) && (strcmp(argv[1], "push") != 0) && 
   (strcmp(argv[1], "create") != 0) && (strcmp(argv[1], "destroy") != 0) && (strcmp(argv[1], "add") != 0) &&
   (strcmp(argv[1], "remove") != 0) && (strcmp(argv[1], "currentversion") != 0) && (strcmp(argv[1], "history") != 0) &&
   (strcmp(argv[1], "rollback") != 0)){
   
   printf("Error: Not a valid command\n");
   return 0;
 }

char* CONFIGURE = ".Configure";
char* MANIFEST = ".Manifest";
char* IP = malloc(sizeof(char)*13);
char* projectName;
char str[22];
int port;
int sfd;

char* p = malloc(sizeof(char)*5);
if(strcmp(argv[1],"configure") == 0){
  if(argc<4){
   printf("Error: Parameters missing\n");
   exit(0);
  }
int fd = open(CONFIGURE, O_RDWR | O_CREAT, 00600);
  configure(argv[2],argv[3],fd);
  printf("Project configured\n");
  close(fd);
}

if(!checkFileExist(CONFIGURE)){
 printf("Error: First configure\n");
 exit(0);
}

char* token;
 int config_fd = open(CONFIGURE, O_RDONLY, 00600);
  read(config_fd, str,22); 
    
    token = strtok(str," ");
   strncpy(IP,token,12);
   token = strtok(NULL, " ");
   strncpy(p, token,5);
   port = atoi(p);
   
   printf("\n");


        
 if(strcmp(argv[1],"checkout") == 0){
  if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
  projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  sendCommandToServer(sfd,"checkout",projectName); 
 }
 
 else if(strcmp(argv[1],"create") == 0){
    if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  sendCommandToServer(sfd,"create",projectName);
 }
 
 else if(strcmp(argv[1],"destroy") == 0){
    if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  destroy(sfd,"destroy",projectName);
  
 }
 
 else if(strcmp(argv[1],"add") == 0){
    if(argc<4){
   printf("Error: Parameters missing\n");
   close(sfd);
   exit(0);
  }
  char* fileName;
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  fileName = (char*)malloc(sizeof(char)*(strlen(argv[3])));
  strcpy(fileName, argv[3]);
  add(sfd,"add",projectName, fileName);
  
 }
  
 else if(strcmp(argv[1],"remove") == 0){
    if(argc<4){
   printf("Error: Parameters missing\n");
   close(sfd);
   exit(0);
  }
  char* fileName;
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  fileName = malloc(sizeof(char)*(strlen(argv[3])));
  strcpy(fileName, argv[3]);
  removeFile(sfd,"remove",projectName, fileName);
  
 }
 
 else if(strcmp(argv[1],"currentversion") == 0){
    if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  currentVersion(sfd,"currentversion",projectName);
  
 }
 
 else if(strcmp(argv[1],"commit") == 0){
    if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  commit(sfd,"commit",projectName);
  
 }
 
 else if(strcmp(argv[1], "push") == 0){
 
 if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  push(sfd,"push",projectName);
 
 }
 
 else if(strcmp(argv[1], "update") == 0){
 
 if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  update(sfd,"update",projectName);
 
 }
 
 else if(strcmp(argv[1], "upgrade") == 0){
 
 if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  upgrade(sfd,"upgrade",projectName);
 
 }
 
 else if(strcmp(argv[1], "checkout") == 0){
 
 if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  checkout(sfd,"checkout",projectName);
 
 }
  
  else if(strcmp(argv[1], "history") == 0){
 
 if(argc<3){
   printf("Error: Project name missing\n");
   close(sfd);
   exit(0);
  }
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  history(sfd,"history",projectName);
 
 }     
 
 else if(strcmp(argv[1],"rollback") == 0){
    if(argc<4){
   printf("Error: Parameters missing\n");
   close(sfd);
   exit(0);
  }
  char* v;
  sfd = connectToServer(IP, port);
    projectName = malloc(sizeof(char)*(strlen(argv[2])));
  strncpy(projectName,argv[2],strlen(argv[2]));
  v = (char*)malloc(sizeof(char)*(strlen(argv[3])));
  strcpy(v, argv[3]);
  rollback(sfd,"rollback",projectName, v);
  
 }
        close(sfd);
        return 0;
}
