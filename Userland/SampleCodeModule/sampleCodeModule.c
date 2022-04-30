#include <userstdlib.h>
#include <shell.h>

int main() {

	welcome_message();
	while(1){
		put_char(2, '>');
		wait_command();
	}

}