#include <graphicMode.h>
#include <keyboard.h>
#include <exceptions.h>

typedef void (*Exception)(void);
static void zero_division();
static void invalid_opcode();
static void excepHandler(char * msg);
void give_control_to_user();
void _hlt();


//								Exception 0						Exception 6
static Exception exceptions[]={&zero_division, 0, 0, 0, 0, 0, &invalid_opcode};
static char * message[] = {"Zero Division Exception", "Invalid Opcode Exception"};

void exceptionDispatcher(int exception) {
    Exception ex = exceptions[exception];
	if(ex!=0){
		ex();
	} 
}

static void excepHandler(char * msg){
	print(msg); 		
	newLine();
	print("Press enter to continue");
	int c;
	do{
		_hlt();							// Stops machine but interruptions are on.
	} while((c=getChar())!='\n');
	clearAll();
	give_control_to_user();
}

static void zero_division() {
	excepHandler(message[0]);
}

static void invalid_opcode(){
	excepHandler(message[1]);
}

