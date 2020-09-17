#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
extern int errno;

struct node{
char* str;
int num;
struct node* next;
struct node* prev;
int size;
int valid;
int isDigit;
int ascii;
int pivot;
int index;
};

void store_to_Node(char*,int);
void check_digits(struct node*);
void print_list(struct node*);
char* delete_prev(char*, int);
void make_str_arr(struct node*);
struct node* lastNode(struct node*);
void make_arr(struct node*);
int  int_partition(int , int);
void int_quick_sort(int ,int);
void int_quick_sort_setup(int*);
int  string_partition(int, int);
void string_quick_sort(int,int);
void string_quick_sort_setup(char**);
int quickSort(void* toSort, int(*comparator)(void*,void*));
void string_arrange(struct node*,struct node*,struct node*);
int insertion_sort(void* toSort,int(*comparator)(void*,void*));
int string_comparator(void*,void*);
int int_comparator(void*, void*);
void print_array();
void print_str_array();
void free_list(struct node*);


int* arr;
int length;
char** str_arr;
struct node* head = NULL; 
int check_arr = 0;
int max_size = 0;
int check_dig = 0;


void store_to_Node(char* str,int valid){

int j,i=0;

struct node* temp = malloc(sizeof(struct node));
struct node* ptr = head; 
struct node* rear = NULL;

while(str[i]!='\0'){
i++;
}


if(head == NULL && valid == 1){// first complete string
temp->str = (char*)malloc(sizeof(char)*i);
for(j=0;j<i;j++){
 (temp->str)[j] = str[j];
}
temp->next = NULL;
temp->prev = NULL;
temp->size = i;
temp->valid = valid;
head = temp;
return;
}
else if(head == NULL && valid == 0){// first incomplete string
temp->str = (char*)malloc(sizeof(char)*i);
for(j=0;j<i;j++){
 (temp->str)[j] = str[j];
}
temp->next = NULL;
temp->prev = NULL;
temp->size = i;
temp->valid = valid;
head = temp;
return;
}


while(ptr!=NULL && ptr->valid==1){//to find the incomplete string
rear = ptr;
ptr=ptr->next;
}
if(ptr == NULL){
temp->str = (char*)malloc(sizeof(char)*i);
for(j=0;j<i;j++){
 (temp->str)[j] = str[j];
}
rear->next = temp;
temp->prev = rear;
temp->size = i;
temp->valid = valid;
temp->next = NULL;
return;
}

if(ptr->valid == 0){

 int y = 0;
 while(str[y]!='\0'){
y++;
}
ptr->str = (char*)realloc(ptr->str,sizeof(char)*(y+(ptr->size)));
int k,x=0;
for(k=(ptr->size);k<(y+(ptr->size));k++){
(ptr->str)[k] = str[x];
x++;
}
ptr->valid = 1;
ptr->size = y+(ptr->size);
return;
}

temp->str = (char*)malloc(sizeof(char)*i);
for(j=0;j<i;j++){
 (temp->str)[j] = str[j];
}

}

void check_digits(struct node* head){

struct node* ptr = head;
while(ptr!=NULL){
 int i = 0;
 while((ptr->str)[i]!='\0'){
  if(isdigit((ptr->str)[i])){
  
   ptr->num = atoi(ptr->str);
   ptr->isDigit = 1;
   break;
  }
  i++;
 }
 ptr=ptr->next;
}
}

void print_list(struct node* head){

struct node* ptr = head;

 int i = 0;
 
 if(ptr->isDigit == 1){
 while(ptr!=NULL){
 if(ptr->str == '\0'){
 ptr = ptr->next;
 }
  printf("%d\n",(ptr->num));
  ptr=ptr->next;
  }
  
 }
 else{
 
 while(ptr!= NULL){
 if(ptr->str[0] == '\0'){
 ptr = ptr->next;
 }
 printf("%s\n",(ptr->str));
 ptr=ptr->next;
 }

}


}


char* delete_prev(char* str, int count){

int i;

for(i=0;i<count;i++){
str[i] = '\0'; 
}

return str; 
}

struct node* lastNode(struct node* head){

struct node* ptr = head;
while(ptr->next!=NULL){
ptr=ptr->next;
}
return ptr;
}

void make_str_arr(struct node* head){
int i=0;
int s = 0;
struct node* ptr = head;
while(ptr!=NULL){
i++;
ptr=ptr->next;
}
length = i;
ptr = head;
s = ptr->size;
for(i=1;i<length;i++){
 
 if(s<(ptr->size)){
  s = ptr->size;
 }
 
}
max_size = s;
str_arr = malloc(length*sizeof(char*));
ptr = head;
i=0;
for(i=0;i<length;i++){
 str_arr[i] = malloc(s*sizeof(char));
}

for(i=0;i<length;i++){
 strcpy(str_arr[i],ptr->str);
 ptr=ptr->next;
}

}

