#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// history buffer
#define HISTORY_SIZE 100
char *history[HISTORY_SIZE];
int history_count = 0;

/*
  Function Declarations for builtin shell commands:
  Added since: "lsh_help() uses the array of builtins, and the arrays contain lsh_help()"
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_pwd(char **args);
int lsh_echo(char **args);
int lsh_clear(char ** args);
int lsh_history(char **args);
int lsh_whoami(char **args);
int lsh_mkdir(char **args);
int lsh_rmdir(char **args);
int lsh_touch(char **args);
int lsh_cat(char **args);
int lsh_clearhistory(char **args);
int lsh_ls(char **args);
int lsh_delete(char **args);
int lsh_mydate(char **args);
int lsh_rdelete(char **args);

// array of strings containing built-in shell commands
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "echo",
    "clear",
    "history",
    "whoami",
    "mkdir",
    "rmdir",
    "touch",
    "cat",
    "clearhistory",
    "ls",
    "delete",
    "mydate",
    "rdelete"
};

// an array of function pointers, each one mapping to a function that uses a built-in shell command
// function pointers: pointers pointing to function address in memory
// functions take in string array and return an int
int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_pwd,
    &lsh_echo,
    &lsh_clear,
    &lsh_history,
    &lsh_whoami,
    &lsh_mkdir,
    &lsh_rmdir,
    &lsh_touch,
    &lsh_cat,
    &lsh_clearhistory,
    &lsh_ls, 
    &lsh_delete, 
    &lsh_mydate,
    &lsh_rdelete
};

// returns # of built-in commands
int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// for native cd command
int lsh_cd(char **args)
{
    // to check if second argument exists (ex: cd file, cd directory/file)
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        // if the directory user wants to change to is not valid, print out error message
        if (chdir(args[1]) != 0) {
        perror("lsh");
        }
    }
    
    // if the directory is valid, tell the shell to keep on running
    return 1;
}

// for native help command
int lsh_help(char **args)
    {
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are included:\n");

    // lists all of the built-in shell commands
    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");

    return 1;
}

// return working directory using getcwd()
int lsh_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("lsh");

    return 1;
}

// echo command
int lsh_echo(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");

    return 1;
}

// prints ANSI escape code
int lsh_clear(char **args) {
    printf("\033[H\033[J");
    fflush(stdout);
    return 1;
}

// print out command history
int lsh_history(char **args) {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s", i + 1, history[i]);
    }

    return 1;
}

// whoami command using getlogin()
int lsh_whoami(char **args) {
    char *username = getlogin();
    if (!username) username = getenv("USER");
    if (username)
        printf("%s\n", username);
    else
        fprintf(stderr, "whoami: cannot determine user\n");
    return 1;
}

// create directory using mkdir()
int lsh_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkdir: missing operand\n");
        return 1;
    }
    // 0755 → owner: 7 | group: 5 | others: 5
    // 7 = 4 (read) + 2 (write) + 1 (execute) (cd into dir needs execute permission)
    // 5 = 4 + 1 = read + execute
    if (mkdir(args[1], 0755) != 0)
        perror("mkdir");

    return 1;
}

// create files using touch 
int lsh_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "touch: missing file operand\n");
        return 1;
    }

    // open() needs filename, file action, file permission
    int fd = open(args[1], O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("touch");
        return 1;
    }

    // update timestamp (like real touch)
    futimens(fd, NULL);

    close(fd);
    return 1;
}

// remove directory using rmdir()
int lsh_rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rmdir: missing operand\n");
        return 1;
    }
    if (rmdir(args[1]) != 0)
        perror("rmdir");

    return 1;
}

// cat command
int lsh_cat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cat: missing file\n");
        return 1;
    }

    // open file
    FILE *fp = fopen(args[1], "r");
    // if file DNE
    if (!fp) {
        fprintf(stderr, "cat: %s: ", args[1]);
        perror("No such file or directory");
        return 1;
    }   

    // read file -> write char to standard output stream (maps to int)
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }
    // close file
    fclose(fp);

    return 1;
}

// for clearing history 
int lsh_clearhistory(char **args) {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);     // free each command string
        history[i] = NULL;    // just to be safe
    }
    history_count = 0;        // reset counter
    printf("History cleared.\n");
    return 1;
}

// to list files in current dir
int lsh_ls(char **args) {
    DIR *d = opendir(".");
    if (!d) {
        perror("listfiles");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(d);
    return 1;
}

// delete command with user choice verification
int lsh_delete(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "delete: missing file operand\n");
        return 1;
    }

    char choice;
    printf("Are you sure you want to delete '%s'? (y/n): ", args[1]);
    scanf(" %c", &choice);
    while (getchar() != '\n'); // clear stdin

    if (choice == 'y' || choice == 'Y') {
        if (remove(args[1]) != 0) {
            perror("delete");
        } else {
            printf("'%s' deleted.\n", args[1]);
        }
    } else {
        printf("Delete cancelled.\n");
    }

    return 1;
}

// command telling date
int lsh_mydate(char **args) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("Current date/time: %04d-%02d-%02d %02d:%02d:%02d\n",
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
           tm.tm_hour, tm.tm_min, tm.tm_sec);
    return 1;
}

// Recursive helper function for rdelete
int rdelete_recursive(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {       // Check if path exists
        perror("rdelete");
        return -1;
    }

    // If directory
    if (S_ISDIR(st.st_mode)) {  
        // open it      
        DIR *d = opendir(path);
        if (!d) return -1;

        struct dirent *entry;
        while ((entry = readdir(d)) != NULL) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                char newpath[1024];
                snprintf(newpath, sizeof(newpath), "%s/%s", path, entry->d_name);
                rdelete_recursive(newpath);  // Recursive call for each entry
            }
        }
        // close it
        closedir(d);

        // Remove directory itself
        if (rmdir(path) != 0) {        
            perror("rdelete");
            return -1;
        }
    // If regular file
    } else {                            
        if (remove(path) != 0) {
            perror("rdelete");
            return -1;
        }
    }

    return 0;
}

// lsh_rdelete command
int lsh_rdelete(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rdelete: missing directory operand\n");
        return 1;
    }

    char choice;
    printf("Are you sure you want to recursively delete '%s'? (y/n): ", args[1]);
    scanf(" %c", &choice);
    while (getchar() != '\n'); // clear stdin

    if (choice == 'y' || choice == 'Y') {
        if (rdelete_recursive(args[1]) == 0) {
            printf("'%s' deleted recursively.\n", args[1]);
        }
    } else {
        printf("Recursive delete cancelled.\n");
    }

    return 1;
}


// if the native exit command is executed, exit the shell program
int lsh_exit(char **args)
{
    return 0;
}

// takes list of arguments (token array) -> forks the process -> saves return value
// shells start processes (programs in execution that have a unique PID #), which the kernel controls/manages. Very interesting how it works.
// A process is made up of an executable program, it's data (with program ptr) and stack (with stack ptr), CPU registers which can be shared between parent and child processes, etc. 
// the shell itself is a process that creates child processes to execute commands and loops indefinitely, also interesting how system calls work to manipulate processes for shell functionality
int lsh_launch(char **args)
{
    pid_t pid, wpid;                                        // stores processID (PID) returned by fork(), stores PID returned by waitpid()
    int status;                                             // stores the exit status of the child process 

    // Creates a new process (child process) by duplicating current process (the parent/shell process)
    // Will either return:
    // 1. 0: we are in the child process
    // 2. PID of child process (positive value): returns to parent process or caller after sucessfully creating child process
    // 3. negative value: returns error
    pid = fork();
    if (pid == 0) {
        // Child process
        // if we are in child process we want to run command given by user through exec() system call
        // calls exec() variant to test if first argument in array is a proper command, if not and it returns, print error message
        if (execvp(args[0], args) == -1) {
        perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking (the fork() call failed)
        // up to user to decide whether or not to continue, could add exit status here as well 
        perror("lsh");
    } else {
        // Parent process
        // if none of above, we are in parent process and wait (using waitpid(), a variant of wait system call) for child process to finish
        // useful so that shell does not execute next command until after current one finishes executing
        // WIFEXITED(status) = child process exits normally, WIFSIGNALED(status) = child process terminated by signal
        do {
        wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    // indicates that shell should continue running
    return 1;
}

// execute the given arguments (char** args == char** tokens)
int lsh_execute(char **args)
{
    int i;

    // if first argument is empty (user enters whitespace and/or hits enter key)
    if (args[0] == NULL) {
        // An empty command was entered. 
        return 1;
    }

    // loop through the built-in command array to find a match for command entered in by the user (usually at args[0])
    // if a match is found then the corresponding function pointer is called with arguments array as parameter for function
    // example: user enters "cd" -> builtin_func[i] points to lsh_cd -> lsh_cd(args) is called/executed
    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
        return (*builtin_func[i])(args);
        }
    }

    // if the command user enters is not one built into the shell, it calls lsh_launch() to launch process
    return lsh_launch(args);
}

/*
// same purpose as function below, but without using getline() (a line input function)
// initial size of buffer used to store user input (1 kilobyte or 1024 bytes)
#define LSH_RL_BUFSIZE 1024
// reads a line of input from the user and dynamically resizes the buffer if necessary (if input exceeds initial size)
char *lsh_read_line(void)
{
 
    int bufsize = LSH_RL_BUFSIZE;                           // buffer size (assigned to variable since it can change)
    int position = 0;                                       // current position in buffer
    char *buffer = malloc(sizeof(char) * bufsize);          // allocate memory for buffer                          
    int c;                                                  // stores char from input as ASCII value (an int)

    // if memory allocation fails, error msg printed to stderr and exit program with a failure status
    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character from standard input (the terminal)
        c = getchar();

        // If we hit EOF or a newline, replace it with a null character and return.
        // else buffer at that current position is assigned the character value 
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;                                         // go to next position in buffer by incrementing

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;                      // increase buffer size by 1024 bytes
            buffer = realloc(buffer, bufsize);              // use this new buffer size as parameter in realloc()
            // always safe to include this to prevent any errors if memory cannot be allocated
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);                         // exit program with a failure status
            }
        }
    }
} */

