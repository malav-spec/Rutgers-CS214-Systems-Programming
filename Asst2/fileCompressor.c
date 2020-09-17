#include <stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include <sys/stat.h>
#include<fcntl.h>
#include<limits.h>

struct node{

char* token;
struct node* next;
int frequency;
int repeat;
int size;
char* code;
int* huffmanCode;
};

struct heapNode{
char* string;
int frequency;
struct heapNode* left;
struct heapNode* right;
struct heapNode* parent;
int*  huffmanCode;
};

void addNode(struct node* nd);
void addNodeCpy(struct node* nd);
struct heapNode* newHeapNode(char* str, int size, int freq);
struct node* newNode(int size);
void findRepeat();
void insertCopyList();
void swapNodes(struct heapNode* node1, struct heapNode* node2);
int getLeftChild(int index);
int getRightChild(int index);
int getParent(int index);
void printHeap(struct heapNode arr[]);
void  addHeapNode(struct heapNode arr[], struct heapNode* node);
void heapify(struct heapNode arr[], int index);
struct heapNode* getMinimum(struct heapNode arr[]);
struct heapNode* buildHuffmanCode(struct heapNode arr[]);
void printArray(int arr[], int n);
void storeToList(int arr[],char* token, int size);
void getHuffmanCode(struct heapNode* root, int arr[], int top);
void findFrequency(struct node* nd);
void tokenizer(char* string);
void printList(struct node* hd);
void freeList();
char* putHCZ(char* file, int size);
void writeToCodeBook(struct node* hd);
void editCodeBook(char* codeBook);
void printFiles(char* basePath, char flag);
int searchList1(char* code);
void writeToOriginalFile1(char* file, char* code);
void decompressFile1(char* path, char* codeBook);
void recursiveDecompress(char* basePath, char* codeBook);
void compressFile(char* path, char flag, char* codeBook);



struct node* head = NULL;
struct node* head_cpy = NULL;
int heap_size = 0;
int heap_array_size = 0;
int code_size = 0;
int max_size;
int two_flags = 0;
struct node* escape_char = NULL;



void addNode(struct node* nd){

struct node* ptr = head;

if(head == NULL){

head = nd;
return;
}

while(ptr->next!=NULL){
ptr = ptr->next;
}

ptr->next = nd;
return;
}

void addNodeCpy(struct node* nd){

struct node* ptr = head_cpy;

if(head_cpy == NULL){

head_cpy = nd;
return;
}
while(ptr->next!=NULL){
ptr = ptr->next;
}

ptr->next = nd;
return;
}

struct heapNode* newHeapNode(char* str, int size, int freq){

struct heapNode* temp = malloc(sizeof(struct heapNode));

temp->string = malloc(size*sizeof(char));
strcpy(temp->string,str);
temp-> frequency = freq;
temp->left = temp->right = NULL;
}

struct node* newNode(int size){

struct node* temp = malloc(sizeof(struct node));

temp->token = malloc(size*sizeof(char));
temp->next = NULL;
temp->repeat = 0;
return temp;

}


void findRepeat(){

head->repeat = 1;
struct node* ptr = head->next;
struct node* temp = head;

while(ptr!=NULL){


 while(temp != NULL){
     if(strcmp(ptr->token,temp->token) == 0 && (temp->repeat == 0)){
       ptr->repeat = 1;
       break;
     }
     if(strcmp(ptr->token,temp->token) == 0 && (temp->repeat == 1)){
      break;
     }
   temp = temp->next;
 }
 temp = head;
 ptr = ptr->next;
}

}
 
void insertCopyList(){

struct node* ptr = head;

while(ptr != NULL){

if(ptr->repeat == 1){
heap_array_size++;
struct node* temp = newNode(strlen(ptr->token));
strcpy(temp->token , ptr->token);
temp-> frequency = ptr->frequency;
temp-> repeat = ptr -> repeat;
addNodeCpy(temp);
}
ptr = ptr->next;
}

}
void swapNodes(struct heapNode* node1, struct heapNode* node2){

struct heapNode t;
t = *node1;
*node1 = *node2;
*node2 = t;
} 