void make_arr(struct node* head){
int i=0;
struct node* ptr = head;
while(ptr!=NULL){
ptr=ptr->next;
i++;
}
length = i;
arr = (int*)malloc(sizeof(int)*i);
ptr=head;
i=0;
while(ptr!=NULL){
arr[i] = ptr->num;
i++;
ptr=ptr->next;
}
}

int  int_partition(int lo, int hi){

  int left = lo+1;
 int right = hi;
 int pivot = arr[lo];
 int cmp;
 while(1){
 while(left <= right){
 //cmp = int_comparator(arr[left],pivot);
 if(arr[left]<pivot){//cmp==1){
  left++;
 }
 else{
 break;
 }
 }
 
 while(right > left){
 
 //cmp = int_comparator(arr[right],pivot);
 if(arr[right]<pivot){//cmp==1){
 break;
 }
 else{
 right--;
 }
 }
 
 if(left >= right){
 break;
 }
 
 int temp = arr[left];
 arr[left] = arr[right];
 arr[right]=temp;
 
 left++; right--;
 }
 
 arr[lo] = arr[left-1];
 arr[left-1] = pivot;
 return left-1;
}

void int_quick_sort(int lo,int hi){
 
 if( (hi-lo) <= 0){
 return;
 }
 int x; 
 x = int_partition(lo,hi);
 int_quick_sort(lo,x-1);
 
 int_quick_sort(x+1,hi);
 
}

void int_quick_sort_setup(int* arr){
 if(length<=1){
 return;
 }
 int_quick_sort(0,length-1);
}


int  string_partition(int lo, int hi){

  int left = lo+1;
 int right = hi;
 char* pivot = str_arr[lo];
 int cmp;
 while(1){
 while(left <= right){
 cmp = string_comparator(str_arr[left],pivot);
 if(cmp == 0){
  left++;
 }
 else{
 break;
 }
 }
 
 while(right > left){
 cmp = string_comparator(str_arr[right],pivot);
 if(cmp == 0){
 break;
 }
 else{
 right--;
 }
 }
 
 if(left >= right){
 break;
 }
 
 char* temp = (char*)malloc((max_size)*sizeof(char));
 strcpy(temp,str_arr[left]);
 strcpy(str_arr[left],str_arr[right]);
 strcpy(str_arr[right],temp);
 
 left++; right--;
 }
 
 str_arr[lo] = str_arr[left-1];
 str_arr[left-1] = pivot;
 
 return left-1;
}

void string_quick_sort(int lo,int hi){
 
 if( (hi-lo) <= 0){
 return;
 }
 int x; 
 x = string_partition(lo,hi);
 string_quick_sort(lo,x-1);
 
 string_quick_sort(x+1,hi);
 
}

void string_quick_sort_setup(char** str_arr){
 if(length<=1){
 return;
 }
 check_arr = 1;
 string_quick_sort(0,length-1);
}


int quickSort(void* toSort, int(*comparator)(void*,void*)){
 
 if(check_dig == 1){
  int_quick_sort_setup(arr);
  return 1;  
 }
 else{
  string_quick_sort_setup(str_arr);
  return 1;
 }
 return 0;
}

void string_arrange(struct node* head,struct node* ptr,struct node* rear){

if(ptr->next == NULL){
 ptr->prev->next = NULL;
 ptr->next = ptr->prev = NULL;
 rear->next->prev = ptr;
 ptr->next = rear->next;
 rear->next = ptr;
 ptr->prev = rear;
}
else{
ptr->prev->next = ptr->next;
ptr->next->prev = ptr->prev;
ptr->next = ptr->prev = NULL;
ptr->next = rear->next;
rear->next->prev = ptr;
rear->next = ptr;
ptr->prev = rear;
} 
}


