#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <string.h>	// strtok()
#include <time.h>	// time(), ...
#include <stdio.h>	// printf(), ...
#include <unistd.h>
#include <sys/wait.h>	// open()
#include <sys/types.h>
#include <signal.h>	// signal()
#define MAXLINE 100
#define MOD "exit with CTR C"
using namespace std;
time_t startprogram;
time_t endprogram;
//Abbruch (Reagierung) bei "CTR + C"
void handler(int sig)
{
       
        endprogram = time(NULL);
        printf("Time vergangen : %ds\n", (endprogram - startprogram));
        exit(0);
}

void sigchld_handler(int sighl)
{
    while (waitpid(-1, NULL,WNOHANG) > 0); 
}

//Funktion für Arbeitsverzeichnis
string aktuellesVerzeichnis()
{
   char temp[256];
   
   if (getcwd(temp,256)){
       return string (temp);
   }else
   {
       return string(""); 
   }
}

int read_command(char* command, char* parameters[])
{
    char befehl[256];
    int i = 0;
    char* tmp = NULL;
        
    string prompt = "> ";
    string directory = aktuellesVerzeichnis(); 
    cout << directory.append(prompt);

    cin.getline(befehl, 256); // 
	
    tmp = strtok(befehl, " ");

    strcpy(command, tmp);
     
    while(tmp != NULL)
    {
        parameters[i] = new char[256];
        strcpy(parameters[i], tmp);

        //hole nächsten token
        tmp = strtok(NULL, " ");

        //Weiter in dem parameters array
        i++; 
    }
    
    if((command[0] == 'c') && (command[1] == 'd'))
    {
        cout << chdir(parameters[1]) << endl;	
	//continue;
    }
            
    if(strcmp(parameters[i-1], "&") == 0)
    {
        parameters[i-1] = NULL;
        return 0;
    }
    else
    {
        parameters[i] = NULL; 
    }

    //Keine Parameter gesetzt
    return 1;
   
   
    return -1;
}

int main()
{   
    int childPid;
    int status;
    char command[256];
    char* parameters[256];
    int noParams;
     startprogram = time(NULL);
   
    if (signal(SIGCHLD, sigchld_handler)) 
    { 
  
        perror("signal");
        return 1;
    }
    
    signal(SIGINT, handler);
    bool val = true;
   
    while (val)
    {
        //Eingabe lesen
        noParams = read_command(command, parameters);
        
        //Kinderprozess erzeugen
        childPid = fork();
        
       
        if (childPid == -1)
        {
            cout << "can not fork!" << endl;
            exit(1);
        }

        //Kindprozess
        else if (childPid == 0)
        {
            int a = execvp(command, parameters);

            //Überprüfen auf falsche eingabe
            if(a != 0) // -1 bedeutet, dass es fehlgeschlagen ist
            {
                cout << command << ": Kommando nicht gefunden." << endl;
            }
            
            //Programm ende
            exit(0);
        }

        //Vaterprozess
        else 
        {
            //Überprüfen auf Parameterübergabe
            if (noParams == 1)
            {    
                wait(&status); // Vaterprozess wartet mit sleep bis Kind fertig ist
			       
            }
            else
            {
                cout << "[" << childPid << "]" << endl;
            }
        }
    }
    //Programm beenden
    return 0;
}
