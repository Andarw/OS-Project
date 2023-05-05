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
#include <regex.h>
#include <sys/wait.h>
#include <stdint.h>

int count_process = 0;      // this variable is used to count the number of processes
int pfd[2];                 // this is the pipe used for communication between the parent and the childS
bool already_duped = false; // this variable is used to check if the pipe has already been duped

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
    // printf("7. Exit -e \n-------------------------\n");
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
}

void print_dir_menu(char *path) // this prints the menu for directories
{
    printf("->Directory \"%s\"\n", path);
    printf("Menu for directory: \n");
    printf("1. Directory name -n \n");
    printf("2. Size of the dir -d \n");
    printf("3. Access rights -a \n");
    printf("4. Total number of .C files from directory -c \n");
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
            if (!strchr("nldhma", option[i]))
            {
                return false;
            }
        }
    }
    else if (type_of_file == SYMBOLIC_LINK)
    {
        for (int i = 1; i <= length_option; i++)
        {
            if (!strchr("nldta", option[i]))
            {
                return false;
            }
        }
    }
    else if (type_of_file == DIRECTORY)
    {
        for (int i = 1; i <= length_option; i++)
        {
            if (!strchr("ndac", option[i]))
            {
                return false;
            }
        }
    }
    return true;
}

void verify_nr_of_args(int argc) // this verifies if the user has introduced the correct number of arguments
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR!\n");
        fprintf(stderr, "Usage ./p <file/dir/symlnk>, the program needs at least one argument!\n");
        exit(1);
    }
}

DIR *open_DIR(char *path, int type_of_file) // this returns a pointer to the directory after skipping the "." and ".." entries, if the path is not a directory it returns NULL
{
    if (type_of_file != DIRECTORY)
    {
        return NULL;
    }
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        fprintf(stderr, "ERROR, cant open directory!");
        exit(1);
    }
    readdir(dir);
    readdir(dir);
    return dir;
}

void print_acces_rights(struct stat buff) // this prints the acces rights in user-friendly format
{
    printf("Access rights for owner: \n \n");
    if (buff.st_mode & S_IRUSR)
        printf("Read permission for owner\n");
    if (buff.st_mode & S_IWUSR)
        printf("Write permission for owner\n");
    if (buff.st_mode & S_IXUSR)
        printf("Execute permission for owner\n");
    printf("----------------------\nAccess rights for group: \n \n");
    if (buff.st_mode & S_IRGRP)
        printf("Read permission for group\n");
    if (buff.st_mode & S_IWGRP)
        printf("Write permission for group\n");
    if (buff.st_mode & S_IXGRP)
        printf("Execute permission for group\n");
    printf("----------------------\nAccess rights for others: \n \n");
    if (buff.st_mode & S_IROTH)
        printf("Read permission for others\n");
    if (buff.st_mode & S_IWOTH)
        printf("Write permission for others\n");
    if (buff.st_mode & S_IXOTH)
        printf("Execute permission for others\n");
}

int count_c_files_from_dir(DIR *dir, char *path) // this counts the number of .c files from a given directory
{
    int nr_of_c_files = 0;
    struct dirent *entry;
    struct stat buff;
    char file_path[1000];
    regex_t regex;
    if (regcomp(&regex, ".c$", REG_EXTENDED))
    {
        fprintf(stderr, "ERROR, cant compile regex!");
        exit(1);
    }
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
            if (regexec(&regex, entry->d_name, 0, NULL, 0) == 0)
            {
                nr_of_c_files++;
            }
        }
    }
    return nr_of_c_files;
}

int type_of_entry(struct stat buff) // this retruns the type of the entry{regular file, symbolic link, directory}
{
    if (S_ISREG(buff.st_mode))
    {
        return REGULAR_FILE;
    }
    else if (S_ISLNK(buff.st_mode))
    {
        return SYMBOLIC_LINK;
    }
    else if (S_ISDIR(buff.st_mode))
    {
        return DIRECTORY;
    }
    else
    {
        return -1;
    }
}

int read_choice(char *choice, struct stat buff, char *path) // this reads the choice and verify if it is valid and returns the length of the choice
{
    scanf("%7s", choice);
    while (!is_valid_option(type_of_entry(buff), choice))
    {
        not_valid_option(type_of_entry(buff), path);
        scanf("%7s", choice);
    }
    system("clear");
    printf("choice %s \n", choice);
    return strlen(choice);
}

