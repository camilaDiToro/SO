/* sampleCodeModule.c */

// #include <naiveConsole.h>
#include <userstdlib.h>
#include <shell.h>

static int var1 = 0;
static int var2 = 0;


int main() {

	welcome_message();

	/*
	int i = 65;
 	char str[] = "not printed with printf :D";
  	my_printf("\nMessage = %s %d %x", str, i, i);
	*/

	while(1){
		put_char(2,'>');
		wait_command();
	}

	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}
