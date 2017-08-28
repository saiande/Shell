#include "sfish.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int MAX_SIZE = 1000000;
char *previous = NULL;
char *forever;
char *for_sec;

void commandParsing(char *cmd)
{
    char *pointer;
        if(strcmp(cmd, "help") == 0)
        {
            fprintf(stdout, "%s\n", "HELP \n"                                                   \
            "    - exit         exits the shell\n"                                              \
            "    - cd           changes the current working directory of the shell\n"           \
            "                   cd -        change to the last directory user was in\n"         \
            "                   cd          go to user's home directory\n"                      \
            "                   cd .        leaves you in the same directory\n"                 \
            "                   cd ..       move you up one directory\n"                        \
            "    - pwd          prints the absolute path of the working dircetory\n"            \
            "    - help         display this help menu.");                                      \
        }
        else if(strcmp(cmd, "exit") == 0)
        {
            exit(3);
        }

        else if(strcmp(cmd, "cd -") == 0)
        {
            if(previous == NULL)
            {
                fprintf(stderr, "%s\n","ERROR: OLD PWD NOT SET");
                return;
            }
            int a = chdir(previous);
            char buff[MAX_SIZE];
            setenv("PWD", getcwd(buff, MAX_SIZE), 1);
            if(a != 0)
            {
                fprintf(stderr, "%s\n", "ERROR: COULDN'T CHANGE DIRECTORY");
                return;
            }
        }

        else if(strcmp(cmd, "cd ..") == 0)
        {
            char bufff[MAX_SIZE];
            previous = getcwd(bufff, MAX_SIZE);
            int c = chdir("..");
            char buff[MAX_SIZE];
            setenv("PWD", getcwd(buff, MAX_SIZE), 1);
            if(c != 0)
            {
                fprintf(stderr, "%s\n", "ERROR: COULDN'T CHANGE DIRECTORY");
                return;
            }
        }

        else if(strcmp(cmd, "cd .") == 0)
        {
            char bufff[MAX_SIZE];
            previous = getcwd(bufff, MAX_SIZE);
            int c = chdir(".");
            char buff[MAX_SIZE];
            setenv("PWD", getcwd(buff, MAX_SIZE), 1);
            if(c != 0)
            {
                fprintf(stderr, "%s\n", "ERROR: COULDN'T CHANGE DIRECTORY");
                return;
            }
        }

        else if(strcmp(cmd, "cd") == 0)
        {
            char bufff[MAX_SIZE];
            previous = getcwd(bufff, MAX_SIZE);
            int a = chdir(getenv("HOME"));
            char buff[MAX_SIZE];
            setenv("PWD", getcwd(buff, MAX_SIZE), 1);
            if(a != 0)
            {
                fprintf(stderr, "%s\n", "ERROR: COULDN'T CHANGE DIRECTORY");
                return;
            }
        }
        else if((pointer = strstr(cmd, "cd")) == cmd)
        {
            char bufff[MAX_SIZE];
            previous = getcwd(bufff, MAX_SIZE);
            char *dir = cmd + 3;
            int c = chdir(dir);
            char buff[MAX_SIZE];
            setenv("PWD", getcwd(buff, MAX_SIZE), 1);
            if(c != 0)
            {
                fprintf(stderr, "%s\n", "ERROR: COULDN'T CHANGE DIRECTORY");
                return;
            }
        }

        else if(strcmp(cmd, "pwd") == 0)
        {
            // char buff[MAX_SIZE];
            // char * dir = getcwd(buff, MAX_SIZE);
            // int f = fork();
            // if(f == -1)
            // {
            //     fprintf(stderr, "%s\n","ERROR: FORK" );
            //     return;
            // }
            // if(f == 0)
            // {
            //     printf("%s\n", dir);
            //     exit(0);
            // }
            // else
            //     waitpid(-1, NULL, 0);
            pwd();
        }

        else
            redirection(cmd);

        changePrompt();

}
void pwd()
{
    char buff[MAX_SIZE];
    char * dir = getcwd(buff, MAX_SIZE);
    int f = fork();
    if(f == -1)
    {
        fprintf(stderr, "%s\n","ERROR: FORK" );
            return;
    }
    if(f == 0)
    {
        printf("%s\n", dir);
        exit(0);
    }
    else
        waitpid(-1, NULL, 0);
}

