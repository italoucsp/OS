#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include<string>
#include<vector>
#include <queue> 
#include<fcntl.h> 

using namespace std;

queue<string>historial;

char **get_input(char *input) {
    char **command = new char*;
    string temp(input);
    if (temp=="!!"){
      if (historial.size()==0)
        cout<<"No hay Comandos en Historial"<<endl;
      else
        strcpy(input,(historial.back()).c_str());
    }
    int pos = temp.find(" > ");
    if (pos !=-1)
    {
      string Comando=temp.substr(0,pos);
      string Descriptor=temp.substr(pos+3);

      int out = open(Descriptor.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
      dup2(out, 1) ; 
      strcpy(input,Comando.c_str());
    }  
    pos = temp.find(" < ");
    if ( pos !=-1)
    {
      string Comando=temp.substr(0,pos);
      string Descriptor=temp.substr(pos+3);

      Comando= Comando + " " + Descriptor;
      strcpy(input,Comando.c_str());
    }
    if (historial.size()<10){
      historial.push(temp);
    }  
    else {
      historial.pop();
      historial.push(temp);
    }
    char *parsed;
    int index = 0;
    char separator[]=" ";

    parsed = strtok(input, separator);

    while (parsed != NULL) {
      command[index] = parsed;
      index++;
      parsed = strtok(NULL, separator);
    }
    command[index] = NULL;
    if (input[0]=='h'){
      queue<string> second (historial);
      int i=1;
      while (!second.empty()){
        cout<<i<<":"<<second.front()<<endl;
        second.pop();
        i++;
      }
    }
    return command;
}

int main() {
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;
    while (true) {
      input = readline("osh> ");
      command = get_input(input);  
      if (!command[0]) {   
        free(input);
        free(command);
        continue;
      }
      child_pid = fork();
      if (child_pid == 0) {
        execvp(command[0], command);
      } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
      }
      free(input);
      free(command);
    }
    return 0;
}
