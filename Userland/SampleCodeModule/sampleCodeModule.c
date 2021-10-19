/* sampleCodeModule.c */

// #include <naiveConsole.h>
#include <userstdlib.h>


static int var1 = 0;
static int var2 = 0;


int main() {

	sprint(1,"Welcome to Userland");

	// while(1){
	// 	put_char(2,'>');
	// 	wait_command();
	// }

	put_char(2,'C');
	char c = get_char();
	put_char(2,c);

	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}

void wait_command(void){
	char c = 0;
	char command[128];
	int i = 0;
	while((c=get_char())!='\n'){
		put_char(1,c);
		command[i++] = c;
	}
	command[i] = 0;
}
