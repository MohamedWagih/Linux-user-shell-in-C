#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>


//global variables
int in =0, out=0;
char outputfile[20];
char* argsout [1024];
int length = 0;
//------------

char **takeInput(){

    int index = 0;
    char **command =(char**) calloc(1024,sizeof(char));

    int k =0;
    char buffer [1024];

    int dquoted =0;
    int squoted =0;

    int c;
    printf("sish:> ");
    while(1){

        c = getchar();

        if (c == '\n'){


            if (buffer[0] == '\0' ||buffer[0] == '\t'){
                strcpy(buffer,"\0");
                length = index;
                return command;

            }
            buffer[k] = '\0';
            command[index] = (char*)malloc(sizeof(char) * strlen(buffer));
            strcpy(command[index],buffer);
            strcpy(buffer,"\0");
            length = index+1;
            return command;
        }
        else if (c == '"' && !squoted){
            if(dquoted == 0)
                dquoted = 1;
            else
                dquoted = 0;
        }
        else if (c == 39 &&! dquoted){
            if(squoted == 0)
                squoted = 1;
            else
                squoted = 0;
        }
        else if ((c ==' ' ||c == '\t')&& !squoted &&!dquoted){
            if (buffer[0] == '\0'){
                k = 0;
                continue;
            }
            buffer[k] = '\0';

            command[index] = (char*)malloc(sizeof(char) * strlen(buffer));
            strcpy(command[index],buffer);
            strcpy(buffer,"\0");
            index++;
            k = 0;
        }
        else if ((c =='<'||c =='>'||c =='|'||c =='&')&& !squoted &&!dquoted){
                if(strlen(buffer) == 0){
                buffer[k] = c;
                k++;
                }
                else{
                    buffer[k] = '\0';
                    command[index] = (char*)malloc(sizeof(char) * strlen(buffer));
                    strcpy(command[index],buffer);
                    strcpy(buffer,"\0");
                    index++;
                    k = 0;
                    buffer[k] = c;
                    buffer[k+1] = '\0';
                    command[index] = (char*)malloc(sizeof(char) * strlen(buffer));
                    strcpy(command[index],buffer);
                    strcpy(buffer,"\0");
                    index++;
                    k = 0;
                }

        }
        else{
            buffer[k] = c;
            k++;
        }
    }


}



int prcs_start(char **args , int background){
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if(out){
        out =0;
        int fd_output = open(outputfile,O_WRONLY|O_CREAT, 0644);
        dup2(fd_output,STDOUT_FILENO);
        close (fd_output);
        out =0;
    }
    if (execvp(args[0], args) == -1) {
          perror("ERROR");
        }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("ERROR");
  } else {
    
    // Parent process
    if (!background){
      do {
        wpid = waitpid(pid, &status, WUNTRACED);
      } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if(out){
            out =0;
        }
    }
}
  return 1;
}


int main(){
        int background = 0;
        char **tokens;
        while(1){
            background = 0;
            tokens = takeInput();
            if (!tokens[0]){
                continue;
            }
            if ((!tokens[1] &&  !strcmp(tokens[0],"exit") )){
                return 0;
            }
            
            //background check
            if (length>1){
              if (!strcmp(tokens[length -1],"&")){
                background = 1;
                tokens[length -1]= NULL;
                }
              }
              
              
            int j = 0;
            //file I/O check
            while(tokens[j]){
                  //printf("%s\n", tokens[j]);
                  if (!strcmp(tokens[j], ">")){
                    //printf("ouuuuuuut\n");
                    out=1;
                    strcpy(outputfile, tokens[j+1]);
                    break;
                    //printf("%s\n", outputfile);
                  }
                  argsout[j] = tokens[j];
                  j++;
            }  
              
            //run
        if(out){
            printf("out to a file: \n");
            prcs_start(argsout,background);
        } else{
            printf("out to screen: \n");
            prcs_start(tokens,background);
        }
            
        }
return 0;
}
