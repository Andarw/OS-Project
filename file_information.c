#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include <fcntl.h>

enum{
    REGULAR_FILE = 1,
    SYMBOLIC_LINK = 2
};

void print_regular_file_menu(char* path){
     printf("regular file %s \n", path);
     printf("Menu for regular file \n");
     printf("1. Read -n \n");
     printf("2. Size -d \n");
     printf("3. Number of hard links -h \n");
     printf("4. Time of last modification -m \n");
     printf("5. Access rights -a \n");
     printf("6. Create a symbolic link give:link name -l \n");
     printf("7. Exit -e \n"); 
}

void print_symbolic_link_menu(char* path){
     printf("symbolic link %s \n", path);
     printf("Menu for symbolic link \n");
     printf("1. Link name -n \n");
     printf("2. Delete link -l \n");
     printf("3. Size of the link -d \n");
     printf("4. Size of the target -t \n");
     printf("5. Access rights -a \n");
     printf("6. Exit -e \n"); 
}

void not_valid_option(int type_of_file, char* path){
            system("clear");
            printf("Not valid option!\n");
            printf("Please enter a valid option from the menu: \n");

            if(type_of_file == 1)
            {
                print_regular_file_menu(path);
            }
            else
            {
                print_symbolic_link_menu(path);
            }
}

void print_menu(int type_of_file, char* path){
    if(type_of_file == 1)
    {
        print_regular_file_menu(path);
    }
    else
    {
        print_symbolic_link_menu(path);
    }
}

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
    int type_of_file = 0;
    bool quit = false;
    strncpy(path,args[1],999);
    
    if(lstat(path,&buff) == -1){
        fprintf(stderr,"The specified path \" %s \" is not reacheable!",path);
        exit(1);
    }

    if (S_ISREG(buff.st_mode))
    {
        print_regular_file_menu(path);
        type_of_file = REGULAR_FILE;
    }
    if(S_ISLNK(buff.st_mode))
    {
        print_symbolic_link_menu(path);
        type_of_file = SYMBOLIC_LINK;
    }

    scanf("%4s",choice);
    printf("choice %s \n",choice);

    while(!quit)
    {
        if(choice[0] == '-' && strlen(choice) == 2)
        {
            if (strchr("nldhmae", choice[1]) && (type_of_file == REGULAR_FILE))
            {
                switch(choice[1])
                 {
                    case 'n':
                        system("clear");
                        printf("------\nFile name : %s\n------\n",path);
                        break;
                    case 'd':
                        system("clear");
                        printf("------\nSize  %ld\n------ \n",buff.st_size);
                        break;
                    case 'h':
                        system("clear");
                        printf("------\nNumber of hard links -h  %ld\n------\n",buff.st_nlink);
                        break;
                    case 'm':
                        system("clear");
                        printf("------\nTime of last modification -m %s\n------\n",ctime(&buff.st_atime));
                        break;
                    case 'a':
                        system("clear");
                        printf("------\nAccess rights: %d,%d\n------\n",buff.st_uid,buff.st_gid);
                        break;
                    case 'l':
                        system("clear");
                        printf("------\nCreate a symbolic link to the file,give the link name:");
                        char lnk[1000];
                        scanf("%999s",lnk);
                        printf("------\n");
                        symlink(path,lnk);
                        break;
                    case 'e':
                        printf("------\nExit\n------\n");
                        quit = true;
                        break;
                    default:
                        break;
                }
                if(!quit)
                {
                    print_menu(type_of_file,path);
                    scanf("%4s",choice);
                }
           }
           else if(strchr("nldtae", choice[1])){
                switch(choice[1])
                {
                    case 'n':
                        system("clear");
                        printf("------\nLink name : %s\n------\n",path);
                        break;
                    case 'l':
                        system("clear");
                        unlink(path);
                        printf("------\nDelete link:%s\n------\n",path);
                        printf("------\nExit\n------\n");
                        quit = true;
                        break;
                    case 'd':
                        system("clear");
                        printf("------\nSize of the link:%ld\n------\n",buff.st_size);
                        break;
                    case 't':
                        system("clear");
                        struct stat buff2;
                        stat(path,&buff2);
                        printf("------\nSize of the target:%ld\n------\n",buff2.st_size);
                        break;
                    case 'a':
                        system("clear");
                        printf("------\nAccess rights: %d,%d\n------\n",buff.st_uid,buff.st_gid);
                        break;
                    case 'e':
                        printf("------\nExit\n------\n");
                        quit = true;
                        break;
                    default:
                        break;
                }
               if(!quit)
                {
                    print_menu(type_of_file,path);
                    scanf("%4s",choice);
                } 
           }
           else
           {
            not_valid_option(type_of_file,path);
            scanf("%4s",choice);
           }
        }
        else 
        {
            not_valid_option(type_of_file,path);
            scanf("%4s",choice);
        }
    }
    return 0;      
}