int getLeftChild(int index){

if((index >= 0) && (((2*index)) <= heap_size)){

 return (2*index)+1;

}
return -1;
}

int getRightChild(int index){

if((index >= 0) && (((2*index)+1) <= heap_size)){

 return (2*index)+2;

}
return -1;
}

int getParent(int index){

if((index > 1) && (index < heap_size)){

 return (index/2);

}
else if(index == 1){
return 0;
}
return -1;
}

void printHeap(struct heapNode arr[]){
int i;
for(i=0;i<heap_size;i++){

printf("%d\t", arr[i].frequency);

}
printf("\n");

}

void  addHeapNode(struct heapNode arr[], struct heapNode* node){


int index = heap_size;
arr[heap_size] = *node; 
heap_size++;
while( index>0 && (arr[getParent(index)].frequency > node->frequency)) {

swapNodes(&arr[index] , &arr[getParent(index)]);
index = getParent(index);

}
}

void heapify(struct heapNode arr[], int index){ // sift down process

int smallest = index;

int left_index = getLeftChild(index);

int right_index = getRightChild(index);

if( left_index>0 && (left_index < heap_size)){

  if(arr[left_index].frequency < arr[smallest].frequency){
  
   smallest = left_index;
  }
}

if( right_index>0 && (right_index < heap_size)){

  if(arr[right_index].frequency < arr[smallest].frequency){
  
   smallest = right_index;
  }
}

if(smallest != index){

swapNodes(&arr[index], &arr[smallest]);
heapify(arr, smallest);

}
}

struct heapNode* getMinimum(struct heapNode arr[]){

struct heapNode min = arr[0];
struct heapNode t = arr[heap_size-1];
arr[0] = t;

heap_size--;

heapify(arr, 0);

struct heapNode* temp = malloc(sizeof(struct heapNode));

temp->string = min.string;

temp->frequency = min.frequency;

temp->left = min.left;

temp->right = min.right;
return temp;

}


struct heapNode* buildHuffmanCode(struct heapNode arr[]){

struct heapNode* min1;
struct heapNode* min2;
struct heapNode* temp = malloc(sizeof(struct heapNode));

while(heap_size > 1){

min1 = getMinimum(arr);
min2 = getMinimum(arr);

temp->string = '\0';
temp->left = min1;
temp->right = min2;
temp->frequency = min1->frequency + min2->frequency;
addHeapNode(arr, temp);
}

return getMinimum(arr);
}

void printArray(int arr[], int n){
int i;
for(i=0;i<n;i++){
printf("%d", arr[i]);

}
}

void storeToList(int arr[],char* token, int size){

struct node* ptr = head;
int i;
while(ptr != NULL){

if(strcmp(ptr->token,token) == 0){
ptr->huffmanCode =  malloc(sizeof(int)* size);
 for(i=0;i<size;i++){
   ptr->huffmanCode[i] = arr[i];
 }

ptr->size = size;
}
ptr = ptr->next;
}

ptr = head_cpy;
while(ptr != NULL){
 if(strcmp(ptr->token, token) == 0){
 ptr->huffmanCode = malloc(sizeof(int)*size);
 for(i=0;i<size;i++){
  ptr->huffmanCode[i] = arr[i];
 }
 ptr->size = size;
 }
ptr = ptr->next;
}

}

void getHuffmanCode(struct heapNode* root, int arr[], int top){

if(root->left){
arr[top] = 0;
getHuffmanCode(root->left, arr, top+1);
}

if(root->right){
arr[top] = 1;
getHuffmanCode(root->right, arr, top+1);
}

if(root->left == NULL && root->right == NULL){
root->huffmanCode = arr;
storeToList(arr, root->string, top);
}

}

void findFrequency(struct node* nd){

struct node* ptr = head;
int count = 0;
if(head == NULL){
return;
}

while(ptr!=NULL){

 if(strcmp(ptr->token,nd->token)==0){
 count++;
 
 }
 ptr = ptr->next;
}

nd->frequency = count;
return;
}

