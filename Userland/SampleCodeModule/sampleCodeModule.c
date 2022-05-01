#include <userstdlib.h>
#include <shell.h>

int main() {
	int i = 0;
	welcome_message();
	while(1){
		put_char(2, '>');
		int * p;
		++i;
		p = malloc(i * sizeof(int));
		my_printf("dir: %x - aloc: %d ", p, i*sizeof(int));
		wait_command();
		free(p);
	}

}