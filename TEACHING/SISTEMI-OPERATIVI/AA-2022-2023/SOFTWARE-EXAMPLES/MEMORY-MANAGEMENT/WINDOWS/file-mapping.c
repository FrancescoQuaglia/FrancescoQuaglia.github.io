
#include <windows.h>
#include <stdio.h>

#define DISP_ 20 

#define Errore_(x) { puts(x); ExitProcess(1); }

char messaggio[256]; 

void scrittore(HANDLE mapping) {      
	char *p;      
	p = (char *) MapViewOfFile(mapping, FILE_MAP_WRITE,  0, 0, 0);       
	if ( p == NULL ) Errore_("MapViewOfFile error");       
	puts("Type the strings to post to file mapping ('quit' to close):");	fflush(stdout);
	do {           
		scanf("%s", messaggio);           
		strncpy(p, messaggio, DISP_);           
		p += DISP_;       
	} while( (strcmp(messaggio,"quit") != 0));       
	
	ExitProcess(0);   
}

void lettore(HANDLE mapping) { 
	char *p;       
	p = (char *)MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, 0);       
	if (p == NULL) Errore_("MapViewOfFile error");       
	printf("File mapping: \n");       

	while ((strcmp(p, "quit") != 0)) { 
		printf("%s\n", p);           
		p += DISP_; 
	}       
	
	ExitProcess(0); 
}   

int main(int argc, char *argv[]) {
	HANDLE mapping;  
	BOOL newprocess;  
	STARTUPINFO si; 
	PROCESS_INFORMATION pi;      
	
	if (argc == 1) {    /* Creazione/accesso segmento memoria condivisa */         
		mapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 50000, "my_mapping");         
		if (mapping == INVALID_HANDLE_VALUE) Errore_("Errore nella CreateFileMapping");         
		
		/* genero il processo scrittore */         
		memset(&si, 0, sizeof(si));         
		memset(&pi, 0, sizeof(pi));         
		si.cb = sizeof(si);
		newprocess = CreateProcess(".\\file-mapping.exe", ".\\file-mapping.exe scrittore", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);         
		if (newprocess == 0) { 
			Errore_("writer creation error");
		}  
		WaitForSingleObject(pi.hProcess, INFINITE);		memset(&si, 0, sizeof(si)); /* genero il processo lettore */  
		memset(&pi, 0, sizeof(pi));  
		si.cb = sizeof(si);  
		newprocess = CreateProcess(".\\file-mapping.exe", ".\\file-mapping lettore", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);         
		if (newprocess == 0) { 
			Errore_("reader creation error");
		}   
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	else 
	if (argc == 2 && strcmp(argv[1], "scrittore") == 0){ 
		mapping = OpenFileMapping(FILE_MAP_WRITE, FALSE, "my_mapping"); /* Apro il file mapping */        
		if (mapping == INVALID_HANDLE_VALUE) { 
			Errore_("OpenFileMapping error");
		}         
		scrittore(mapping); /* chiamo la scrittura */ 
	}
	else 
	if (argc == 2 && strcmp(argv[1], "lettore") == 0){ 
		mapping = OpenFileMapping(FILE_MAP_WRITE, FALSE, "my_mapping"); /* Apro il file mapping */         
		if(mapping == INVALID_HANDLE_VALUE) { 
			Errore_("OpenFIleMapping error");
		}        
		lettore(mapping); /* chiamo il lettore */ 
	}   
	return(0);
}


