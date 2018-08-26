#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPINIT 8
#define DELIMS " \t\r\n\a"

int ezsh_cd(char **argv)
{
    if(argv[1] == NULL)
    {
        fprintf(stderr, "ezsh_cd: expected path\n");
    }
    else
    {
        if(chdir(argv[1]) == -1)
        {
            perror("chdir");
        }
    }

    return 1;
}

int ezsh_help(char **argv)
{
    printf("usage: enter a command or built in (cd, help, exit)\n");
    return 1;
}

int ezsh_exit(char ** argv)
{
    return 0;   
}

char *builtin_names[] = {
    "cd",
    "help",
    "exit"
};

int(*builtin_fps[]) (char **) = {
    &ezsh_cd,
    &ezsh_help,
    &ezsh_exit
};

int ezsh_num_builtins()
{
    return sizeof(builtin_names) / sizeof(char *);
}

/*
 * allocate a byte buffer or exit
 */
char *make_buffer_or_exit(size_t n)
{
    char *buf = (char *) malloc(sizeof(char) * n);

    if(buf == NULL)
    {
        perror("malloc");
        exit(1);
    }

    return buf;
}

/*
 * read a line byte by byte using fgetc from stdin, exit on failure or eof
 */
char *ezsh_read_line()
{
    int cap = CAPINIT, len = 0, c;
    char *buf = make_buffer_or_exit(cap + 1);

    while((c = fgetc(stdin)) != EOF && c != '\n')
    {
        if(len == cap)
        {
            char *tmp = make_buffer_or_exit(cap * 2 + 1);
            cap *= 2;

            for(int i=0; i<len; ++i)
                tmp[i] = buf[i];

            free(buf);
            buf = tmp;
        }

        buf[len++] = c;
    }

    if(ferror(stdin))
    {
        perror("fgetc");
        exit(1);
    }
    else if(feof(stdin))
    {
        printf("ezsh_read_line: exiting\n");
        exit(0);
    }

    buf[len] = '\0';
    return buf;
}

/*
 * parse command and arguments from line, assuming quotations are not allowed
 */
char **ezsh_split_line(char *line)
{
    int cap = CAPINIT, n = 0;
    char **tokens = (char **) malloc(sizeof(char *) * cap), *token;

    if(tokens == NULL)
    {
        perror("malloc");
        exit(1);
    }

    token = strtok(line, DELIMS);
    while(token != NULL)
    {
        if(n >= cap)
        {
            cap *= 2;
            token = realloc(tokens, sizeof(char *) * cap);

            if(tokens == NULL)
            {
                perror("realloc");
                exit(1);
            }
        }

        tokens[n++] = token;
        token = strtok(NULL, DELIMS);
    }
    
    tokens[n] = NULL;
    return tokens;
}

/*
 * exec command, searching the user's path, exit on any error
 */
int ezsh_launch(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    
    if(pid == 0)
    {
        if(execvp(args[0], args) == -1)
            perror("execvp");

        exit(1);
    }
    else if(pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else
    {
        if(waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            exit(1);
        }

        /*        
        if(WIFEXITED(status))
            printf("waitpid: exited %d\n", WEXITSTATUS(status));
        else
            printf("waitpid: signalled/stopped/continued\n");
        */
    }

    return 1;
}


/*
 * command or builtin
 */
int ezsh_execute(char **argv)
{
    if(argv[0] == NULL)
        return 1;
    
    for(int i=0; i<ezsh_num_builtins(); ++i)
    {
        if(strcmp(argv[0], builtin_names[i]) == 0)
            return builtin_fps[i](argv);
    }    
    
    return ezsh_launch(argv);
}

/*
 * read stdin, parse, execute command with arguments
 */
void ezsh_loop()
{
    char *line;
    char **argv;
    int status;

    do
    {
        printf("> ");

        line = ezsh_read_line();
        // printf("%s\n", line);

        argv = ezsh_split_line(line);
        // int i=0;
        // while(argv[i] != NULL)
        //     printf("%s\n", argv[i++]);

        status = ezsh_execute(argv);

        free(line);
        free(argv);
    }
    while(status);
}

/*
 * ezsh is a simple Unix shell written in C 
 */
int main(int argc, char** argv)
{ 
    ezsh_loop();
    return 0;
}
