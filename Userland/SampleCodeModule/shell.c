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
	command[i] = 0;

  //Check if de command is valid.
  // Execute it if its valid
  if(!execute_command(command)){
    //If not valid, show mensage
    sprint(2, "Invalid command ");
    sprint(2, command);
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
  sprint(1, "el unico comando valido por ahora es help");
}

void time(void){
  char time[11];
  get_time(time);
  sprint(1, "Hora: ");
  sprint(1, time);
}