// for reading a line of input from user using getline()
char *lsh_read_line(void)
{
    char *line = NULL;                                      // where to store the users input line, first initialize to NULL
    ssize_t bufsize = 0;                                    // have getline allocate a buffer for us
  
    if (getline(&line, &bufsize, stdin) == -1){             // when getline() == -1, either error has occured or end of input (EOF) has been reached
        if (feof(stdin)) {       // checks if EOF was reached 
            exit(EXIT_SUCCESS);  // We recieved an EOF (line has been successfully stored)
        } else  {
            perror("readline");  // if there was an error
            exit(EXIT_FAILURE);  // exit program with failure status
        }
    }
  
    return line;                                            // return the line
}


// for parsing through the stored user input line, and separating it into a list of arguments (token array) using strtok()
// buffer size macro for storing tokens (can initially hold up to 64 tokens)
#define LSH_TOK_BUFSIZE 64
// macro defining what delimiters are valid for splitting up the input line (will be used as an argument of strtok())
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;            // bufsize = current size of tokens array, position = current location in tokens array (the tokens array is where we will be storing our args)
    char **tokens = malloc(bufsize * sizeof(char*));        // allocate memory for tokens array
    char *token;                                            // a token (arg) strtok() will point to 

    // checks if the memory allocation for tokens array failed
    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    // splits the line into tokens (parses the line) based on the delimiter's
    // first call returns first token within input line
    token = strtok(line, LSH_TOK_DELIM);
    // while ptr returned by strtok does not reach end of string line
    while (token != NULL) {
        // insert the pointer to token in the tokens array and move to next empty position
        tokens[position] = token;
        position++;

        // to dynamically grow out the size of tokens array, and check if the memory allocation failed
        if (position >= bufsize) {
        bufsize += LSH_TOK_BUFSIZE;
        tokens = realloc(tokens, bufsize * sizeof(char*));
        if (!tokens) {
            fprintf(stderr, "lsh: allocation error\n");
            exit(EXIT_FAILURE);
        }
        }
        // tells strtok() to continue on to the next token in the string (input line) with regards to the delimiter 
        // passing NULL as first argument continues tokenization (splitting line into args)
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    // Null-terminate array (many functions that process string arrays expect it to be null-terminated)
    tokens[position] = NULL;
    // return the token array
    return tokens;
}

// loop gets input from the user and executes it
void lsh_loop(void)
{
    char *line;                                         // pointer to string holding user input
    char **args;                                        // pointer to array of strings (arguments) from splitting the input line from user
    int status;                                         // indicates whether shell should keep running or not

    do {
        printf("> ");                                   // 1. print prompt
        line = lsh_read_line();                         // 2. read the line from user (we call a function for it)
        if (history_count < HISTORY_SIZE)               // 3. if history buffer not full add command into it
            history[history_count++] = strdup(line);
        args = lsh_split_line(line);                    // 4. split the line into args (we call a function for it)
        status = lsh_execute(args);                     // 5. Execute those args, also determines whether or not shell should keep on running (we call a function for it)

        // free pointers for line and arguments
        free(line);
        free(args);
    } while (status);
}

// argc = argument count [holds # of command line arguments passed to program, includes program name itself]
// argv = argument vector [array of strings (or specifically char* pointers) containing the command-line arguments passed into program]
int main(int argc, char **argv)
{
    // Load config files, if any.

    // Run command loop.
    lsh_loop();

    // Perform any shutdown/cleanup.
    for (int i = 0; i < history_count; i++)
        free(history[i]);

    // typically returns 0, indicates successful program termination
    return EXIT_SUCCESS;
}