void tokenizer(char* string){

int i,c =0;
char* s1;
if( strlen(string) == 0){

 struct node* pt = newNode(1);
 strcpy(pt->token, "\n");
 addNode(pt);
}

else{
 s1 = malloc(sizeof(char)*(strlen(string)));
strcpy(s1,string);
 for(i=0;i<strlen(s1);i++){
  if(s1[i] == '\t' || s1[i] == ' ' || s1[i] == '\n'){
    c = 0;
    continue;   
   }
  c = 1;
  break;
 }

if(c == 1){
int string_size = strlen(string);
char* string_cpy = malloc(string_size*sizeof(char));
char* string_cpy1 = malloc(string_size*sizeof(char));
strcpy(string_cpy,string);
strcpy(string_cpy1,string);

char* delim  = " 	\n";

int size = 0;

char* token0 = strtok(string_cpy,delim);

while(token0 != NULL){
size++;
token0 = strtok(NULL,delim);
}
char* token1 = strtok(string_cpy1,delim);
struct node* ptr;

char* store_token;

while(token1!=NULL){
ptr = newNode(strlen(token1));
strcpy(ptr->token,token1);
addNode(ptr);

token1 = strtok(NULL,delim);

 }
 }
}

if(strlen(string) == 0){
return;
}

struct node* p = head;

  for(i=0;i<strlen(s1);i++){

  if(s1[0] == ' '  || s1[0] == '\t' || s1[0] == '\n'){
  char* escape = malloc(sizeof(char)*5);   
  if(s1[0] == ' '){escape = "[/s]";}
  else if(s1[0] == '\t'){ escape = "[/t]";}
  else{ escape = "[/n]";}  
  struct node* x = newNode(strlen(escape));
  strcpy(x->token, escape);
  x->next = p;
  head = x;
  p = p->next;
  }

  if(s1[i] == ' ' ){
   char escape[] = "[/s]"; 
   struct node* x = newNode(strlen(escape));
   strcpy(x->token,escape);
  if(p->next != NULL){ 
   x->next = p->next;
   p->next = x;
   p = p->next->next;
   }
  else{
  p->next = x;
  x->next = NULL;
  p = p->next;
   }  
  }
   else if(s1[i] == '\t'){
    char escape[] = "[/t]";
    struct node* x = newNode(strlen(escape));
    strcpy(x->token,escape);
    if(p->next != NULL){
    x->next = p->next;
    p->next = x;
    p = p->next->next;
     }
    else{
    p->next = x;
    x->next = NULL;
    p = p->next;
    }
   }
   else if(s1[i] == '\n'){
    char escape[] = "[/n]";
     struct node* x = newNode(strlen(escape));
     strcpy(x->token,escape);
     if(p->next != NULL){
      x->next = p->next;
      p->next = x;
      p = p->next->next;
     }
     else{
     p->next = x;
     x->next = NULL;
     p = p->next;
     }
   }
  
}

struct node* temp = head;
while(temp!=NULL){
 findFrequency(temp);
 temp = temp->next;
}
return;
}

void printList(struct node* hd){

struct node* ptr = hd;

while(ptr != NULL){
printf("%s: ", ptr->token);
printArray(ptr->huffmanCode,ptr->size); 
ptr = ptr->next;
}
}


void freeList(){

struct node* ptr ;
while(head != NULL){
ptr = head;
head=head->next;
free(ptr);
}

while(head_cpy!=NULL){
 ptr = head_cpy;
head_cpy = head_cpy->next;
free(ptr);
}

while(escape_char!=NULL){
ptr = escape_char;
escape_char = escape_char->next;
free(ptr);
}

}

char* putHCZ(char* file, int size){

return strcat(file, ".hcz");
}

