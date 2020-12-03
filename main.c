#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

int ls_cd(char** argv){
   if(argv[1]==NULL){
     fprintf(stderr, "ACM-SH: expected argument to \"cd\"\n");
   }
   else{
   if(chdir(argv[1])!=0)
   perror("lsh");
  
   }
   return 1;
}



int ls_exit(char** argv){
return 0;
}

char* builtin_str[] = {
 "cd",  "exit"
};

int builtin_size(){
  return sizeof(builtin_str)/sizeof(char*);
}

int (*builtin_fun[])(char**) = {
&ls_cd,
&ls_exit
};

char* ls_read_line(void){
  char* line = NULL;
  ssize_t buff = 0;
  if(getline(&line, &buff, stdin)==-1){
  if(feof(stdin)){
    exit(EXIT_SUCCESS);
  }
  else{
  perror("readline");
  exit(EXIT_FAILURE);
  }
  }
  return line;
}

#define ls_buff_size 64
#define ls_delim " \t\r\n\a"
char** ls_split_line(char* line){
   int position = 0;
   int buff = ls_buff_size;
   char** tokens = malloc(buff*sizeof(char*));
   if(!tokens){
   fprintf(stderr, "ACM-SH: allocation error\n");
   exit(EXIT_FAILURE);
   }
   char* token = strtok(line, ls_delim);
   while(token!=NULL){
   tokens[position] = token;
   position++;
   if(position>=buff){
   buff = buff+ls_buff_size;
   tokens = realloc(tokens, buff*sizeof(char*));
   if(!tokens){
   fprintf(stderr, "ACM-SH: allocation error\n");
   exit(EXIT_FAILURE);
   }
   }
   token = strtok(NULL, ls_delim);
   }
   tokens[position] = NULL;
   return tokens;
}

int ls_launch(char** argv){
  pid_t pid = fork();
  pid_t wpid;
  int status;
  if(pid==0){
  if(execvp(argv[0], argv)==-1) {
  perror("lsh");
  }
  exit(EXIT_FAILURE);
  }
  else if(pid<0){
  perror("lsh");
  }
  else{
  do{
  wpid = waitpid(pid, &status, WUNTRACED);
  }
  while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int ls_execute(char** argv)
{
int i;
if(argv[0]==NULL){
return 1;
}
for(i = 0; i<builtin_size(); i++){
if(strcmp(argv[0] , builtin_str[i])==0)
return (*builtin_fun[i])(argv);
}
return ls_launch(argv);
}

int main(int argc, char** argv){
   int status;
   char* line;
   char** args;
   do{
   printf("ACM-SH: > ");
   line = ls_read_line();
   args  = ls_split_line(line);
   status = ls_execute(args);
   free(line);
   free(args);
   }
   while(status);
   return EXIT_SUCCESS;
}

