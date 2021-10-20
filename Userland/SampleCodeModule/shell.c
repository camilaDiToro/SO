#include <shell.h>
#include <userstdlib.h>

#define COMMANDS_QTY 2

typedef void (*void_function)(void);

typedef struct{
  void_function action;
  char * name;
}command;

void help(void);
void time(void);
int execute_command(char * command);

static command valid_commands[COMMANDS_QTY] = {{&help,"help"}, {&time,"time"}};

void wait_command(void){

  char c = 0;
	char command[128];
	int i = 0;

  // Read de command until user presses enter
  while((c=get_char())!='\n'){
		put_char(1,c);
		command[i++] = c;
	}
  sprint(2, "\n");
	command[i] = 0;

  //Check if de command is valid.
  // Execute it if its valid
  if(!execute_command(command)){
    //If not valid, show mensage
    sprint(2, "Invalid command \n");
    sprint(2, command);
    sprint(2, "\n");
  }
}

int execute_command(char * command){
  for(int i=0 ; i<COMMANDS_QTY ; ++i){
    if(strcmp(command,valid_commands[i].name) == 0){
      valid_commands[i].action();
      return 1;
    }
  }
  return 0;
}

void help(void){
  sprint(1, "el unico comando valido por ahora es help \n");

  /*
    //clear
	  //sprint(1, "'clear' - Comando para limpiar la pantalla");
    //inforeg
    sprint(1, "'inforeg' - Comando para imprime todos los registros.");
    //printmem c
    sprint(1, "'printmem' - Comando para realizar un volcado de memoria de 32 bytes a partir del puntero pasado como parametro ");
    //time
	  sprint(1, "'time' - Comando utilizado para desplegar el dia y hora del sistema");

    //cronometro

    //sudoku

    //exit
	  sprint(1, "'exit' - Comando para apagar el sistema");
    */
}

void time(void){
  char time[11];
  get_time(time);
  sprint(1, "Hora: ");
  sprint(1, time);
  sprint(2, "\n");
}
