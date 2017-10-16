#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int in =0, out=0;
char outputfile[20];
char* argsout [1024];
int main()
{
    while(1){
        char *token;
        char* tokens[1024] = { NULL };
        char input[1024];
        //int c = NULL;
        int  i=0, j=0;
        const char s[2] = " ";
        printf("sish:>");
        //take input
        int c = getchar();
        if (c == '\n'  || c =='\r'|| c =='\a'|| c =='\t' || c == ' '){
            continue;
        }
        else{
            input[0] = c;
                }
        scanf("%[^\n]%*c", input+1);
        if (!strcmp(input,"exit")){
            return 0;
        }

        /* get the first token */
        token = strtok(input, s);
        tokens[i] = token;
        i++;

        /* walk through other tokens */
        while( token != NULL ){
            token = strtok(NULL, s);
            tokens[i] = token;
            i++;
        }

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
         //printf("%s\n", tokens[1]);

        if(out){
            printf("out to a file: \n");
            prcs_start(argsout);
        } else{
            printf("out to screen: \n");
            prcs_start(tokens);
        }

    }
    return(0);
}




int prcs_start(char **args){

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
    //close (fd_output);



    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("ERROR");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    if(out){
        out =0;
    }
  }

  return 1;
}

