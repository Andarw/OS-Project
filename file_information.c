#include<stdio.h>
#include <errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argv,char *args[])
{

    if(argv > 2){
        fprintf(stderr,"Too many arguments!\n");
        fprintf(stderr,"Usage p <>");
        exit(1);
    }

    struct stat buff;
    char path[1000];
    char choice[5];
    strncpy(path,args[1],999);
    
    if(lstat(path,&buff) == -1){
        fprintf(stderr,"The specified path \" %s \" is not reacheable!",path);
        exit(1);
    }

    if (S_ISREG(buff.st_mode))
    {
     printf("regular file %s \n", path);
     printf("Menu for regular file \n");
     printf("1. Read -n \n");
     printf("2. Size -d \n");
     printf("3. Number of hard links -h \n");
     printf("4. Time of last modification -m \n");
     printf("5. Access rights -a \n");
     printf("6. Create a symbolic link give:link name -l \n");   
    }
    if(S_ISLNK(buff.st_mode))
    {
     printf("symbolic link %s \n", path);
     printf("Menu for symbolic link \n");
     printf("1. Link name -n \n");
     printf("2. Delete link -l \n");
     printf("3. Size of the link -d \n");
     printf("4. Size of the target -t \n");
     printf("5. Access rights -a \n");
    }
    scanf("%s",choice);
    int i = 0;
    while(choice[i] != '\0')
           {
                if(choice[0] == '-')
                {
                    if ( i!=0 && strchr("nldhma", choice[i]))
                    {
                        switch(choice[i])
                        {
                            case 'n':
                                printf("File name : %s \n",path);
                                break;
                            case 'd':
                                printf("Size  %ld \n",buff.st_size);
                                break;
                            case 'h':
                                printf("Number of hard links -h  %ld\n",buff.st_nlink);
                                break;
                            case 'm':
                                printf("Time of last modification -m %ld \n",buff.st_mtime);
                                break;
                            case 'a':
                                printf("Access rights -a \n");
                                break;
                            case 'l':
                                printf("Create a symbolic link give:link name -l \n");
                                break;
                            default:
                                printf("Invalid option \n");
                                break;
                        }
                    }
                }
                else 
                {
                    printf("Not valid option");
                }
           }
    return 0;
           
}