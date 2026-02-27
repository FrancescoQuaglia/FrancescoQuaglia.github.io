/******************************************************************
Welcome to the Operating System examination

You are editing the '/home/esame/prog.c' file. You cannot remove 
this file, just edit it so as to produce your own program according to
the specification listed below.

In the '/home/esame/'directory you can find a Makefile that you can 
use to compile this program to generate an executable named 'prog' 
in the same directory. Typing 'make posix' you will compile for 
Posix, while typing 'make winapi' you will compile for WinAPI just 
depending on the specific technology you selected to implement the
given specification. Most of the required header files (for either 
Posix or WinAPI compilation) are already included in the head of the
prog.c file you are editing. 

At the end of the examination, the last saved snapshot of this file
will be automatically stored by the system and will be then considered
for the evaluation of your exam. Modifications made to prog.c which are
not saved by you via the editor will not appear in the stored version
of the prog.c file. 
In other words, unsaved changes will not be tracked, so please save 
this file when you think you have finished software development.
You can also modify the Makefile if requesed, since this file will also
be automatically stored together with your program and will be part
of the final data to be evaluated for your exam.

PLEASE BE CAREFUL THAT THE LAST SAVED VERSION OF THE prog.c FILE (and of
the Makfile) WILL BE AUTOMATICALLY STORED WHEN YOU CLOSE YOUR EXAMINATION 
VIA THE CLOSURE CODE YOU RECEIVED, OR WHEN THE TIME YOU HAVE BEEN GRANTED
TO DEVELOP YOUR PROGRAM EXPIRES. 


SPECIFICATION TO BE IMPLEMENTED:
Scrivere un programma che riceva in input tramite argv[] N coppie di stringhe 
con N maggiore o uguale a 1, in cui la prima stringa della coppia indica il 
nome di un file. 
Per ogni coppia di strighe dovra' essere attivato un processo che dovra' creare 
il file associato alla prima delle stringhe della coppia o poi ogni 5 secondi 
dovra' effettuare il controllo su quante volte la seconda delle stringhe della 
coppia compare nel file, riportando il risultato su standard output.
Il main thread del processo originale dovra' leggere lo stream da standard input in 
modo indefinito, e dovra' scrivere i byte letti in uno dei file identificati 
tramite i nomi passati con argv[]. La scelta del file dove scrivere dovra' 
avvenire in modo circolare a partire dal primo file identificato tramite argv[], 
e il cambio del file destinazione dovra' avvenire qualora venga ricevuto il 
segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI).
Se il segnale SIGINT (o CTRL_C_EVENT nel caso WinAPI) colpira' invece uno degli 
altri processi, questo dovra' riportare il contenuto del file che sta gestendo 
su standard output.

In caso non vi sia immissione di dati sullo standard input, l'applicazione 
dovra' utilizzare non piu' del 5% della capacita' di lavoro della CPU.

*****************************************************************/
#ifdef Posix_compile
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#else
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv){

	return 0;
}
