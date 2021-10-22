#include <graphicMode.h>
#include <keyboard.h>
#include <exceptions.h>

typedef void (*Exception)(void);
static void zero_division();
static void invalid_opcode();
static void exceptionHandler(char * msg);


//								Exception 0						Exception 6
static Exception exceptions[]={&zero_division, 0, 0, 0, 0, 0, &invalid_opcode};
static char * message[] = {"Zero Division Exception", "Invalid Opcode Exception"};

void exceptionDispatcher(int exception) {
    Exception ex = exceptions[exception];
	if(ex!=0){
		ex();
	}
}

static void exceptionHandler(char * msg){
	print(msg); 		// TO DO: check if we have modified print function for multiple windows
	newLine(0);
	print_registers();	// TO DO: Update function to print what we need.
	print("Press enter to continue");
	int c;
	do{
		_hlt();	
	} while((c=getChar())!='\n');
	clearAll(0);
	give_control_to_user();
}

static void zero_division() {
	exceptionHandler(message[0]);
}

static void invalid_opcode(){
	exceptionHandler(message[1]);
}

// Function to print in register format
void printRegisterFormat(uint64_t reg){
	
	uint64_t aux = reg;
	uint64_t count =  16;
	
	while(aux){
		aux = aux>>4;
		--count;
	}

	for(int i=0; i<count ;i++){
		printChar('0');
	}
	
	if(reg){
		printHex(reg);
	}
	
}