void writeToFile(struct node* hd, char* fileName){
int i,s;
struct node* ptr = hd;
int fd = open(fileName, O_RDWR|O_CREAT, 00600);
while(ptr != NULL){
char* code = malloc(sizeof(char)*ptr->size);
  for(i=0;i<ptr->size;i++){
  
   code[i] = ptr->huffmanCode[i]+'0';
  }
 s = write(fd, code, ptr->size);
  ptr = ptr->next;
}

close(fd);
}

void writeToCodeBook(struct node* hd){

int i,s;
struct node* ptr = hd;
int fd = open("HuffmanCodebook", O_RDWR|O_CREAT|O_APPEND,00600);
if(hd == NULL){
 write(fd , "\\",1);
 write(fd, "\n", 1);
 close(fd);
 return;
}
s = write(fd, "\\", 1);
s = write(fd, "\n", 1);
while(ptr != NULL){
 char* code = malloc(sizeof(char)*ptr->size);
  for(i=0;i<ptr->size;i++){
   
  code[i] = ptr->huffmanCode[i]+'0';
  }

s = write(fd, code, ptr->size);
s = write(fd, "\t", 1);
s = write(fd, ptr->token, strlen(ptr->token));
s = write(fd, "\n", 1);
ptr = ptr->next;
}
close(fd);
}



void editCodeBook(char* codeBook){

 int fd = open(codeBook, O_RDWR);
 char buffer[1];
 char escape ;

  read(fd,buffer,1);
  escape = buffer[0];
  while(read(fd,buffer,1) !=0){
   if(buffer[0] == escape){
   break;
   }

  }

int fd1 = open("HuffmanCodebook1",O_RDWR|O_CREAT, 00600);
write(fd1,buffer,1);  
while(read(fd,buffer,1)!= 0){
    write(fd1,buffer,1);
   }
close(fd1);
close(fd);
rename("HuffmanCodebook1",codeBook);
}


void printFiles(char* basePath, char flag){

char path[100];
char path1[100];
struct dirent* de;
char* file;
char* fileName;
struct stat st;
DIR *dr = opendir(basePath);
char content[11];
if(dr==NULL){
return;
}

while((de = readdir(dr))!= NULL){
if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
memset(path,0,strlen(path)); 
strcpy(path,basePath);
 strcat(path,"/");
 strcat(path, de->d_name);

 int status = stat(path, &st);
if(flag == 'c'){
 if(!S_ISDIR(st.st_mode)){
 file = malloc(sizeof(char)*(strlen(de->d_name)));
 strcpy(file, de->d_name);
 int fd3 = open(path,O_RDONLY);
  if(fd3 == -1){ 
  printf("Error: No such file or directory\n");
  exit(0);
  }
close(fd3);
char* ext = malloc(sizeof(char)*4);
strcpy(path1, path);
 file = putHCZ(file, strlen(de->d_name));
 fileName = malloc(sizeof(char)*(strlen(de->d_name)+4));
 strcpy(fileName, file);
 strcat(path1,".hcz");
 int fd1 = open(path1, O_RDWR|O_CREAT, 00600);
 close(fd1);
 memset(file, 0 , strlen(file)); 
 }
}
  if(!S_ISDIR(st.st_mode)){
   int fd = open(path,O_RDONLY);

   if(fd == -1){
    printf("Error: No such file or directory\n");
    }

   char buffer[11];
   int length;

   while((length = read(fd,buffer,10)) != 0){
     
     strncat(content,buffer,length);
     }

   tokenizer(content);
   findRepeat();
   insertCopyList();

    struct heapNode* a = malloc(sizeof(struct heapNode)*heap_array_size);
    struct node* ptr = head_cpy;
    while(ptr != NULL){

        struct heapNode* n1 = newHeapNode(ptr->token, strlen(ptr->token), ptr->frequency);
	addHeapNode(a,n1);
	ptr = ptr->next;
     }

     struct heapNode* b = malloc(sizeof(struct heapNode)*heap_array_size);
     b = a;

    char* code;
    int* arr = malloc(sizeof(int)*heap_array_size);

    struct heapNode* hc = buildHuffmanCode(a);
    getHuffmanCode(hc,arr,0);
    if(flag == 'c'){
    writeToFile(head, path1);
    }
    if(flag == 'b'){
     writeToCodeBook(head_cpy);
    }
    freeList();
    heap_size = 0;
    head = NULL;
    head_cpy = NULL;
    escape_char = NULL;
    heap_array_size = 0;
 memset(content,0,strlen(content));
   
   close(fd);
   }
  printFiles(path,flag);
}
}
closedir(dr);     
}