void redirection(char *cmd)
{
    // putting all elements of cmd into an array
    int a = 0;
    int i = 0;
    int slash = -1;
    int success = -1;
    char *anotha = malloc(sizeof(cmd));
    strcpy(anotha, cmd);
    int counter = count(anotha);
    char *array[counter];
    char d[2] = " ";
    char *first = NULL;
    first = strtok(anotha, d);
    while(first != NULL)
    {
        array[a] = first;
        first = strtok(NULL, d);
        a++;
    }
    array[counter - 1] = NULL;
        // for(int y = 0; y < counter; y++)
        // {
        //     printf("array[%d%s%s\n",y,"]: ", array[y]);
        // }
    int c = 0;
    for(int x = 0; x < counter - 1; x++)
    {
        if(strcmp(array[x], "|") == 0)
            c++;
    }

    //free(anotha);

    if((strstr(cmd, ">") != NULL) || (strstr(cmd, "<") != NULL))
    {
        //if program doesn't have a slash in it
        if(strstr(array[0], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[0]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[0]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
                array[0] = path;
        }
        // if command has both redirections
        if(strstr(cmd, ">") != NULL && strstr(cmd, "<") != NULL && (success == 0 || slash != 0))
        {
            for(i = 0; i < counter; i++)
            {
                if(strcmp(array[i], "<") == 0)
                    break;
            }
            char *args_array[i + 1];
            args_array[i] = NULL;
            for(int k = 0; k < i; k++)
                args_array[k] = array[k];
            char *in = array[i + 1];
            char *out = array[i + 3];
            int fd = open(out, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR);
            if(fd == -1)
            {
                fprintf(stderr, "%s\n","ERROR - CANNOT OPEN FILE" );
                return;
            }
            int fd1 = open(in, O_RDONLY, S_IRUSR);
            if(fd1 == -1)
            {
                fprintf(stderr, "%s\n","ERROR - CANNOT OPEN FILE" );
                return;
            }
            int f = fork();
            if(f == -1)
            {
                fprintf(stderr, "%s\n","ERROR: FORK" );
                return;
            }
            if(f == 0)
            {
                int d = dup2(fd, STDOUT_FILENO);
                if(d == -1)
                {
                    fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                    return;
                }
                int e = dup2(fd1, STDIN_FILENO);
                if(e == -1)
                {
                    fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                    return;
                }
                close(fd);
                close(fd1);
                execv(array[0], args_array);
            }
            else
            {
                close(fd);
                waitpid(-1, NULL, 0);
            }
            return;
        }
        // if command only has output redirection
        else if(strstr(cmd, ">") != NULL && strstr(cmd, "<") == NULL && (success == 0 || slash != 0))
        {
            output(array);
            return;
        }
        //if command only has input redirection
        else if(strstr(cmd, ">") == NULL && strstr(cmd, "<") != NULL && (success == 0 || slash != 0))
        {
            input(array);
            return;
        }
    }
    //if command has piping
    // else if(strstr(cmd, "|") != NULL)
    else if(c == 1)
    {
        //if command has no slash
        if(strstr(array[0], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[0]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[0]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
                array[0] = path;
        }

        for(i = 0; i < counter; i++)
        {
            if(strcmp(array[i], "|") == 0)
                break;
        }

        forever = malloc(sizeof(array[0]));
        strcpy(forever, array[0]);

        // if second command has no slash
        if(strstr(array[i + 1], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[i + 1]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[i + 1]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
            {
                array[i + 1] = path;
            }
        }
        array[0] = forever;
        //free(forever);
        char *input_args[i + 1];
        input_args[i] = NULL;
        for(int k = 0; k < i; k++)
            input_args[k] = array[k];

        int o_length = sizeof(array) - i;
        char *output_args[o_length];
        output_args[o_length - 1] = NULL;
        for(int k = 0; k < o_length; k++)
            output_args[k] = array[k + i + 1];

        int p[2];
        pipe(p);
        pid_t one;
        pid_t two;

                //p[1] = write end
                //p[0] = read end
        one = fork();
        if(one == -1)
        {
            fprintf(stderr, "%s\n","ERROR: FORK" );
            return;
        }
        if(one == 0)
        {
            int d = dup2(p[1], STDOUT_FILENO);
            if(d == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(p[0]);
            close(p[1]);
            execv(input_args[0], input_args);
        }
        two = fork();
        if(two == -1)
        {
            fprintf(stderr, "%s\n","ERROR - TWO" );
            return;
        }
        if(two == 0)
        {
            int e = dup2(p[0], STDIN_FILENO);
            if(e == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(p[0]);
            close(p[1]);
            execv(output_args[0], output_args);
        }
        else
        {
            close(p[0]);
            close(p[1]);
            waitpid(one, NULL, 0);
            waitpid(two, NULL, 0);
        }
        return;
        //free(forever);
    }
    else if(c == 2)
    {
        //if command has no slash
        if(strstr(array[0], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[0]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[0]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
                array[0] = path;
        }

        for(i = 0; i < counter; i++)
        {
            if(strcmp(array[i], "|") == 0)
                break;
        }

        forever = malloc(sizeof(array[0]));
        strcpy(forever, array[0]);

        // if second command has no slash
        if(strstr(array[i + 1], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[i + 1]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[i + 1]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
            {
                array[i + 1] = path;
            }
        }

        array[0] = forever;
        //free(forever);
        int v = 0;
        for(v = i + 1; v < counter; v++)
        {
            if(strcmp(array[v], "|") == 0)
                break;
        }
        for_sec = malloc(sizeof(array[i + 1]));
        strcpy(for_sec, array[i + 1]);
        //if third prog doesnt have a slash
        if(strstr(array[v + 1], "/") == NULL)
        {
            slash = 0;
            char *p = getenv("PATH");
            char buffer[MAX_SIZE];
            strcpy(buffer, p);
            char s[2] = ":";
            char *token = strtok(buffer, s);
            char path[strlen(array[v + 1]) + strlen(token) + 2];
            while(token != NULL)
            {
                strcpy(path, token);
                strcat(path, "/");
                strcat(path, array[v + 1]);
                struct stat buff;
                if(stat(path, &buff) == 0)
                {
                    success = 0;
                    break;
                }
                token = strtok(NULL, s);
            }
            if(success != 0)
            {
                fprintf(stderr, "%s\n","ERROR: UNKOWN COMMAND" );
                return;
            }
            else
            {
                array[v + 1] = path;
            }
        }
        array[i + 1] = for_sec;
        //free(for_sec);
        char *input_args[i + 1];
        input_args[i] = NULL;
        for(int k = 0; k < i; k++)
            input_args[k] = array[k];

        int o_length = v - i;
        char *output_args[o_length];
        output_args[o_length - 1] = NULL;
        for(int k = 0; k < o_length - 1; k++)
            output_args[k] = array[k + i + 1];

        int oo = counter - v - 1;
        char *out_out_args[oo];
        out_out_args[oo - 1] = NULL;
        for(int k = 0; k < oo - 1; k++)
            out_out_args[k] = array[k + v + 1];


        int p[2];
        int p2[2];
        pipe(p);
        pipe(p2);
        pid_t one;
        pid_t two;
        pid_t three;

                //p[1] = write end
                //p[0] = read end
        one = fork();
        if(one == -1)
        {
            fprintf(stderr, "%s\n","ERROR: FORK" );
            return;
        }
        if(one == 0)
        {
            int d = dup2(p[1], STDOUT_FILENO);
            if(d == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(p[0]);
            close(p[1]);
            close(p2[0]);
            close(p2[1]);
            execv(input_args[0], input_args);
        }
        two = fork();
        if(two == -1)
        {
            fprintf(stderr, "%s\n","ERROR - TWO" );
            return;
        }
        if(two == 0)
        {
            int e = dup2(p[0], STDIN_FILENO);
            if(e == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            int g = dup2(p2[1], STDOUT_FILENO);
            if(g == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(p[0]);
            close(p[1]);
            close(p2[0]);
            close(p2[1]);
            execv(output_args[0], output_args);
        }

        three = fork();
        if(three == -1)
        {
            fprintf(stderr, "%s\n","ERROR - THREE" );
            return;
        }
        if(three == 0)
        {
            int e = dup2(p2[0], STDIN_FILENO);
            if(e == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(p[0]);
            close(p[1]);
            close(p2[0]);
            close(p2[1]);
            execv(out_out_args[0], out_out_args);
        }
        else
        {
            close(p[0]);
            close(p[1]);
            close(p2[0]);
            close(p2[1]);
            waitpid(one, NULL, 0);
            waitpid(two, NULL, 0);
            waitpid(three, NULL, 0);
        }
        return;
        //free(forever);
        //free(for_sec);
    }
    else
        executable(cmd);
}

void output(char *array[])
{
    int i = 0;
    for(i = 0; i < sizeof(*array); i++)
    {
        if(strcmp(array[i], ">") == 0)
            break;
    }
    char *args_array[i +1];
    for(int k = 0; k < i; k++)
        args_array[k] = array[k];
    args_array[i] = NULL;
                // for(int w = 0; w < i; w++)
                //     printf("args_array[%d%s%s\n",w, "]: " ,args_array[w]);
    char *out = array[i + 1];
                //printf("%s\n",out );
    int fd = open(out, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR);
    if(fd == -1)
    {
        fprintf(stderr, "%s\n","ERROR - CANNOT OPEN FILE" );
        return;
    }
    if(strcmp(array[0],"pwd") == 0)
        pwd();
    else
    {
        int f = fork();
        if(f == -1)
        {
            fprintf(stderr, "%s\n","ERROR: FORK" );
            return;
        }
        if(f == 0)
        {
            int d = dup2(fd, STDOUT_FILENO);
            if(d == -1)
            {
                fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
                return;
            }
            close(fd);
            execv(array[0], args_array);
        }
        else
        {
            close(fd);
            waitpid(-1, NULL, 0);
        }
    }
}

void input(char *array[])
{
    int i = 0;
    for(i = 0; i < sizeof(*array); i++)
    {
        if(strcmp(array[i], "<") == 0)
            break;
    }
    char *args_array[i +1];
    for(int k = 0; k < i; k++)
        args_array[k] = array[k];
                    //printf("args_array[ %d%s%s\n",k, "]: ", args_array[k] );
    args_array[i] = NULL;
    char *in = array[i + 1];
    int fd = open(in, O_RDONLY, S_IRUSR);
    if(fd == -1)
    {
        fprintf(stderr, "%s\n","ERROR - CANNOT OPEN FILE" );
        return;
    }
    int f = fork();
    if(f == -1)
    {
        fprintf(stderr, "%s\n","ERROR: FORK" );
        return;
    }
    if(f == 0)
    {
        int d = dup2(fd, STDIN_FILENO);
        if(d == -1)
        {
            fprintf(stderr, "%s\n","ERROR IN REDIRECTION" );
            return;
        }
        close(fd);
                    //printf("array[0]: %s\n",array[0] );
        execv(array[0], args_array);
    }
    else
    {
        close(fd);
        waitpid(-1, NULL, 0);
    }
}

void executable(char *cmd)
{
    char *anotha = malloc(sizeof(cmd));
    char *anotha_two = malloc(sizeof(cmd));
    strcpy(anotha, cmd);
    strcpy(anotha_two, cmd);
    int a = 0;
    int counter = count(anotha);
    char *array[counter];
    char d[2] = " ";
    char *first = NULL;
    first = strtok(anotha, d);
    if(strstr(first, "/") != NULL)
    {
        struct stat buff;
        if(stat(first, &buff) == 0)
        {
            while(first != NULL)
            {
                array[a] = first;
                first = strtok(NULL, d);
                a++;
            }
            //free(anotha);
            array[counter - 1] = NULL;
            int f = fork();
            if(f == -1)
            {
                fprintf(stderr, "%s\n","ERROR: FORK" );
                return;
            }
            if(f == 0)
                execv(array[0], array);
            else
                waitpid(-1, NULL, 0);
        }
        else
        {
            fprintf(stderr, "%s\n", "ERROR: NON - EXISTENT FILE");
            return;
        }
    }

    else if(strstr("/", cmd) == NULL)
    {
        char *p = getenv("PATH");
        char buffer[MAX_SIZE];
        strcpy(buffer, p);
        char s[2] = ":";
        char *token = strtok(buffer, s);
        int success = -1;
        char path[strlen(first) + strlen(token) + 2];
        while(token != NULL)
        {
            strcpy(path, token);
            strcat(path, "/");
            strcat(path, first);
            struct stat buff;
            if(stat(path, &buff) == 0)
            {
                success = 0;
                break;
            }
            token = strtok(NULL, s);
        }
        if(success == 0)
        {
            char *tok = strtok(anotha_two, d);
            while(tok != NULL)
            {
                array[a] = tok;
                tok = strtok(NULL, d);
                a++;
            }
            array[counter - 1] = NULL;
            //free(anotha_two);
            int f = fork();
            if(f == -1)
            {
                fprintf(stderr, "%s\n","ERROR: FORK" );
                return;
            }
            if(f == 0)
                execv(path, array);
            else
                waitpid(-1, NULL, 0);
        }
        else
        {
            fprintf(stderr, "%s\n", "ERROR: NON - EXISTENT PROGRAM");
            return;
        }
    }
    else
    {
        fprintf(stderr, "%s\n","ERROR" );
        return;
    }
}


void handler(int cmd)
{
    static int number = 0;
    if(++number < alarm_num)
        alarm(1);
    else
    {
        char *y = "Your ";
        char *s = " second timer has finished";
        char x[1024];
        char *i = itoa(alarm_num, x);
        write(1, y, strlen(y));
        write(1, i, strlen(i));
        write(1, s, strlen(s));
        exit(0);
    }
}

char* itoa(int number, char str[])
{
    int s;
    int remainder;
    int size = 0;
    int n = 0;
    n = number;
    while (n != 0)
    {
        size++;
        n = n / 10;
    }
    for (s = 0; s < size; s++)
    {
        remainder = number % 10;
        number = number / 10;
        str[size - (s + 1)] = remainder + '0';
    }
    str[size] = '\0';
    return str;
}

void userHandler(int cmd)
{
    printf("%s\n","Well that was easy." );
    return;
}

void changePrompt()
{
    char buffer[MAX_SIZE];
    char *dir = getcwd(buffer, MAX_SIZE);
    char *prompt = "<sande> : < ";
    char *end = " > $ ";
    result = (char*)malloc(strlen(dir) + strlen(prompt) + strlen(end) + 1);
    strcpy(result, prompt);
    strcat(result, dir);
    strcat(result, end);
}

int count(char *cmd)
{
    int count = 0;
    for(int q = 0; q < strlen(cmd); q++)
    {
        if(cmd[q] == ' ')
            count++;
    }
    return count + 2;
}

char* trimExtra(char *cmd)
{
    int count = 0;
    int new_count = 0;
    char *dup = cmd;
    char *anotha_one = cmd;
    while(*dup != '\0')
    {
        while(isspace(*dup) && isspace(*(dup + 1)))
        {
            dup++;
        }
        count++;
        if(*(dup + 1) == '\0')
            break;
        else
            dup++;

    }
    char new_arr[count + 1];
    int i = 0;
    while(*anotha_one != '\0')
    {
        while(isspace(*anotha_one) && isspace(*(anotha_one + 1)))
        {
            anotha_one++;
        }
        new_arr[i] = *anotha_one;
        i++;
        anotha_one++;
    }
    new_arr[count] = '\0';
    int flag = -1;
    if(new_arr[0] == ' ')
    {
        flag = 0;
        new_count = count - 1;
        for(int k = 0; k < count; k++)
        {
            if(k + 1 < count)
                new_arr[k] = new_arr[k + 1];
        }
    }
    if(new_arr[count - 1] == ' ')
    {
        if(flag != 0)
            new_count = count - 1;
        else
            new_count --;
        new_arr[count -1] = '\0';
    }
    else
        new_arr[count] = '\0';
    char *new = malloc( count * sizeof(char) + 1);
    new = new_arr;
    if(new_count == 0)
        return new;
    else if(new_count != 0)
    {
        char anotha_arr[new_count + 1];
        for(int y = 0; y < new_count; y++)
        {
            anotha_arr[y] = new_arr[y];
            //printf("%d%c\n",y, anotha_arr[y] );
        }
        anotha_arr[new_count] = '\0';
        char *n = malloc(new_count * sizeof(char) + 1);
        n = anotha_arr;
        return n;
    }
    else
        return NULL;
}
