/* sampleCodeModule.c */

// #include <naiveConsole.h>
#include <userstdlib.h>
#include <shell.h>

static int var1 = 0;
static int var2 = 0;


int main() {

	welcome_message();

	while(1){
		put_char(2,'>');
		wait_command();
	}

	//Test if BSS is properly set up
	if (var1 == 0 && var2 == 0)
		return 0xDEADC0DE;

	return 0xDEADBEEF;
}