int searchList1(char* code){

if(head == NULL){
 return 0;
}
int i;
struct node* ptr = head;

while(ptr!=NULL){
 
   if(strcmp(code,ptr->code) == 0){
    return 1;
   }
   
   ptr = ptr->next;
}
return 0;
}


void writeToOriginalFile1(char* file, char* code){


char* string;
char* escape; 
int i;
struct node* ptr = head;
while(ptr != NULL){

if(strcmp(ptr->code,code) == 0){
 string = malloc(sizeof(char)*(ptr->size));
 strcpy(string, ptr->token);
  break;
  } 
ptr = ptr->next;
}

int fd = open(file, O_RDWR|O_APPEND);
  
 if(strcmp(string, "[/s]") == 0){
 write(fd, " ", 1);
  }
 else if(strcmp(string, "[/t]") == 0){
 write(fd,"	",1);
 }
 else if(strcmp(string, "[/n]") == 0){
 write(fd, "\n", 1);
 }
 else{
  write(fd ,string,strlen(string));
  }

  close(fd);
}


void decompressFile1(char* path, char* codeBook){

int f = open(path, O_RDONLY);
 if(f == -1){
 printf("Error: Not a file\n");
}
int valid = 0;
char* string = malloc(sizeof(char));
char buffer[1];
char* token;
char* code;
char* ext = malloc(sizeof(char)*4);
memset(buffer,0,strlen(buffer));
memset(string,0,strlen(string));
ext = strrchr(path,'.');
if(strcmp(ext,".hcz") != 0){
 printf("Error: Not a .hcz file\n");
 exit(0);
}
int size= 0;
char* s = malloc(sizeof(char));
memset(s,0,strlen(s));
char* originalFile = malloc(sizeof(char)*(strlen(path) - 4));
strncpy(originalFile, path, strlen(path)-4);
 int fd1 = open(originalFile, O_RDWR|O_TRUNC|O_CREAT, 00600);
 close(fd1);
char escape;
int fd = open(codeBook, O_RDONLY);
if(read(fd ,buffer,1) != 0){
escape = buffer[0];
}
else{

int fds = open(originalFile, O_RDWR);
write(fds, "\n", 1);
close(fds);

return;
}
read(fd ,buffer,1);
int i;

while(read(fd,buffer,1) != 0){
i = 0;
        if(buffer[0] == '\\'){
          break;
        }
       if(buffer[0] != '\n'){

           if(buffer[0] != '\t' && buffer[0] != '0' && buffer[0] != '1'){
             
            strcat(s,buffer);
             if(size == 0){strcpy(string,s);}
            size++;
             valid = 1;
            continue;    
           } 
           else if(valid == 1 && buffer[0] == '\t'){
           token = malloc(sizeof(char)*size);
           strcpy(token,s);
           memset(s,0,strlen(s));
           valid = 0;
           size = 0;
           continue;
           } 
           else if(buffer[0] == '1' || buffer[0] == '0'){
           strcat(s,buffer);
           size++;    
            valid = 1;
            continue;
           }
           else if(valid == 1 && (buffer[0] == '1' || buffer[0] == '0')){
           code = malloc(sizeof(char)*size);
           strcpy(code, s);
           continue;        
        }

      }

code  = malloc(sizeof(char)*size);
strcpy(code,token);
struct node* ptr; 
ptr  = newNode(strlen(s));
strcpy(ptr->token, s);
ptr->code = malloc(sizeof(char)*size);
strcpy(ptr->code, code);
ptr->size = size;
addNode(ptr); 
memset(s,0,strlen(s));
memset(code,0,strlen(code));
size = 0;
valid = 0;
}

close(fd);

int fd2 = open(path, O_RDONLY);
memset(buffer,0,strlen(buffer));
code = malloc(sizeof(char));
read(fd,buffer,1);
   if(buffer == NULL){//Empty file
   exit(0);
   }

   
    valid = searchList1(buffer);
    if(valid){
    writeToOriginalFile1(originalFile,buffer);
    }
    else{
    strcpy(code,buffer);
    }
     
   while(read(fd,buffer,1)!=0){
     
     strncat(code,buffer,1);
     valid = searchList1(code);
     if(valid){
      writeToOriginalFile1(originalFile,code);
      memset(code,0,strlen(code));
      }
   }
close(fd2);

close(fd);
editCodeBook(codeBook);

}

