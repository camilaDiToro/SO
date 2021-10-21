#include <shell.h>
#include <userstdlib.h>

#define COMMANDS_QTY 3
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
static void play(void);

static command valid_commands[COMMANDS_QTY] = {{&help,"help"}, {&time,"time"}, {&play, "play"}};

static uint8_t modify_chrono(char * chrono, uint8_t ms_ticks);
static void restart(char * chrono);
static int execute_command(char * command);

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



void handle_chrono(int tick, int c){
  static uint8_t paused = 0;
  static uint8_t ms_ticks = 0;
  static char chrono[] = {'0',':','0','0',':','0','0',',','0',' ',' ',0};

  if(!paused && tick){
    ms_ticks = modify_chrono(chrono,++ms_ticks);
    restartCursor(CHRONO_SCREEN);
    sprintId(1, chrono,CHRONO_SCREEN);
  }
  if(c=='0'){
    restart(chrono);
    paused = 1;
    restartCursor(CHRONO_SCREEN);
    sprintId(1, chrono,CHRONO_SCREEN);
  }
  else if(c == '+'){
    paused = !paused;
  }

}

handle_time(int tick, int c){
  static uint8_t tick_counter = 0;
  if(tick){
    if(!(tick_counter%18)){
      char time[11];
      get_time(time);
      restartCursor(TIME_SCREEN);
      sprintId(1, time,TIME_SCREEN);
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