int insertion_sort(void* toSort,int(*comparator)(void*,void*)){

if(head->isDigit){
struct node* head1 = (struct node*)(toSort);
int cmp;
struct node* ptr = head1->next;
struct node* rear = head1;
struct node* counter;
struct node* mark;

while(ptr!=NULL){
   counter = ptr;
   mark = ptr;
   cmp = comparator(ptr,rear);
   if(cmp){
   while(rear->prev !=NULL && (cmp)){
   rear = rear->prev;
   cmp = comparator(ptr,rear);
   }
   ptr=ptr->next;
   if(ptr!=NULL && rear == head && cmp == 1){
   
   mark->prev->next = mark->next;
   ptr->prev = mark->prev;
   mark->next=mark->prev=NULL;
   mark->next = head;
   head->prev = mark;
   head = mark;
   rear = ptr->prev;
   
  
   }
   
   else if(ptr!=NULL && cmp == 0){
   mark->prev->next = ptr;
   ptr->prev = mark->prev;
   mark->next=mark->prev=NULL;
   mark->next = rear->next;
   mark->prev = rear;
   rear->next->prev = mark;
   rear->next = mark;
   rear = ptr->prev;
   
   
   }
   
   else if(ptr== NULL && rear == head && cmp == 1){
   mark->prev->next = NULL;
   mark->next=mark->prev=NULL;
   mark->next = head;
   head->prev = mark;
   head = mark;
   
   }
   
   else{
   mark->prev->next = NULL;
   mark->next=mark->prev=NULL;
   mark->next = rear->next;
   rear->next->prev = mark;
   mark->prev = rear;
   rear->next = mark;
   
   }
   
   }
   else{
   rear = ptr;
   ptr=ptr->next;
   }
   
 }
 
 return 1;
 }
 else{
 
 struct node* head2 = (struct node*)toSort;
struct node* ptr = head2->next;
struct node* rear = head2;
struct node* counter;
struct node* mark;
int i = 0,c=1,v=1;

while(rear!=NULL){
rear->ascii = (int)(rear->str[i]);
rear=rear->next;
}
rear = head2;

while(ptr!=NULL){
   c=0;
   counter = ptr;
   mark = ptr;
   i=0;
   v = 1;
   c = comparator(ptr,rear);
   ptr->ascii = (int)(ptr->str[0]); 
   rear->ascii = (int)(rear->str[0]);
  
   if(c == 0){
   
   while(rear->prev !=NULL && v==1 && c==0){
   
   rear = rear->prev;
    c = comparator(ptr,rear);
    ptr->ascii = (int)(ptr->str[0]);
       rear->ascii = (int)(rear->str[0]);    
  
   if(c == 1){
     string_arrange(head,mark,rear);
        v = 0;
   }
 
   }//end while
   
   
   if(c==1 && ptr!= NULL){
   rear = ptr->prev;
   }
   else if(c==0){
   ptr=ptr->next;
   }   
  
   if(ptr!=NULL && rear == head && c == 0){
   mark->prev->next = mark->next;
   mark->next->prev = mark->prev;
   mark->next=mark->prev=NULL;
   mark->next = head;
   head->prev = mark;
   head = mark;
   rear = ptr->prev;
   }
   
   else if(ptr!=NULL && c==0){
    
   mark->prev->next = ptr;
   ptr->prev = mark->prev;
   mark->next=mark->prev=NULL;
   mark->next = rear->next;
   rear->next->prev = mark;
   mark->prev = rear;
   rear->next = mark;
   rear = ptr->prev;
   }
   
   else if(ptr == NULL && rear == head && c==0){
   
   mark->prev->next = NULL;
   mark->next=mark->prev=NULL;
   mark->next = rear;
   rear->prev = mark;
   head = mark;
   }
   
   else if(c==0){
    
   mark->prev->next = NULL;
   mark->next=mark->prev=NULL;
   rear->next->prev = mark;
   mark->next = rear->next;
   mark->prev = rear;
   rear->next = mark;
   }
   
   }
   else{
   rear = ptr;
   ptr=ptr->next;
   }
   
 }
  
 return 1;
 }
 
 return 0;
}

int string_comparator(void* i1,void* i2){
int i = 0;
if(check_arr == 0){
struct node* ptr = i1;
struct node* rear = i2;

while(ptr->ascii == rear->ascii){
i++;
ptr->ascii = (int)(ptr->str[i]);
rear->ascii = (int)(rear->str[i]);
}

if(ptr->ascii < rear->ascii){

return 0;
}

return 1;
}

char* str1 = i1;
char* str2 = i2;
int ascii_1 = (int)str1[i];
int ascii_2 = (int)str2[i];
while(ascii_1 == ascii_2){
i++;
ascii_1 = (int)(str1[i]);
ascii_2 = (int)(str2[i]);
}

if(ascii_1 < ascii_2){
return 0;
}

return 1;

}

int int_comparator(void* i1, void* i2){
if(check_arr == 0){
struct node* item1 = i1;
struct node* item2 = i2;

if(item1->num < item2->num){

return 1;
}
else if(item1->num == item2->num){

return -1;
}
return 0;
}

int* obj1 = i1;
int* obj2 = i2;
if(*obj1<*obj2){
return 1;
}
else if(*obj1==*obj2){
return -1;
}
return 0;
}

