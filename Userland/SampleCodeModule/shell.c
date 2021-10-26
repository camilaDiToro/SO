#include <shell.h>
#include <userstdlib.h>

#define COMMANDS_QTY 7
#define TICKS_PER_SECOND 18

#define CHRONO_SCREEN 1
#define TIME_SCREEN 3

typedef void (*void_function)(void);

typedef struct{
  void_function action;
  char * name;
}command;

static void help(void);
static void time(void);
//static void date(void);
static void play(void);
static void divide_by_zero(void);
static void invalid_operation(void);
static void print_mem(void);
static void inforeg(void);   // TO DO

static command valid_commands[COMMANDS_QTY] = {{&help,"help"}, {&time,"time"}, {&play, "play"}, {&divide_by_zero,"divideZero"},
         {&invalid_operation,"invalidOp"}, {&print_mem, "printmem"}, {&inforeg, "inforeg"}};

static uint8_t modify_chrono(char * chrono, uint8_t ms_ticks);
static void restart(char * chrono);
static int execute_command(char * command);

void wait_command(void){

  char c = 0;
	char command[128];
	int i = 0;

  // Read the command until the user presses enter
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

void divide_by_zero(void){
  divideByZero();
}

void invalid_operation(void){
  invalidOp();
}

void print_mem(void){
  printMem();
}

void inforeg(void){
  // TO DO
}

void help(void){
    sprint(1, "Los comandos disponibles son:\n");
    //my_printf("Los comandos disponibles son:\n");

    //help
    sprint(1, "   'help'       - Despliega un listado de todos los comandos disponibles.\n");
    //my_printf("   'help'       - Despliega un listado de todos los comandos disponibles.\n");

    //time
	  sprint(1, "   'time'       - Despliega el dia y hora del sistema.\n");
    //my_printf("   'time'     - Despliega el dia y hora del sistema.\n");

    //clear
	  //sprint(1, "   'clear'     - Limpia la pantalla.\n");
    //my_printf("   'clear'     - Limpia la pantalla.\n");

    //inforeg
    sprint(1, "   'inforeg'    - Imprime el valor de todos los registros.\n");
    //my_printf("   'inforeg'     - Imprime el valor de todos los registros.\n");

    //printmem 
    sprint(1, "   'printmem'   - Realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como parametro.\n");
    //my_printf("   'printmem'     - Realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como parametro.\n");

    //play
    sprint(1, "   'play'       - Divide la pantalla en 4 ventanas, donde:\n");
    sprint(1, "        1. Se podra visializar la hora, actualizandose permanentemente.\n");
    sprint(1, "        2. Se dispondra de un cronometro, capaz de ser operado desde el teclado.\n");
    sprint(1, "        3. Se podra jugar al sudoku.\n");
    sprint(1, "        4. Se podra jugar al ahorcado.\n");
    //my_printf("   'play'     - Divide la pantalla en 4 ventanas, donde:\n");
    //my_printf("          1. Se podra visializar la hora, actualizandose permanentemente.\n");
    //my_printf("          2. Se dispondrá de un cronómetro, capaz de ser operado desde el teclado.\n");
    //my_printf("          3. Se podra jugar al sudoku.\n");
    //my_printf("          4. Se podra jugar al ahorcado.\n");

    //Division por 0
    sprint(1, "   'divideZero' - Genera una excepcion causada por dividir por 0.\n");
    //my_printf("   'divisionByCero'     -  Genera una excepcion causada por dividir por 0 (00).\n");

    //Invalid operation
    sprint(1, "   'invalidOp'  - Genera una excepcion causada por una operacion invalida.\n");
    //my_printf("   'invalidOp'     - Genera una excepcion causada por una operacion invalida.\n");

    //exit
	  //sprint(1, "   'exit'     - Apaga el sistema.\t");
    //my_printg("   'exit'     - Apaga el sistema.\t");
    
}

void time(void){
  char time[11];
  get_time(time);
  sprint(1, "Hora: ");
  sprint(1, time);
  sprint(2, "\n");
  //my_printf("\n Hora: %... \n", time);

  char date[11];
  get_date(date);
  sprint(1, "Fecha: ");
  sprint(1, date);
  sprint(2, "\n");
  //my_printf("\n Fecha: %... \n", date);
}

/*
void date(void){
  char date[11];
  get_date(date);
  sprint(1, "Fecha: ");
  sprint(1, date);
  sprint(2, "\n");
} */



void handle_chrono(int tick, int c){
  static uint8_t paused = 0;
  static uint8_t ms_ticks = 0;
  static char chrono[] = {'0',':','0','0',':','0','0',',','0',' ',' ',0};

  setScreen(CHRONO_SCREEN);

  if(!paused && tick){
    ms_ticks = modify_chrono(chrono,++ms_ticks);
    restartCursor();
    sprint(1, chrono);
  }
  if(c=='0'){
    restart(chrono);
    paused = 1;
    restartCursor();
    sprint(1, chrono);
  }
  else if(c == '+'){
    paused = !paused;
  }

}

handle_time(int tick, int c){
  setScreen(TIME_SCREEN);
  static uint8_t tick_counter = 0;
  if(tick){
    if(!(tick_counter%18)){
      char time[11];
      get_time(time);
      restartCursor();
      sprint(1, time);
      tick_counter = 0;
    }
    ++tick_counter;
  }
}

void play(void){

  divideWindow();
  int c;
  int t;
  uint8_t quit = 0;

  while(!quit){
    c = read_char();
    t= tick();
    handle_chrono(t, c);
    handle_time(t, c);
    if(c== 'q'){
      quit = 1;
    }
  }

  uniqueWindow();
}


uint8_t modify_chrono(char * chrono, uint8_t ms_ticks){

  if(ms_ticks == TICKS_PER_SECOND){
    chrono[8] = '0';
    if(chrono[6] == '9'){
      chrono[6] = '0';
      if(chrono[5] == '5'){
        chrono[5] = '0';
        if(chrono[3] == '9'){
          chrono[3] = '0';
          if(chrono[2] == '5'){
            chrono[2] = '0';
            chrono[0]++;
          }else{
            chrono[2]++;
          }
        }else{
          chrono[3]++;
        }
      }else{
        chrono[5]++;
      }
    }else{
      chrono[6]++;
    }
  }else{
    chrono[8] = '0' + (ms_ticks * 10)/18;
  }
  return ms_ticks%TICKS_PER_SECOND;
}

void restart(char * chrono){
  chrono[0] = chrono[2] = chrono[3] = '0';
  chrono[5] = chrono[6] = chrono[8] = '0';
}
