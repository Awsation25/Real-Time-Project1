#include <stdio.h>
//
// Created by  Mohammad Khdour on 25/03/2025.
//
#include "header.h"
int main(int args , char** argc)
{
    printf("hello from child %d get %s\n",getpid(),argc[0]);
    printf("num of arg %d",args);
    int des[2];


    if(pipe(des)==-1)
    {
        perror("pipe error");
    }

    pid_t child_pid = fork();


     if(child_pid == -1)
    {
        perror("fork error no memory");
    }
     else if(child_pid == 0)
     {
         char message[BUFSIZ];
         printf("i am in child\n");
         close(des[1]);
         if(read(des[0],message,BUFSIZ)!= -1)
         {
             printf("the message is reached from the pipe is %s\n", message);
         }
         else
         {
             perror("read error");
         }
     }

    else
    {
        close(des[0]);
        char str[10] = "mohammad";
        //snprintf(str, sizeof(str), "%s", argc[1]);  // Copy argument safely
        if(write(des[1],argc[0],strlen(argc[0])+1) != -1)
        {
            printf("message sent\n");
        }
        else
        {
            perror("write error");
        }

    }
    }