void print_array(){
int i;
for(i=0;i<length;i++){
printf("%d\n",arr[i]);
}
}

void print_str_array(){
int i,j;
for(i=0;i<length;i++){
if(str_arr[i][0] == '\0'){
i++;
} 
    printf("%s\n",str_arr[i]); 
}
}

void free_list(struct node* h){
struct node* current = h;
struct node* next;
while(current != NULL){
next = current->next;
free(current);
current = next;
}

}

int main(int argc, char* argv[]){

int i;
char* s;
char c;
int buff_size=201;
int count = 0;
char* str = (char*)malloc(sizeof(char)*buff_size);
char* str_cpy = (char*)malloc(sizeof(char)*buff_size);
char n;

if(argc<3){
  printf("Fatal Error: expected two arguments, had one\n");  
  exit(0);
}
else if(argc>3){
 printf("Fatal Error: expected two arguments, had %d\n",argc);
 exit(0);
}

if(str == NULL || str_cpy == NULL){

printf("Error: malloc() failed\n");
   free(str);
   free(str_cpy);
   exit(0);
  
}


if(((int)argv[1][1]) != 105 && ((int)argv[1][1])!=113){
  printf("Fatal Error: %s not a valid sort flag\n",argv[1]);
  exit(0);
}
n = argv[1][1];
int fd = open(argv[2], O_RDONLY);

if(fd == -1){
printf("Fatal error: file does not exist\n");
free(str);
free(str_cpy);
close(fd);
exit(0);
}

int j;
int x = 1;
while(read(fd,str,200) != 0){

  i=0;
  j=0;
  while(str[j]!='\0'){
    
    if(str[j] == ',' && str[j+1]!=','){
      if(str_cpy[0]!='\0'){
      store_to_Node(str_cpy,1);
      }
      str_cpy = delete_prev( str_cpy, buff_size);
      i=0;
      j++;
      continue;
    }
    
    if(str[j+1]!='\0' && str[j] == ',' && str[j+1] == ','){
    while(str[j] == ','  || str[j] == '\n' || str[j] == '	' || str[j] == ' '){
     j++;
    }
    j--;
     store_to_Node(str_cpy,1);
     i=0;
     j++;
      str_cpy = delete_prev(str_cpy,buff_size);
      
    continue;
     }
    
    if(str[j+1]!='\0' && (str[j] == ' ' || str[j] == '	' || str[j] == '\n')){
       while(str[j] == ' ' || str[j] == '	' || str[j] == '\n'){
        j++;
      }
      if(j == 200){
      break;
      }
       if(str[j] == ','){
        continue;
       }
      str_cpy[i] = str[j];
      i++;
      j++;
      
      continue;  
    }
    
    else if(str[j+1]=='\0' && (str[j] == ' ' || str[j] == '	' || str[j] == '\n')){
    count=1;
     break;
    }
    
    str_cpy[i] = str[j];
    i++;
  j++;
  }
  if(count==1 && (str_cpy[i-1] == '\0')){
  continue;
  }
 store_to_Node(str_cpy,0);
str = delete_prev( str, buff_size);
str_cpy = delete_prev( str_cpy, buff_size);
  
}
if(str[0] =='\0' && head == NULL){
printf("Warning: empty file\n");
}
free(str);
free(str_cpy);
if(head != NULL){

check_digits(head);

struct node* ptr = head;
i=0;
while(ptr!=NULL){
ptr->index = i;
i++;
ptr=ptr->next;
}

struct node* p = head;
while(p!=NULL){
p->ascii = (int)(p->str[0]);
p = p->next;
}

int y;
switch(n){

case 'i':
check_arr = 0;
  if(head->isDigit == 1){
   int(*comparator)(void*,void*) = int_comparator; 
   y = insertion_sort(head,comparator);
  }
  else{
   int(*comparator)(void*,void*) = string_comparator;
   y = insertion_sort(head,comparator);
   }
   print_list(head);
   free_list(head);
  break;
  
case 'q': 
check_arr = 1;

  if(head->isDigit == 1){
  check_dig = 1;
  make_arr(head);
   int(*comparator)(void*,void*) = int_comparator;
   y = quickSort(arr,comparator);
   print_array();
   free(arr);
   free_list(head);
   }
  else{
  check_dig = 0;
  make_str_arr(head);
   int(*comparator)(void*,void*) = string_comparator;
   y = quickSort(str_arr,comparator);
   print_str_array();
   free(str_arr);
   free_list(head);
  }
  
   break;
  
}

i = close(fd);
if(i == -1){
printf("Error: file not closed\n");
}

}

return 0;
}
