#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<fcntl.h>

enum{
    REGULAR_FILE = 1,
    SYMBOLIC_LINK = 2,
    DIRECTORY = 3
};

void print_regular_file_menu(char* path){
     printf("Regular file %s \n", path);
     printf("Menu for regular file \n");
     printf("1. Print name -n \n");
     printf("2. Size -d \n");
     printf("3. Number of hard links -h \n");
     printf("4. Time of last modification -m \n");
     printf("5. Access rights -a \n");
     printf("6. Create a symbolic link give:link name -l \n");
     printf("7. Exit -e \n"); 
}

void print_symbolic_link_menu(char* path){
     printf("Symbolic link %s \n", path);
     printf("Menu for symbolic link \n");
     printf("1. Link name -n \n");
     printf("2. Delete link -l \n");
     printf("3. Size of the link -d \n");
     printf("4. Size of the target -t \n");
     printf("5. Access rights -a \n");
     printf("6. Exit -e \n"); 
}

void print_dir_menu(char* path){
    printf("Directory %s",path);
    printf("1. Directory name -n \n");
    printf("2. Size of the dir -d \n");
    printf("3. Access rights -a \n");
    printf("4. Total number of .C files from directory -c \n");
    printf("5. Exit -e \n");
}

void print_menu(int type_of_file, char* path){
    if(type_of_file == REGULAR_FILE)
    {
        print_regular_file_menu(path);
    }
    else if(type_of_file == SYMBOLIC_LINK)
    {
        print_symbolic_link_menu(path);
    }
    else
    {
        print_dir_menu(path);
    }
}

void not_valid_option(int type_of_file, char* path){
            system("clear");
            printf("Not valid option!\n");
            printf("You have introduced an option that is not present in the menu\n");
            printf("Please enter a valid option from the menu: \n");
            print_menu(type_of_file,path);
}

int main(int argv,char *args[])
{

    if(argv > 2){
        fprintf(stderr,"Too many arguments!\n");
        fprintf(stderr,"Usage p <>");
        exit(1);
    }

    struct stat buff;
    struct stat buff2;
    char path[1000];
    char choice[10];
    int type_of_file = 0;
    bool quit = false;
    strncpy(path,args[1],999);
    
    if(lstat(path,&buff) == -1){
        fprintf(stderr,"The specified path \" %s \" is not reacheable!",path);
        exit(1);
    }

    if (S_ISREG(buff.st_mode))
    {
        type_of_file = REGULAR_FILE;
    }
    else if(S_ISLNK(buff.st_mode))
    {
        type_of_file = SYMBOLIC_LINK;
    }
    else if(S_ISDIR(buff.st_mode)){
        type_of_file = DIRECTORY;

        
    }

    print_menu(type_of_file,path);
    scanf("%7s",choice);
    
    printf("choice %s \n",choice);
    int i = 0;
    while(!quit && choice[i] != '\0')
    {
        if(choice[0] == '-')
        {
            if (strchr("nldhmae", choice[i]) && (type_of_file == REGULAR_FILE))
            {
                switch(choice[1])
                 {
                    case 'n':
                        printf("------\nFile name : %s\n------\n",path);
                        break;
                    case 'd':
                        printf("------\nSize  %ld\n------ \n",buff.st_size);
                        break;
                    case 'h':
                        printf("------\nNumber of hard links -h  %ld\n------\n",buff.st_nlink);
                        break;
                    case 'm':
                        printf("------\nTime of last modification -m %s\n------\n",ctime(&buff.st_atime));
                        break;
                    case 'a':
                        printf("------\nAccess rights: %d,%d\n------\n",buff.st_uid,buff.st_gid);
                        break;
                    case 'l':
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
           else if(strchr("nldtae", choice[i]) && type_of_file == SYMBOLIC_LINK){
                switch(choice[i])
                {
                    case 'n':
                        printf("------\nLink name : %s\n------\n",path);
                        break;
                    case 'l':
                        unlink(path);
                        printf("------\nDelete link:%s\n------\n",path);
                        printf("------\nExit\n------\n");
                        quit = true;
                        break;
                    case 'd':
                        printf("------\nSize of the link:%ld\n------\n",buff.st_size);
                        break;
                    case 't':
                        stat(path,&buff2);
                        printf("------\nSize of the target:%ld\n------\n",buff2.st_size);
                        break;
                    case 'a':
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
           else if(strchr("nldtae", choice[i]) && type_of_file == DIRECTORY)
           {
                switch (choice[i])
                {
                    case 'n':
                        printf("------\nDirectory name : %s\n------\n",path);
                        break;
                    case 'd':
                        printf("------\nSize of the directory: %ld\n------\n",buff.st_size);
                        break;
                    
                
                    break;
                
                default:
                    break;
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