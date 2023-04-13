#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>

enum // this enum is used to determine the type of file
{
    REGULAR_FILE = 1,
    SYMBOLIC_LINK = 2,
    DIRECTORY = 3
};

void print_regular_file_menu(char *path) // this prints the menu for regular files
{
    printf("->Regular file \"%s\" \n", path);
    printf("Menu for regular file: \n");
    printf("1. Print name -n \n");
    printf("2. Size -d \n");
    printf("3. Number of hard links -h \n");
    printf("4. Time of last modification -m \n");
    printf("5. Access rights -a \n");
    printf("6. Create a symbolic link give:link name -l \n");
    printf("7. Exit -e \n-------------------------\n");
}

void print_symbolic_link_menu(char *path) // this prints the menu for symbolic links
{
    printf("->Symbolic link \"%s\" \n", path);
    printf("Menu for symbolic link: \n");
    printf("1. Link name -n \n");
    printf("2. Delete link -l(this action will result in the program exiting) \n");
    printf("3. Size of the link -d \n");
    printf("4. Size of the target -t \n");
    printf("5. Access rights -a \n");
    printf("6. Exit -e \n\n-------------------------\n");
}

void print_dir_menu(char *path) // this prints the menu for directories
{
    printf("->Directory \"%s\"\n", path);
    printf("Menu for directory: \n");
    printf("1. Directory name -n \n");
    printf("2. Size of the dir -d \n");
    printf("3. Access rights -a \n");
    printf("4. Total number of .C files from directory -c \n");
    printf("5. Exit -e \n\n-------------------------\n");
}

void print_menu(int type_of_file, char *path) // this prints the menu depending on the type of file
{
    if (type_of_file == REGULAR_FILE)
    {
        print_regular_file_menu(path);
    }
    else if (type_of_file == SYMBOLIC_LINK)
    {
        print_symbolic_link_menu(path);
    }
    else
    {
        print_dir_menu(path);
    }
    printf("Enter your choice: ");
}

void not_valid_option(int type_of_file, char *path) // this warns the user that he has introduced an invalid option and prints the menu again
{
    system("clear");
    printf("Not valid option!\n");
    printf("You have introduced an option that is not present in the menu\n");
    printf("Please enter a valid option from the menu: \n");
    print_menu(type_of_file, path);
}

bool is_valid_option(int type_of_file, char *option) // this verifies if the option introduced by the user is valid
{
    int length_option = strlen(option);

    if (option[0] != '-')
    {
        return false;
    }
    if (type_of_file == REGULAR_FILE)
    {
        for (int i = 1; i <= length_option; i++)
        {
            if (!strchr("nldhmae", option[i]))
            {
                return false;
            }
        }
    }
    else if (type_of_file == SYMBOLIC_LINK)
    {
        for (int i = 1; i <= length_option; i++)
        {
            if (!strchr("nldtae", option[i]))
            {
                return false;
            }
        }
    }
    else if (type_of_file == DIRECTORY)
    {
        for (int i = 1; i <= length_option; i++)
        {
            if (!strchr("ndace", option[i]))
            {
                return false;
            }
        }
    }
    return true;
}

void verify_nr_of_args(int argc) // this verifies if the user has introduced the correct number of arguments
{
    if (argc != 2)
    {
        fprintf(stderr, "ERROR!\n");
        fprintf(stderr, "Usage ./p <file/dir/symlnk>");
        exit(1);
    }
}

void check_directory(DIR *dir) // this verifies if the directory was opened successfully and parses over the "." and ".." directories
{
    if (dir == NULL)
    {
        fprintf(stderr, "ERROR, cant open directory!");
        exit(1);
    }
    readdir(dir);
    readdir(dir);
}

void print_acces_rights(uid_t u, gid_t g) // this prints the acces rights in user-friendly format
{
    if(u == 0 && g == 0)
    {
        printf("root,root\n");
    }else
    {
        printf("user with id %d ,group with id %d",u,g);
    }
    
}

int count_c_files_from_dir(DIR *dir, char *path) // this counts the number of .c files from a given directory
{
    int nr_of_c_files = 0;
    struct dirent *entry;
    struct stat buff;
    char file_path[1000];
    while ((entry = readdir(dir)) != NULL)
    {
        strcpy(file_path, path);
        strcat(file_path, "/");
        strcat(file_path, entry->d_name);
        if (lstat(file_path, &buff) == -1)
        {
            fprintf(stderr, "ERROR, cant open file!");
            exit(1);
        }
        if (S_ISREG(buff.st_mode))
        {
            if (strstr(entry->d_name, ".c") != NULL)
            {
                nr_of_c_files++;
            }
        }
    }
    return nr_of_c_files;
}

