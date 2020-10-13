#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE 80
#define CMDS_N MAX_LINE/2 + 1
#define HISTORY_SIZE 10

char history_queue[HISTORY_SIZE][MAX_LINE];
int history_len[HISTORY_SIZE];
int n_hcmd = 0, posin, posout;

void saveCmdsInHistory(char *command, char input[], int length){
    if (strcmp(command,"history")) {
        int i;
        for (i = n_hcmd; i > 0; i--) {
            strcpy(history_queue[i], history_queue[i - 1]);
            history_len[i] = history_len[i - 1];
        }
        strcpy(history_queue[0], input);
        history_len[0] = length;
        n_hcmd += 1;
        if (n_hcmd > 10) {
            n_hcmd = 10;
        }
    }
}

void showHistory(){
    if (n_hcmd) {
        printf("Commands History:\n");
        int i, j;
        for (i = 0; i < n_hcmd; i++) {
            printf("\t%i. ", i + 1);
            for (j = 0; j < history_len[i]; j++){
                printf("%c", history_queue[i][j]);
            }
            printf("\n");
        }
    }
    else {
        printf("The commands history is empty.\n");
    }
}

void dup2_file_in(char *commands[]){
    commands[posin] = NULL;
    int file = open(commands[posin + 1], O_RDONLY);
    dup2(file, 0);
}

void dup2_file_out(char *commands[]){
    commands[posout] = NULL;
    int file = open(commands[posout + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);
    dup2(file, 1);
}

int readInput(char *commands[], char input[], int length) {
    if (length == 0)
        exit(0);

    input[length - 1] = '\0';

    char *temp = &input[0];

    if(!strcmp(temp,"!!")){
        if(n_hcmd) {
            input = history_queue[0];
            length = history_len[0];
        }
        else {
            printf("The commands history is empty.\n");
            exit(0);
        }
    }

    saveCmdsInHistory(temp, input, length);

    int i, ci = 0, cmd_flag = -1;
    commands[ci] = &input[0];
    for (i = 1; i < length; i++){
        if (input[i] == ' '){
            input[i] = '\0';
        }
        else if (input[i] == '&'){
            input[i] = '\0';
            if (cmd_flag == -1)
                cmd_flag = 1;
        }
        else if (input[i] == '>'){
            posout = ci + 1;
            if (cmd_flag == -1)
                cmd_flag = 2;
        }
        else if (input[i] == '<'){
            posin = ci + 1;
            if (cmd_flag == -1)
                cmd_flag = 3;
        }
        else if (input[i] == '|'){
            if (cmd_flag == -1)
                cmd_flag = 4;
        }
        if (!input[i - 1]){
            commands[++ci] = &input[i];
        }
    }
    commands[++ci] = NULL;

    if(!strcmp(commands[0],"history")){
        showHistory();
        return 0;
    }
    return cmd_flag;
}

int main(void) {
    bool should_run = true;
    char input[MAX_LINE];
    char *commands[CMDS_N];
    pid_t child_pid;
    int stat_loc, fin, fout;

    while (should_run) {
        dup2(fin, STDIN_FILENO);
        dup2(fout, STDIN_FILENO);
        printf("oki>> ");
        fflush(stdout);
        int length = read(STDIN_FILENO, input, MAX_LINE);
        int cmd_flag = readInput(commands, input, length);
        if(cmd_flag){
            child_pid = fork();
            if (child_pid == 0) {
                if (cmd_flag == 2){
                    dup2_file_out(commands);
                }
                else if (cmd_flag == 3){
                    dup2_file_in(commands);
                }
                if (execvp(commands[0], commands) == -1) {
                    printf("Error executing the command.\n");
                }
            } 
            else {
                if (cmd_flag == 1) waitpid(child_pid, &stat_loc, WUNTRACED);
            }
        }
    }

    return 0;
}