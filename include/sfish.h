#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>

char *result;
int alarm_num;

void commandParsing(char *cmd);
void changePrompt();
int count(char *cmd);
char* trimExtra(char* cmd);
void executable(char* cmd);
void redirection(char *cmd);
void handler(int cmd);
void userHandler(int cmd);
void input(char *array[]);
void output(char *array[]);
char* itoa(int num, char str[]);
void pwd();


#endif
