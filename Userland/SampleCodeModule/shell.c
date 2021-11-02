#include <shell.h>
#include <hangman.h>
#include <userstdlib.h>
#include <sudoku.h>

#define COMMANDS_QTY 6
#define TICKS_PER_SECOND 18

#define SUDOKU_SCREEN 0
#define CHRONO_SCREEN 1
#define HANGMAN_SCREEN 2
#define TIME_SCREEN 3

typedef void (*void_function)(void);

typedef struct{
  void_function action;
  char * name;
}command;

static void help(void);
static void time(void);
static void play(void);
static void divide_by_zero(void);
static void invalid_operation(void);
static void print_mem(void);
extern void inforeg(void);


static command valid_commands[COMMANDS_QTY] = {{&help,"help"}, {&time,"time"}, {&play, "play"}, {&divide_by_zero, "dividezero"},
         {&invalid_operation, "invalidop"}, {&print_mem, "printmem"}};

static uint8_t modify_chrono(char * chrono, uint8_t ms_ticks);
static void restart(char * chrono);
static int execute_command(char * command);

void welcome_message(void){
  my_printf("Welcome to Userland \n");
  help();
}

void wait_command(void){

  char c = 0;
	char command[128];
	int i = 0;

  // Read the command until the user presses enter
  while((c=get_char())!='\n'){
    if(c == '\b' && i>0){
      put_char(1,c);
      command[--i] = 0;
    } else if(c != '\b'){
      put_char(1,c);
      command[i++] = c;
    }
	}
  sprint(2, "\n");
	command[i] = 0;

  //Check if de command is valid.
  // Execute it if its valid
  if(!execute_command(command)){
    //If not valid, show mensage
    sprint(2, "Comando invalido \n");
    sprint(2, command);
    sprint(2, "\n");
  }
}

int execute_command(char * command){
  
  if(strcmp(command, "inforeg")==0){
    inforeg();
    return 1;
  }

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

void help(void){
    my_printf("Los comandos disponibles son:\n");

    //help
    my_printf("   'help'       - Despliega un listado de todos los comandos disponibles.\n");

    //time
	  my_printf("   'time'       - Despliega el dia y hora del sistema.\n");

    //inforeg
    my_printf("   'inforeg'    - Imprime el valor de todos los registros.\n");

    //printmem
    my_printf("   'printmem'   - Realiza un volcado de memoria de 32 bytes a partir de la direccion recibida como parametro.\n");

    //play
    my_printf("   'play'       - Divide la pantalla en 4 ventanas, donde:\n");
    my_printf("          1. Se podra visializar la hora, actualizandose permanentemente.\n");
    my_printf("          2. Se dispondrá de un cronometro, capaz de ser operado desde el teclado.\n");
    my_printf("          3. Se podra jugar al sudoku.\n");
    my_printf("          4. Se podra jugar al ahorcado.\n");

    //Division por 0
    my_printf("   'dividezero'  - Genera una excepcion causada por dividir por 0.\n");

    //Invalid operation
    my_printf("   'invalidop'   - Genera una excepcion causada por una operacion invalida.\n");

}

void time(void){
  char time[11];
  get_time(time);
  my_printf("\n Hora: %s \n", time);

  char date[11];
  get_date(date);
  my_printf("\n Fecha: %s \n", date);
}



void handle_chrono(int tick, int c){
  static uint8_t paused = 0;
  static uint8_t ms_ticks = 0;
  static char chrono[] = {'0',':','0','0',':','0','0',',','0',' ',' ',0};

  setScreen(CHRONO_SCREEN);

  // If time has passed and the chronometer is not paused, modify the chronometer value
  if(!paused && tick){
    ms_ticks = modify_chrono(chrono, ++ms_ticks);
  }

  // Pause and restart the chronometer if 0 is pressed
  if(c == '0'){
    restart(chrono);
    paused = 1;
  }

  else if(c == '+'){
    paused = !paused;
  }
  
  restartCursor();
  sprint(1, chrono);
}

void handle_time(int tick, int c){
  setScreen(TIME_SCREEN);
  static uint8_t tick_counter = 0;
  if(tick){
    // Ask for time only after 18 ticks (1 second aprox)
    if(!(tick_counter % 18)){
      char time[11];
      get_time(time);
      restartCursor();
      sprint(1, time);
      tick_counter = 0;
    }
    ++tick_counter;
  }
}

void handle_sudoku(int tick, int c){
  setScreen(SUDOKU_SCREEN);
  restartCursor();
  // Move the user if an arrow has been pressed
  if(c>=37 && c<=40){
    moveUser(c);
  }
  // If a number has been pressed, write it in the sudoku
  else if(c>='1' && c<='9'){
    writeNumber(c);
  }

  // Always print the sudoku in order to generate de twinkle effect
  printSudoku();
}


void handle_hangman(int tick, int c){
  if((c>='a' && c<='z') || (c>='A' && c<='Z')){
    setScreen(HANGMAN_SCREEN);
    check_letter(c);
    printHangman();
  }
}

void play(void){

  divideWindow();
  setScreen(SUDOKU_SCREEN);
  initSudoku();
  setScreen(HANGMAN_SCREEN);
  initHangman();


  int c;
  int t;
  uint8_t quit = 0;

  while(!quit){
    c = read_char();
    t = tick();
    handle_chrono(t, c);
    handle_time(t, c);
    handle_sudoku(t,c);
    handle_hangman(t,c);
    if(c== ' '){
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
  return ms_ticks % TICKS_PER_SECOND;
}

void restart(char * chrono){
  chrono[0] = chrono[2] = chrono[3] = '0';
  chrono[5] = chrono[6] = chrono[8] = '0';
}
