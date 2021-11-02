#include <_play.h>

 #define SUDOKU_SCREEN 0
#define CHRONO_SCREEN 1
#define HANGMAN_SCREEN 2
#define TIME_SCREEN 3

static void handle_chrono(int tick, int c);
static void handle_time(int tick, int c);
static void handle_sudoku(int tick, int c);
static void handle_hangman(int tick, int c);

void play(void){

  // When play command is executed, the window has to be divided into 4 independent parts
  divideWindow();

  //Then, it is necessary to initialize our games
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
    if(c == ' '){
      quit = 1;
    }
  }
  uniqueWindow();
}

void handle_chrono(int tick, int c){
  static uint8_t paused = 0;
  static uint8_t ms_ticks = 0;
  static char chrono[] = {'0',':','0','0',':','0','0',',','0',' ',' ',0};

  // Set the corresponding screen
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

  // Override data with the new changes
  restartCursor();
  my_printf(chrono);
}

void handle_time(int tick, int c){
  
  static uint8_t tick_counter = 0;
  
  if(tick){
    // Ask for time only after 18 ticks (1 second aprox)
    if(!(tick_counter % 18)){
      char time[11];
      get_time(time);
      
      // Override data with the new changes
      setScreen(TIME_SCREEN);
      restartCursor();
      my_printf(time);

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
  if(tick){
    if((c>='a' && c<='z') || (c>='A' && c<='Z')){
      setScreen(HANGMAN_SCREEN);
      check_letter(c);
    }
  }
}