void compile_if_C_file(char *path, int type_of_file) // this compiles the file if it is a .c file and prints the number of errors and warnings to the screen
{
    if (type_of_file != REGULAR_FILE)
    {
        return;
    }
    regex_t regex;
    if (regcomp(&regex, ".c$", REG_EXTENDED))
    {
        fprintf(stderr, "ERROR, cant compile regex!");
        exit(1);
    }
    if (regexec(&regex, path, 0, NULL, 0) == 0)
    {
        count_process++;
        pid_t pid = fork();
        if (pid == -1)
        {
            fprintf(stderr, "ERROR, cant fork!");
            exit(1);
        }
        if (pid == 0)
        {
            close(pfd[0]);

            if (!already_duped)
            {
                dup2(pfd[1], 1);
                already_duped = 1;
            }
            close(pfd[1]);
            execlp("sh", "sh", "compile_C_file.sh", path, NULL);
            fprintf(stderr, "ERROR, cant execute script!");
            exit(1);
        }
    }
}

void create_file_if_dir(char *path, int type_of_file) // this creates a file with the name of the entry if it is a directory
{
    if (type_of_file != DIRECTORY)
    {
        return;
    }
    count_process++;
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "ERROR, cant fork!");
        exit(1);
    }
    if (pid == 0)
    {
        char fct_path[1000];
        strcpy(fct_path, path);
        strcat(fct_path, "_file.txt\0");
        execlp("sh", "sh", "create_file.sh", fct_path, NULL);
        fprintf(stderr, "ERROR, cant execute script!");
        exit(1);
    }
}

void print_score(char *path, char *buffer) // this function retrevies the components of the score, computes the score and prints it in the file grades.txt
{
    int score = 0;
    int nr_of_errors = 0;
    int nr_of_warnings = 0;
    char *token = strtok(buffer, " \n");
    nr_of_errors = atoi(token);
    token = strtok(NULL, " \0\n");
    nr_of_warnings = atoi(token);
    if (nr_of_errors == 0)
    {
        if (nr_of_warnings > 10)
        {
            score = 2;
        }
        else if (nr_of_warnings > 0)
        {
            score = 2 + 8 * (10 - nr_of_warnings) / 10;
        }
        else
        {
            score = 10;
        }
    }
    else
    {
        score = 1;
    }
    FILE *fptr;
    fptr = fopen("grades.txt", "a");
    fprintf(fptr, "%s: %d\n", path, score);
    fclose(fptr);
}

int main(int argc, char *args[])
{

    verify_nr_of_args(argc);

    struct stat buff;
    struct stat buff2;
    char path[1000];
    char choice[10];
    DIR *dir;
    int i;
    int type_of_file;
    int nr_of_c_files;

    for (int j = 1; j < argc; j++)
    {
        if (pipe(pfd) < 0)
        {
            fprintf(stderr, "ERROR, cant create pipe!");
            exit(1);
        }
        count_process++;
        type_of_file = 0;
        nr_of_c_files = -1; // is initialized with -1 as the nr of .c files will only be counted once then stored in this variable

        strncpy(path, args[j], 999);

        if (lstat(path, &buff) == -1)
        {
            fprintf(stderr, "The specified path \" %s \" is not reacheable!", path);
            exit(1);
        }

        type_of_file = type_of_entry(buff);

        dir = open_DIR(path, type_of_file); // if the file is not a directory, dir will be NULL

        compile_if_C_file(path, type_of_file);

        char buffer[512] = {0}; // this buffer will be used to read the output of the script compile_C_file.sh
        close(pfd[1]);
        if (read(pfd[0], buffer, 512))
        {
            printf("buffer: %s\n", buffer);
            print_score(path, buffer);
        }

        create_file_if_dir(path, type_of_file);

        sleep(2);
        print_menu(type_of_file, path);

        int length_choice = read_choice(choice, buff, path);

        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Could not create child process!");
            exit(1);
        }
        if (pid == 0)
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
                        print_acces_rights(buff);
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
                        print_acces_rights(buff);
                        printf("\n------\n");
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
                        print_acces_rights(buff);
                        printf("\n------\n");
                        break;
                    case 'c':
                        if (nr_of_c_files == -1) // if the nr of .c files hasn't been counted yet
                        {
                            nr_of_c_files = count_c_files_from_dir(dir, path);
                        }
                        printf("------\nNumber of C files in the directory: %d\n------\n", nr_of_c_files);
                        break;
                    default:
                        break;
                    }
                }
            }
            exit(0);
        }
    }
    close(pfd[0]);
    int status;
    int w;
    for (int i = 0; i < count_process; i++)
    {
        w = wait(&status);

        if (w == -1)
        {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
        {
            printf("exited, status=%d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}