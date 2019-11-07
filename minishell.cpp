//Bibliotheken einfügen
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

//Globale variable deklarieren oder initialisieren
bool myvalue = true;

//Funktion für Abbruch bei "STRG + C"
void handler(int sig)
{

        time_t begin = time(NULL);
        sleep(5);
        time_t end = time(NULL);
        printf("Time elapsed : %ds\n", (end - begin));
        exit(0);
    
}

void sigchld_handler(int sighl)
{
    while (waitpid(-1, NULL,WNOHANG) > 0);
}

//Funktion für Arbeitsverzeichnis
string workingDirectory()
{
   char temp[256];
   return(getcwd(temp, 256) ? string(temp) : string(""));
   //return string(temp), wenn 'working directory path' erfolgreich geholt wurde
   //sonst return string(" ")
   
}

//Funktion um Befehl zu lesen
int read_command(char* command, char* parameters[])
{
	/*
	* <Beispiel> 
	* command = "ls";
	* parameters[0] = "ls";
	* parameters[1] = "-al";
	* parameters[2] = NULL;
	*
	*/

    //Variablen deklarieren und oder initialisieren
    char befehl[256];
    int i = 0;
    char* tmp = NULL;
        
    string prompt = "> ";
    string directory = workingDirectory();  //working directory path 
    cout << directory.append(prompt); //prompt wird nach dem string dirctory hinzufuegt

    //Ganze Zeilenangabe lesen
    cin.getline(befehl, 256);

    //Befehl in Stücke zerlegen und in tmp abspeichern
    tmp = strtok(befehl, " ");

    //Inhalt von tmp in command kopieren
    strcpy(command, tmp);
     
    //Überprüfen, dass tmp nicht leer ist
    while(tmp != NULL)
    {
        //Zerlegte Befehlsstücke in den 2D-Pointer parameters abspeichern
        parameters[i] = new char[256];
        strcpy(parameters[i], tmp);

        //Nächsten Befehl holen
        tmp = strtok(NULL, " ");

        //Um +1 erhoehen
        i++;
    }
    
    //wenn der Befehl cd eingegeben wird, 
    if((command[0] == 'c') && (command[1] == 'd'))
    {
        cout << chdir(parameters[1]) << endl;	//change directory
	//continue;
    }
            
    //Überprüfen, ob das Kommando vorhanden ist
    if(strcmp(parameters[i-1], "&") == 0) //wenn parameters[i-1] '&' ist
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
   
    //Funktion beenden
    return -1;
}

int main()
{   
    int childPid;
    int status;
    char command[256];
    char* parameters[256];
    int noParams;
   
    //Überprüfen auf Zombieprozess und löschen! 
    if (signal(SIGCHLD, sigchld_handler))
    { 
    /*
    	SIGCHLD -> sobald ein Kind beendet wird, 
	erfragt Eltern P., auf welche Art der Kind beendet wurde,
	damit Kind P entgueltig verschwindet. 
	
	!! wenn der Eltern P. keine Statusabfrage durchfuehrt,
	verbleibt das Kind im Zombie-Zustand in der Prozesstabelle
    */
        perror("signal");
        return 1;
    }
    
    //signal(Interrupt) durch die Tastatur bei "CTR + C"
    signal(SIGINT, handler);
    
   
    while (myvalue)
    {
        //Eingabe lesen
        noParams = read_command(command, parameters);
        
        //Kinderprozess erzeugen
        childPid = fork();
        
       
        if (childPid == -1)
        {
            cout << "can't fork!" << endl;
            exit(1);
        }

        //Kindprozess
        else if (childPid == 0)
        {
            //Befehl ausführen
            int a = execvp(command, parameters);
            
            //Überprüfen auf falsche eingabe
            if(a != 0)
            {
                cout << command << ": Kommando nicht gefunden." << endl;
            }
            
            //Programm beenden
            exit(0);
        }

        //Vaterprozess
        else 
        {
            //Überprüfen auf KEINE Parameterübergabe
            if (noParams == 1)
            {    
                wait(&status); // for termination of command
			       // Vaterprozess wartet mit sleep-mode, bis Kind beendet 
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
