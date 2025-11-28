## Shell Commands

### Added Commands

* ```ls``` command: Lists all files and directories in the current working directory.
* ```pwd``` command: Prints out the current working directory to terminal.
* ```echo``` command: Prints out text, args, etc. to terminal.
* ```clear``` command: Clears terminal.
* ```history``` command: Displays a numbered list of commands entered during the current shell session.
* ```clearhistory``` command: Clears the stored command history for the current session.
* ```whoami``` command: Prints username of the current user.
* ```mkdir``` command: Creates a directory with specified name and default permissions (rwx for owner).
* ```rmdir``` command: Removes an empty directory with the specified name.
* ```delete``` command: Deletes a specified file after user confirmation **(USE WITH CAUTION)**.
* ```rdelete``` command: Recursively deletes a directory and all its contents after user confirmation **(USE WITH CAUTION)**.
* ```touch``` command: Creates an empty file or updates the timestamp of an existing file (rw for owner).
* ```cat``` command: Prints the contents of a file to the terminal.
* ```mydate``` command: Prints the current date and time in ```YYYY-MM-DD HH:MM:SS``` format.

### Built-in Default Commands

* ```cd``` command: for switching directories within shell
* ```help``` command: lists all commands available to user and basic info on how to use each one
* ```exit``` command: Exits the shell

## Shell

This repository includes a shell written in C. It takes advantage of system calls to create and manipulate the processes necessary for a shell to operate. Supports both built-in and custom commands. The main repository this shell is based on is [here](https://github.com/brenns10/lsh).

## Memory Allocator

This repository also includes a memory allocator written in C. It implements malloc(), free(), realloc(), and calloc() using the sbrk() call system from Unix/Linux OS. The main repository this allocator is based on is [here](https://github.com/arjun024/memalloc/tree/master.).

## How to use

First of all, the allocator and shell will work on a modern Unix/Linux environment but will not work on Windows. It contains header files (and thus macros/functions from the files) that are not natively supported on Windows. Using WSL or a VM is a possible workaround to this if you are on Windows.
<br />
<br /> 
Compiling main.c :
<br />
```
$ gcc -o main main.c
 ```

<br />
Compiling mem_allocator.c :
<br />

```
$ gcc -o mem_allocator.so -fPIC -shared mem_allocator.c
 ```
<br />
I included my compilation files but you can compile your own too (good practice). You can also make any file within the folder use the implementations from the memory allocator (including the shell) by adding the mem_allocator library file to program before running :
<br />
<br />

```
$ export LD_PRELOAD=$PWD/mem_allocator.so
```

<br />
To unlink the library file :
<br />

```
$ unset LD_PRELOAD
```
