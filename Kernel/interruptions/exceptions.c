#include <naiveConsole.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

static void zero_division();
static void invalid_opcode();

typedef void (*Exception)(void);
static Exception exceptions[]={&zero_division(), 0, 0, 0, 0, 0, &invalid_opcode()};

void exceptionDispatcher(int exception) {
    Exception ex = exceptions[exception];
	if(ex!=0){
		ex();
	}
}

static void zero_division() {
    char * msgError = "Zero Division Exception";
    ncPrint(msgError);
	print_registers();
}

// To do: improve rutines. Ask what we have to do when an exception is thrown.
// The main idea is to make a unique function that is called by this functions.

static void invalid_opcode(){
    char * msgError = "Invalid Opcode Exception";
	ncPrint(msgError);
	print_registers();
}

// Funcion que imprime en formato registro. Deberia incluirse en naiveConsole.c
void ncPrintRegisterFormat(uint64_t reg){
	
	int aux = reg;
	int count =  16;
	
	while(aux){
		aux = aux>>4;
		--count;
	}

	for(int i=0; i<count ;i++){
		ncPrintChar('0');
	}
	ncPrintHexa(reg);
}

