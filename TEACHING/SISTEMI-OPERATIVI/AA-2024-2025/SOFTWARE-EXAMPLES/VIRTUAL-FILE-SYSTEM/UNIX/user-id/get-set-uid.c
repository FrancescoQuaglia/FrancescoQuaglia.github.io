#include <unistd.h>
#include <stdio.h>

int main(int a ,char** b){

	uid_t id;
	uid_t euid;

	id = getuid();
	euid = geteuid();
	printf("I'm running on behalf of user %d - euid user %d\n",(int)id,(int)euid);
	printf(".. who would you like to become? ");
	scanf("%d",&id);
	setuid(id);
	id = getuid();
	euid = geteuid();
	printf("I'm now running on behalf of user %d - euid user %d\n",(int)id,(int)euid);
	pause();

}