int main(int argc, char *args[])
{

    verify_nr_of_args(argc);

    struct stat buff;
    struct stat buff2;
    char path[1000];
    char choice[10];
    DIR *dir;
    int type_of_file = 0;
    int i;
    int nr_of_c_files = -1; // is initialized with -1 as the nr of .c files will only be counted once then stored in this variable
    bool quit = false;
    strncpy(path, args[1], 999);

    if (lstat(path, &buff) == -1)
    {
        fprintf(stderr, "The specified path \" %s \" is not reacheable!", path);
        exit(1);
    }

    if (S_ISREG(buff.st_mode))
    {
        type_of_file = REGULAR_FILE;
        print_menu(type_of_file, path);
    }
    else if (S_ISLNK(buff.st_mode))
    {
        type_of_file = SYMBOLIC_LINK;
        print_menu(type_of_file, path);
    }
    else if (S_ISDIR(buff.st_mode))
    {
        type_of_file = DIRECTORY;
        print_menu(type_of_file, path);
        dir = opendir(path);
        check_directory(dir);
    }

    scanf("%7s", choice);
    while (!is_valid_option(type_of_file, choice))
    {
        not_valid_option(type_of_file, path);
        scanf("%7s", choice);
    }

    system("clear");
    printf("choice %s \n", choice);

    int length_choice = strlen(choice);
    while (!quit)
    {
        for (i = 1; i <= length_choice; i++)
        {
            if (type_of_file == REGULAR_FILE)
            {
                switch (choice[i])
                {
                case 'n':
                    printf("------\nFile name : %s\n------\n", path);
                    break;
                case 'd':
                    printf("------\nSize  %ld\n------ \n", buff.st_size);
                    break;
                case 'h':
                    printf("------\nNumber of hard links %ld\n------\n", buff.st_nlink);
                    break;
                case 'm':
                    printf("------\nTime of last modification %s------\n", ctime(&buff.st_atime));
                    break;
                case 'a':
                    printf("------\nAccess rights: %u,%u\n------\n", buff.st_uid, buff.st_gid);
                    print_acces_rights(buff.st_uid, buff.st_gid);
                    printf("\n------\n");
                    break;
                case 'l':
                    printf("------\nCreate a symbolic link to the file,give the link name:");
                    char lnk[1000];
                    scanf("%999s", lnk);
                    printf("------\n");
                    if (symlink(path, lnk) == -1)
                    {
                        fprintf(stderr, "ERROR!\n");
                        fprintf(stderr, "Could not create the symbolic link!");
                        exit(1);
                    }
                    break;
                case 'e':
                    quit = true;
                    break;
                default:
                    break;
                }
            }
            else if (type_of_file == SYMBOLIC_LINK)
            {
                switch (choice[i])
                {
                case 'n':
                    printf("------\nLink name : %s\n------\n", path);
                    break;
                case 'l':
                    unlink(path);
                    printf("------\nDelete link:%s\n------\n", path);
                    quit = true;
                    break;
                case 'd':
                    printf("------\nSize of the link:%ld\n------\n", buff.st_size);
                    break;
                case 't':
                    stat(path, &buff2);
                    printf("------\nSize of the target:%ld\n------\n", buff2.st_size);
                    break;
                case 'a':
                    printf("------\nAccess rights: %u,%u\n------\n", buff.st_uid, buff.st_gid);
                    print_acces_rights(buff.st_uid, buff.st_gid);
                    printf("\n------\n");
                    break;
                case 'e':
                    quit = true;
                    break;
                default:
                    break;
                }
            }
            else if (type_of_file == DIRECTORY)
            {
                switch (choice[i])
                {
                case 'n':
                    printf("------\nDirectory name : %s\n------\n", path);
                    break;
                case 'd':
                    printf("------\nSize of the directory: %ld\n------\n", buff.st_size);
                    break;
                case 'a':
                    printf("------\nAccess rights: %u,%u\n------\n", buff.st_uid, buff.st_gid);
                    print_acces_rights(buff.st_uid, buff.st_gid);
                    printf("\n------\n");
                    break;
                case 'c':
                    if (nr_of_c_files == -1) // if the nr of .c files hasn't been counted yet
                    {
                        nr_of_c_files = count_c_files_from_dir(dir, path);
                    }
                    printf("------\nNumber of C files in the directory: %d\n------\n", nr_of_c_files);
                    break;
                case 'e':
                    quit = true;
                    break;
                default:
                    break;
                }
            }
        }
        if (!quit)
        {   // if the user didn't choose to exit
            print_menu(type_of_file, path);
            scanf("%7s", choice);
            while (!is_valid_option(type_of_file, choice))
            {
                not_valid_option(type_of_file, path);
                scanf("%7s", choice);
            }
            length_choice = strlen(choice);
            system("clear");
            printf("choice %s \n", choice);
        }
    }
    printf("------\n Exit \n------\n");

    return 0;
}