void recursiveDecompress(char* basePath, char* codeBook){

char* file;
char* fileName;
char* extension = malloc(sizeof(char)*4);
struct stat st;
char path[100];
char newPath[100];
struct dirent* de;
DIR* dr = opendir(basePath);
if(dr == NULL){
return;
}

while((de = readdir(dr)) != NULL){
  if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
  strcpy(path, basePath);
  strcat(path, "/");
  strcat(path, de->d_name);

  int status = stat(path, &st);
   if(!S_ISDIR(st.st_mode)){
     if(strcmp(strrchr(path,'.'), ".hcz") == 0){
      decompressFile1(path, codeBook);
      
     }
   
 }
recursiveDecompress(path,codeBook);
    
  }

}

closedir(dr);
}


void compressFile(char* path, char flag, char* codeBook){

char* cb;
char content[10];
if(codeBook != NULL){ 
 cb = malloc(sizeof(char)*(strlen(codeBook)));
strcpy(cb, codeBook);
}

int fd = open(path,O_RDONLY);
if(fd == -1){
printf("Error: No such file\n");
exit(0);
}
if(flag == 'c' && !two_flags){
strcat(path,".hcz");

 int fd1 = open(path, O_RDWR|O_CREAT, 00600);
 close(fd1);
}
if(flag == 'b' && !two_flags ){

 int i,x=0;
  for(i=0;i<10;i++){
    x ++;
   }
}


   char buffer[10];
   int length;
   memset(buffer,0,strlen(buffer));
   memset(content,0,strlen(content));
  if(read(fd,buffer,10) != 0){
   strcpy(content,buffer);
  }

   while((length = read(fd,buffer,10)) != 0){
     
     strncat(content,buffer,length);
     }

   tokenizer(content);
   findRepeat();
   insertCopyList();

    struct heapNode* a = malloc(sizeof(struct heapNode)*heap_array_size);
    struct node* ptr = head_cpy;
    while(ptr != NULL){

        struct heapNode* n1 = newHeapNode(ptr->token, strlen(ptr->token), ptr->frequency);
	addHeapNode(a,n1);
	ptr = ptr->next;
     }

     struct heapNode* b = malloc(sizeof(struct heapNode)*heap_array_size);
     b = a;
    char* code;
    int* arr = malloc(sizeof(int)*heap_array_size);
    struct heapNode* hc = buildHuffmanCode(a);
    getHuffmanCode(hc,arr,0);
      
     if(two_flags && flag!= 'd'){
      strcat(path,".hcz");
        writeToFile(head,path);
        writeToCodeBook(head_cpy);
      }
     else if(flag == 'c'){
      writeToFile(head, path);
    }    
    else if(flag == 'b'){
     writeToCodeBook(head_cpy);
    }
    else if(flag == 'd'){
     strcat(path,".hcz"); 
      writeToFile(head, path);
     writeToCodeBook(head_cpy);
     freeList();
     head = NULL;
     head_cpy = NULL;
    decompressFile1(path, cb);
    }
    
   freeList();
    heap_size = 0;
    heap_array_size = 0;
    memset(content,0,strlen(content));
   
   close(fd);
return;
}


