#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#include "socket.h"
#include "manifest.h"
#include "commit.h"

#define SA struct sockaddr 


char* SERVER_REPO = "Server_repository";

void createDirectory(char* path){

struct stat buffer;

 if(stat(path,&buffer) == -1){
   mkdir(path, 0777);
 }
 
}

int checkFile(char* file){

struct stat buffer;
 if(stat(file,&buffer) == 0){
  return 1;
 }
 return -1;
}

int checkIsFile(char* path){
struct stat buf;
 stat(path,&buf);
 return S_ISREG(buf.st_mode);
}

long int getFileSize(char* file){

struct stat buffer;

if(stat(file, &buffer) == 0){
 return buffer.st_size;
}
return -1;
}

char* getFileName(char* path){

char* fileName = strrchr(path, '/'); 
fileName++;
return fileName;
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

int checkDirectory(char* dir){

struct stat buffer;

if(!stat(dir,&buffer)){
  return(buffer.st_mode & S_IFDIR);
 }
 return 0;
}

int checkFileOrDirectory(char* token){

if(strchr(token, '.') == NULL){
return 1; //Directory
}
return 2; //File
}

void createDirectoryIfNeeded(char* newPath, char* projectName, int version){
 
char* token = strtok(newPath, "/");
char* path = malloc(sizeof(char)*(strlen(newPath)));
int c = 0;
sprintf(path, "%s/%d",projectName, version);

while(token != NULL){

if(c<=3){
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

void copyFile(char* sourceFile, char* destFile){

int sfd = open(sourceFile, O_RDONLY, 00600);

  if(sfd == -1){
   printf("Error: Cannot open source file\n");
   exit(0);
  }
  
int dfd = open(destFile, O_CREAT | O_WRONLY, 00600);

  if(dfd == -1){
   printf("Error: Cannot open destination file\n");
   exit(0);
  }  
  
  char ch;
  
 while(read(sfd, &ch, 1) != 0){
   write(dfd, &ch, 1);
 }
 close(dfd);
 close(sfd);
}

char* getCurrentVersion(char* projectName){

char* v_path = malloc(sizeof(char)*(strlen(projectName) + 10));

char* version = malloc(sizeof(char)*2);
sprintf(v_path, "%s/%s", projectName, ".Version");

char* c = malloc(sizeof(char)*2);
char chr;
int x = 0;
int fd = open(v_path, O_RDWR, 00600);

 read(fd, &chr, sizeof(char));
 
 close(fd);
 
x = chr - '0';
sprintf(version, "%d", x);

 return version;
}

char* getNewPath(char* filePath, char* projectName, int version){

char* ch = strchr(filePath, '/');
ch++;
char* ch1 = strchr(ch, '/');

return ch1;
}

char* replaceVersion(char* filePath, char* projectName, int newVersion){

char* ch = strchr(filePath, '/');
ch++;
char* ch1 = strchr(ch, '/');
ch1++;
char* ch2 = strchr(ch1, '/');

return ch2;
}

void receiveFile(int connfd, char* project){

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();


tokenizeString(list,connfd,':');
char* fileLen = readList(list);
int len = atoi(fileLen);

tokenizeString(list1,connfd,':');
char* filePath = readList(list1);
char* file = malloc(sizeof(char)*(strlen(fileLen) + 20));
char* ch = strchr(filePath, '/');
sprintf(file, "./%s%s", SERVER_REPO, ch);
 
tokenizeString(list2,connfd,':');
char* fSize = readList(list2);
int fileSize = atoi(fSize);

int fd = open(file, O_RDWR | O_CREAT, 00600);

if(fd == -1){
printf("Error: Cannot open .Commit file.\n");
int x = -1;
write(connfd, &x, sizeof(int));
exit(0); 
}

char chr; 
int i = 0;
while(i < fileSize){

 read(connfd, &chr, 1);
 if(chr == '\0'){
 
  break;
 }
 write(fd, &chr, 1);
 i++;
}


close(fd);


}

void receiveClientFile(int connfd, char* project, char* newPath){

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();


tokenizeString(list,connfd,':');
char* fileLen = readList(list);
int len = atoi(fileLen);

tokenizeString(list1,connfd,':');
char* filePath = readList(list1);

tokenizeString(list2,connfd,':');
char* fSize = readList(list2);
int fileSize = atoi(fSize);

int fd = open(newPath, O_RDWR | O_CREAT, 00600);

if(fd == -1){
printf("Error: Cannot open file.\n");
exit(0); 
}

char chr; 
int i = 0;
while(i < fileSize){

 read(connfd, &chr, 1);
 if(chr == '\0'){
 
  break;
 }
 write(fd, &chr, 1);
 i++;
}
close(fd);
}

struct ManifestFileList* receiveServerManifest(char* project){

char* path = malloc(sizeof(char)*(strlen(project) + 20));

sprintf(path, "./%s/%s/%s", SERVER_REPO, project, ".Manifest");

if(!checkFile(path)){
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

struct commitFileList* receiveClientCommit(int connfd, char* project, int clines){

struct socketList* list = createList();
struct socketList* list1 = createList();
struct socketList* list2 = createList();

tokenizeString(list,connfd,':');

tokenizeString(list1,connfd,':');

tokenizeString(list2,connfd,':');

struct commitFileList* list3 = readFromCommit(connfd, clines);
freeList(list);
freeList(list1);
freeList(list2);
return list3;
}

struct commitFileList* receiveServerCommit(char* path, int lines){

if(!checkFile(path)){
printf("Error: .Commit not found in server. First commit then push.\n");
exit(0);
}

int cfd = open(path, O_RDONLY, 00600);

if(cfd == -1){
printf("Error: Cannot open server .Commit\n");
exit(0);
}

struct commitFileList* list = readFromCommit(cfd, lines);

close(cfd);
free(path);
return list;
}

void removeProject(char* project){

DIR *dir = opendir(project);

int len = strlen(project);

char* path = malloc(sizeof(char)*(len + 100));

struct dirent* ptr;

if(dir){


  while((ptr = readdir(dir)) != NULL){
    
    if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
     
     strcpy(path,project);
     strcat(path,"/");
     strcat(path, ptr->d_name);
     
     struct stat s;
     
     if(!stat(path, &s)){
       if(S_ISDIR(s.st_mode)){
         removeProject(path);
       }
       else{
        unlink(path);
       }
     }
    }
    
    
  }

}
else{
return;
}
closedir(dir);
}

void sendFileToClient(int connfd, char* path, char* projectName){

char* filePath = malloc(sizeof(char)*(strlen(projectName) + strlen(path) + 50));

int size = getFileSize(path);

sprintf(filePath, "%d:%s:%d:", strlen(path), path, size);
write(connfd, filePath, strlen(filePath));

char chr;

int fd = open(path, O_RDONLY, 00600);
while(read(fd,&chr,1) != 0){
write(connfd, &chr, 1);
}

close(fd);
}

void writeToHistory(int hfd, struct commitNode* node, char* version){

write(hfd, version, strlen(version));
write(hfd, "\n", 1);

while(node != NULL){

write(hfd, &(node->cmd), sizeof(char));
write(hfd, " ", 1);
write(hfd, node->filePath, strlen(node->filePath));
write(hfd, "\n", 1);

node = node->next;
} 

}

void executeCommand(int connfd){

int size,psize;

struct socketList* list = createList();
struct socketList* list1 = createList();

tokenizeString(list,connfd,':');
char* command = readList(list);
printf("Command: %s\n", command);


tokenizeString(list1,connfd,'\n');
char* projectName = readList(list1);
printf("Project: %s\n", projectName);

printf("\n");

if(!checkDirectory(SERVER_REPO)){
createDirectory(SERVER_REPO);
}

char* projectPath = malloc(sizeof(char)*(strlen(projectName) + 25));
sprintf(projectPath, "./%s/%s", SERVER_REPO, projectName);

if(strcmp(command,"create") == 0){

if(checkDirectory(projectPath)){
 printf("Error: Project exists already\n");
 exit(0);
}

char* path = malloc(sizeof(char)*(strlen(projectPath) + 50));
createDirectory(projectPath);

char* p2 = malloc(sizeof(char)*(strlen(projectPath) + 50));
sprintf(p2, "./%s/%s/%s", SERVER_REPO, projectName, ".Version");

int v_fd = open(p2, O_RDWR | O_CREAT, 0777);
write(v_fd, "0", 1);
close(v_fd);

free(p2);


sprintf(path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");

int mfd = open(path, O_RDWR | O_CREAT, 00600);

write(mfd, projectName, strlen(projectName));
write(mfd, "\n", 1);
write(mfd, "0", 1);
write(mfd, "\n", 1);
write(mfd, "0", 1);
write(mfd, "\n", 1);
close(mfd);
printf("Project created.\n");


sendFileToClient(connfd, path, projectPath);
free(path);
}

else if(strcmp(command, "destroy") == 0){

if(!checkDirectory(projectPath)){
 printf("Error: Project does not exist\n");
 exit(0);
}
char* baseDir = malloc(sizeof(char)*(strlen(projectPath) + 3));
strcat(baseDir , projectPath);
removeProject(baseDir);
rmdir(projectPath);
write(connfd, "success.", strlen("success."));
}

else if(strcmp(command, "currentversion") == 0){

if(!checkDirectory(projectPath)){
 printf("Error: Project does not exist.\n");
 exit(0);
}

char* path = malloc(sizeof(char)*(strlen(projectPath) + 50));
sprintf(path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");
sendFileToClient(connfd, path, projectPath);
 
}

else if(strcmp(command, "commit") == 0){

char* path = malloc(sizeof(char)*(strlen(projectPath) + 12));

sprintf(path, "%s/%s",  projectPath, ".Manifest");

sendFileToClient(connfd, path, projectPath);

char* commit = malloc(sizeof(char)*(strlen(projectPath) + 12));
sprintf(commit, "%s/%s",  projectPath, ".Commit");
receiveClientFile(connfd, projectName,commit);

int x = 1;
write(connfd, &x, sizeof(int));
printf(".Commit received\n");

}

else if(strcmp(command, "push") == 0){

char* commit = malloc(sizeof(char)*(strlen(projectPath) + 10));
sprintf(commit, "./%s/%s/%s", SERVER_REPO, projectName, ".Commit");

int lines = getLines(commit);
struct commitFileList* serverCommit = receiveServerCommit(commit, lines);


int clines;
read(connfd, &clines, sizeof(int));
struct commitFileList* clientCommit = receiveClientCommit(connfd, projectPath, clines);

struct commitNode* clientNode = clientCommit->head;
struct commitNode* clientHead = clientCommit->head;
struct commitNode* serverNode = serverCommit->head;
int check;

check = compareCommit(clientCommit, serverCommit);

if(check == 0){
 printf("Error: Server and Client .Commit not same\n");
 close(connfd);
 exit(0);
}


char* version = getCurrentVersion(projectPath);

int newVersion = atoi(version); 
int oldVersion = newVersion;
newVersion += 1;


char* path = malloc(sizeof(char)*(strlen(projectPath) + 4));
sprintf(path, "./%s/%s/%d", SERVER_REPO, projectName, newVersion);
createDirectory(path);

struct commitNode* node = clientNode;

char* man_path = malloc(sizeof(char)*(strlen(projectPath) + 12));
if(newVersion == 1){
sprintf(man_path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");
}
else{
sprintf(man_path, "./%s/%s/%d/%s", SERVER_REPO, projectName, oldVersion, ".Manifest");
}
int mfd = open(man_path, O_RDONLY, 00600);
struct ManifestFileList* list = readFromManifest(mfd); 
close(mfd);

 
char code = 'r';

while(node != NULL){

char* newPath = getNewPath(node->filePath, projectPath, newVersion);

char* newFilePath = malloc(sizeof(char)*(strlen(node->filePath) + 20));

sprintf(newFilePath, "./%s/%s/%d%s", SERVER_REPO, projectName, newVersion, newPath);

char* dupPath = strdup(newFilePath);

if(node->cmd == 'A'){

createDirectoryIfNeeded(newFilePath, projectPath, newVersion);
write(connfd, &code, 1);
receiveClientFile(connfd, projectPath, dupPath);
}
else if(node->cmd == 'M'){

createDirectoryIfNeeded(newFilePath, projectPath, newVersion);
write(connfd, &code, 1);
receiveClientFile(connfd, projectPath, dupPath);
}
free(dupPath);
free(newFilePath);

node = node->next;
}

mfd = open(man_path, O_RDONLY, 00600);
struct ManifestFileList* oldList = readFromManifest(mfd); 
close(mfd);


struct manifestNode* ptr;
struct manifestNode* temp;
//Update file path to newVersion
int numFiles = list->numFiles;

node = clientHead; 



 while(node != NULL){
 
 char* newPath = getNewPath(node->filePath, projectPath, oldVersion);

 char* newFilePath = malloc(sizeof(char)*(strlen(node->filePath) + 10));

if(newVersion > 1){
 sprintf(newFilePath, "./%s/%d%s", projectName, oldVersion, newPath);
}
else if(newVersion == 1){
sprintf(newFilePath, "./%s/%d%s", projectName, newVersion, newPath);
}
 strcpy(node->filePath, newFilePath);
 
 node = node->next;
 }

node = clientHead;


while(node != NULL){

ptr = list->head;

   while(ptr != NULL){
   
     if(strcmp(node->filePath, ptr->filePath) == 0){
       break;
     }
   
     ptr = ptr->next;
   }
   
   
      if(node->cmd == 'A'){
        //Just add to manifest
        addToManifestList(list, node->filePath, node->hash, node->version);
        list->numFiles++; 
      }
      else if(node->cmd == 'M'){
      
        // JUST REPLACE THE HASH
        temp = list->head;
         while(temp != NULL){
           if(strcmp(temp->filePath, node->filePath) == 0){
             strcpy(temp->hash, node->hash);
             
             break;
           }
         temp = temp->next;
         }
      }
      else if(node->cmd == 'D'){
        //REMOVE FROM MANiFEST
        temp = ptr;
        removeFromManifestList(list, ptr); 
      }
   
  node = node->next;
}

ptr = list->head;
if(newVersion > 1){
while(ptr != NULL){

char* newPath = replaceVersion(ptr->filePath, projectPath, newVersion);
char* newFilePath = malloc(sizeof(char)*(strlen(ptr->filePath) + 20));

sprintf(newFilePath, "./%s/%d%s", projectName, newVersion, newPath);

char* dupPath = strdup(newFilePath);
strcpy(ptr->filePath, newFilePath);

ptr = ptr->next;
}
}
char* man = malloc(sizeof(char)*(strlen(projectPath) + 20));
sprintf(man, "./%s/%s/%d/%s", SERVER_REPO, projectName, newVersion, ".Manifest");
mfd = open(man, O_CREAT | O_RDWR | O_TRUNC, 00600);
char* v = malloc(sizeof(char) * 2);
sprintf(v, "%d", newVersion);
strcpy(list->versionNumber, v);
writeToManifestFileList(list, mfd);
close(mfd);

ptr = list->head;

struct manifestNode* oldPtr = oldList->head;

while(ptr != NULL){

char* t = malloc(sizeof(char)*(strlen(ptr->filePath) + 20)); 
char* ch = strchr(ptr->filePath, '/');
sprintf(t, "./%s%s", SERVER_REPO, ch);

 if(checkFile(t) == -1){
   
   char* str = strdup(t);
  createDirectoryIfNeeded(t, projectPath, newVersion);
  strcpy(t, str);
 char* fileName = getFileName(t);
 
 while(oldPtr != NULL){
 
 char* oldFile = getFileName(oldPtr->filePath);
  if(strcmp(oldFile, fileName) == 0){
    temp = oldPtr;
    char* t1 = malloc(sizeof(char)*(strlen(oldPtr->filePath) + 20)); 
    ch = strchr(temp->filePath, '/');
    sprintf(t1 , "./%s%s", SERVER_REPO, ch);
    copyFile(t1, t);
    
  }
  oldPtr = oldPtr->next;
 }
   
   
 }
 
ptr = ptr->next;
}

char* vp = malloc(sizeof(char)*(strlen(projectPath) + 15));
sprintf(vp, "./%s/%s/%s", SERVER_REPO, projectName, ".Version");
int vfd = open(vp, O_RDWR | O_TRUNC, 00600);
 write(vfd, list->versionNumber, strlen(list->versionNumber));
close(vfd);

char* m = malloc(sizeof(char)*(strlen(projectPath) + 20));
sprintf(m, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest"); 
updateServerManifest(oldList, list);
mfd = open(m, O_RDWR, 00600);
writeToManifestFileList(oldList, mfd);
close(mfd); 

char* history = malloc(sizeof(char)*(strlen(projectPath) + 15));
sprintf(history, "./%s/%s/%s", SERVER_REPO, projectName, ".History");
int hfd = open(history, O_CREAT | O_RDWR | O_APPEND, 00600);
node = clientHead;
writeToHistory(hfd, node, list->versionNumber);
close(hfd);


unlink(commit);
int msg = 1;
write(connfd, &msg, sizeof(int));
printf("Push successful\n");
return;
}

else if(strcmp(command, "update") == 0){

printf("Sending server .Manifest ...\n");

char* path = malloc(sizeof(char)*(strlen(projectPath) + 15));
char* version = getCurrentVersion(projectPath);

sprintf(path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");

sendFileToClient(connfd, path, projectPath);

int msg;

read(connfd, &msg, sizeof(int));
if(msg == 1){
printf("No update required.\n");
return;
}
msg = 0;
 
read(connfd, &msg, sizeof(int));
if(msg == 1){
printf("Update successful. \n");
return;
}
else{
printf("Error: update unsuccessful \n");
return;
}

}

else if(strcmp(command, "upgrade") == 0){

int msg;
read(connfd, &msg, sizeof(int));

if(msg == 0){
printf("Error: Upgrade failed on client side. Disconnecting.. \n");
return;
}

int lines;
read(connfd, &lines, sizeof(int));

if(lines == 0){
printf("No update required. \n");
return;
}

char* path = malloc(sizeof(char)*(strlen(projectPath) + 15));
sprintf(path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");
sendFileToClient(connfd, path, projectPath);
struct ManifestFileList* serverManifest = receiveServerManifest(projectName);
char* version = serverManifest->versionNumber;

int len;
char a;
int i = 0;

while(i < lines){

read(connfd, &a, sizeof(char));

if(a == 's'){

read(connfd, &len ,sizeof(int));

char* file = malloc(sizeof(char)*len);

read(connfd, file, len);

char* newPath = getNewPath(file, projectPath, atoi(version));

char* filePath = malloc(sizeof(char)*(len + 10));

sprintf(filePath, "./%s/%s/%d%s", SERVER_REPO, projectName, atoi(version), newPath);

sendFileToClient(connfd, filePath, projectPath);
i++;
continue;
}

i++;
}
read(connfd, &msg, sizeof(int));
if(msg == 1){
printf("Upgrade done. \n");
}
else{
printf("Error: Upgrade failed. \n");
}

}

else if(strcmp(command, "checkout") == 0){

int msg;
read(connfd, &msg, sizeof(int));

if(msg == 0){
printf("Error: Project already in client directory. \n");
return;
}

if(!checkDirectory(projectPath)){
printf("Error: Project does not exist\n");
msg = 0;
write(connfd, &msg, sizeof(int));
return;
}

msg = 1;
write(connfd, &msg, sizeof(int));

char* version = getCurrentVersion(projectPath);

char* path = malloc(sizeof(char)*(strlen(projectName) + 30));

sprintf(path, "./%s/%s/%s", SERVER_REPO, projectName, ".Manifest");

if(!checkFile(path)){
printf("Error: .Manifest not found\n");
exit(0);
}

sendFileToClient(connfd, path, projectPath);


read(connfd, &msg, sizeof(int));

if(msg == 2){
printf("Checkout done. \n");
return;
}


int mfd = open(path, O_RDONLY, 00600);

if(mfd == -1){
printf("Error: Cannot open server .Manifest\n");
close(mfd);
return;
}

struct ManifestFileList* list = readFromManifest(mfd);

struct manifestNode* ptr = list->head;

while(ptr != NULL){

char* newPath = getNewPath(ptr->filePath, projectName, atoi(version));
char* file = malloc(sizeof(char)*(strlen(ptr->filePath) + 10));
sprintf(file, "./%s/%s/%d%s", SERVER_REPO, projectName, atoi(version), newPath);
sendFileToClient(connfd,file, projectPath);

ptr = ptr->next;
}

read(connfd, &msg, sizeof(int));
if(msg == 1){
printf("Checkout done. \n");
}
else{
printf("Error: checkout failed in client. \n");
}

}

else if(strcmp(command, "history") == 0){

if(!checkDirectory(projectPath)){
printf("Error: Project not in the server repository.\n");
return;
}


char* path = malloc(sizeof(char)*(strlen(projectPath) + 12));

sprintf(path, "%s/%s", projectPath, ".History");

int hfd = open(path, O_RDWR, 00600);
sendFileToClient(connfd, path, projectPath);
close(hfd);

int msg = 0;
read(connfd, &msg, sizeof(int));
if(msg == 1){
printf(" .History received by client. \n");
}
else{
printf("Error: .History not received by client. \n");
}

}

else if(strcmp(command, "rollback") == 0){

char* version = malloc(sizeof(char)*3);
int v;
read(connfd, &v, sizeof(int));
sprintf(version, "%d", v);
int msg = 1;

if(!checkDirectory(projectPath)){
msg = 0;
write(connfd, &msg, sizeof(int));
printf("Error: Project not in the server directory. \n");
return;
}

if(v < 0){
msg = 0;
write(connfd, &msg, sizeof(int));
printf("Error: Invalid version number");
return;
}


char* cv = getCurrentVersion(projectPath);
int sv = atoi(cv);

if(v > sv){
msg = 0;
write(connfd, &msg, sizeof(int));
printf("Error: Invalid version number");
return;
}

while(sv > v){
char* path = malloc(sizeof(char)*(strlen(projectPath) + 5));
sprintf(path, "%s/%d", projectPath, sv);
removeProject(path);
rmdir(path);
sv--;
}

char* man = malloc(sizeof(char)*(strlen(projectPath) + 14));
sprintf(man, "%s/%d/%s", projectPath, v, ".Manifest");
int mfd = open(man, O_RDWR, 00600);
struct ManifestFileList* list = readFromManifest(mfd);
close(mfd);

struct manifestNode* ptr = list->head;

while(ptr != NULL){

char* newPath = replaceVersion(ptr->filePath, projectName, v);
char* file = malloc(sizeof(char)*(strlen(projectPath) + strlen(newPath) + 5));
sprintf(file, "./%s%s", projectName, newPath);
strcpy(ptr->filePath, file);

ptr = ptr->next; 
}

char* man_path = malloc(sizeof(char)*(strlen(projectPath) + 14));
sprintf(man_path, "%s/%s", projectPath, ".Manifest");
 mfd = open(man_path, O_TRUNC | O_RDWR, 00600);
writeToManifestFileList(list, mfd);
close(mfd);

char* vf = malloc(sizeof(char)*(strlen(projectPath) + 12));
sprintf(vf, "%s/%s", projectPath, ".Version");
int vfd = open(vf, O_TRUNC | O_RDWR, 00600);
write(vfd, version, strlen(version));
close(vfd);

msg = 1;
write(connfd, &msg, sizeof(int));
printf("Rollback successful.\n");
}

}


int main(int argc, char* argv[]){

if(argc<2){
printf("Error: Port number missing \n");
exit(0);
}

char* p = malloc(sizeof(char)*(strlen(argv[1]) + 2));
strcpy(p,argv[1]);
int port = atoi(p); 
int sfd,connfd, len;
struct sockaddr_in servaddr, cli;
sfd = socket(AF_INET, SOCK_STREAM, 0);

 if (sfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 

servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
servaddr.sin_port = htons(port);

 if ((bind(sfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
        
        if ((listen(sfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    
    connfd = accept(sfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
        
        
       executeCommand(connfd);
         
        close(sfd);
        printf("Client disconnected\n");
        free(p);
return 0;
}


