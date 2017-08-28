#include "sfish.h"
#include "debug.h"
#include <unistd.h>
#include <signal.h>
#include  <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


//int MAX = 100000;

/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/

    //printf("%d\n",getpid() );
    signal(SIGUSR2, userHandler);

    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGTSTP);
    sigprocmask(SIG_BLOCK,&block, NULL);

    char *cmd;
    changePrompt();
    while((cmd = readline(result)) != NULL) {
        // if (strcmp(cmd, "exit") == 0)
        //     break;
        if((int)(*cmd) == 0)
            continue;
        if((alarm_num = atoi(cmd)))
        {
            signal(SIGALRM, handler);
            alarm(1);
            while(1)
            {
                ;
            }
            exit(0);
        }
        commandParsing(trimExtra(cmd));

        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */
    }

    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    free(result);

    return EXIT_SUCCESS;
}