int main(int argc, char* argv[]){

if(argc>5){
printf("Too many arguments\n");
exit(0);
}
if(argc <3){
printf("Too few arguments\n");
exit(0);
}
if(argc>3 && argv[3][0] == '-'){
printf("Too many flags\n");
exit(0);
}

char flagR;
char flag;
char flag1;
char* path;
int i;

if(argv[1][1] == 'd' && argv[2][0] != '-'){
flag = argv[1][1];
}

else if((argc>=4 && argc<=5)&& ((argv[1][1] != 'b' && argv[1][1]!= 'c' && argv[1][1] != 'd' && argv[1][1] != 'R') || (argv[2][1] != 'b' && argv[2][1]!= 'c' && argv[2][1] != 'd' && argv[2][1]!= 'R'))){
printf("Error: Not a valid flag\n");
exit(0);
}

 if((argc>=3 && argc<5) && (argv[1][1]!='b' && argv[1][1] != 'c' && argv[1][1] != 'd' && argv[1][1] != 'R')){
  printf("Error: Not a valid flag\n");
 exit(0);
}

if(argv[1][1] == 'R' || argv[2][1] == 'R' ){
  if(argv[1][1] == 'R'){
     flagR = argv[1][1];  
   flag = argv[2][1];
      two_flags = 1;
  }
  else{
    flagR = argv[2][1];
    flag = argv[1][1];
    two_flags = 1;
  }

}

else if(argc >= 4 && argc <= 5 && (argv[1][1] != 'R' || argv[2][1] != 'R') ){

flag = argv[1][1];
flag1 = argv[2][1];
two_flags = 1;

 if((flag == 'b' && flag1 == 'd') || (flag == 'd'  && flag1 == 'b')){
   printf("Error: Not a valid flag combination\n");
   exit(0);
 }
 
 
}

else{
flag = argv[1][1];
if(argv[1][1] == 'R'){
printf("Error: No other flag with  flag -R\n");
exit(0); 
}
two_flags = 0;
}

if(two_flags && argc>4 && (flag1 != 'd' && flag!= 'd')){
   printf("Error: Too many arguments\n"); 
   exit(0);
 }


if(flagR == 'R' && strcmp(argv[3],"./") == 0){

if(flag == 'b' || flag == 'c'){
printFiles(".",flag);
}
else if(flag == 'd' && argv[4]!= NULL){
 recursiveDecompress(".",argv[4]);
}
else if(flag == 'd' && argv[4] == NULL){
printf("Error: No CodeBook given\n");
exit(0);
}
exit(0);
}

else if( flagR == 'R' && argv[3] != NULL){

if(flag == 'b' || flag == 'c'){
printFiles(argv[3],flag);
}
else if(flag == 'd' && argv[4] != NULL){
recursiveDecompress(argv[3],argv[4]);
}
else if(flag == 'd' && argv[4] == NULL){
printf("Error: No CodeBook given\n");
exit(0);
}
exit(0);
}

if(!two_flags && flag == 'b'){

if(argv[2] == NULL){
 printf("Error: No file given\n");
exit(0);
}
compressFile(argv[2], 'b' ,argv[3]);
}

if(flagR != 'R' && (flag == 'c' || flag1 == 'c')){
 
if(argv[2] == NULL){
printf("Error: No file given\n");
}

if(!two_flags && flag == 'c'){
 compressFile(argv[2], flag, argv[3]);
}
 
 if(flag == 'c' && two_flags ==1){
  
 compressFile(argv[3],flag1,argv[4]);
 }
 else if(flag1 == 'c' && two_flags == 1){
 compressFile(argv[3], flag, argv[4]);
 }
 
}

else if(flagR!= 'R' && (flag == 'd' || flag1 == 'd')){
if(argv[3] == NULL && !two_flags){
printf("Error: No CodeBook given\n");
exit(0);
}
decompressFile1(argv[2], argv[3]);
}

freeList();
head_cpy = NULL;
head = NULL;
return 0;
}

