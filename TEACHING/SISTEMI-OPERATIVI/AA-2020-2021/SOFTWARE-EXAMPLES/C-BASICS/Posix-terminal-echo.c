#include <unistd.h>

int main(int x, char** y){
	char c;

	while(1){
		read(0,&c,1);
		write(1,&c,1);

	}
}
