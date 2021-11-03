#include <mysudoku.h>
#include <userstdlib.h>

#define LEFT 37
#define UP 38
#define RIGHT 39
#define DOWN 40

static t_sudoku base = { {'-','-','-','2','6','-','7','-','1'},
                         {'6','8','-','-','7','-','-','9','-'},
                         {'1','9','-','-','-','4','5','-','-'},
                         {'8','2','-','1','-','-','-','4','-'},
                         {'-','-','4','6','-','2','9','-','-'},
                         {'-','5','-','-','-','3','-','2','8'},
                         {'-','-','9','3','-','-','-','7','4'},
                         {'-','4','-','-','5','-','-','3','6'},
                         {'7','-','3','-','1','8','-','-','-'} };
static t_sudoku s;
static uint8_t userPosition[2];
static uint8_t freeSpaces;

// Gets the next valid position in the sudoku
// A position is valid if it is inside the sudoku matrix and
// does not have a number at the initial sudoku
static void getNextPosition(uint8_t * userPosition, int dirx, int diry);

// Checks if the user is at the position (i,j) in the matrix
static int isUserPosition(int i, int j);

// Checks if (nextx, nexty) is inside the matrix
static uint8_t validPosition(int nextx, int nexty);

static int checkCompleteSudoku(void);
//Checks if the row and the col recived as parameter are valid
static int checkRowCol(int row, int col);
//Checks if the subsquare started at [row,col] is vaild
static int checkSquare(int row, int col);
static int isUserPosition(int i, int j);


void printSudoku(){

  // Boolean value to generate the twinkle effect
  static uint8_t pritnUserPosition = 1;

  my_printf("\n\n\n");
  for(int i=0; i < DIM; ++i ){

    if(!(i % 3)){
      my_printf("         -------------------------\n");
    }
    my_printf("         ");
    for(int j=0 ; j < DIM ; ++j){

       if(!(j % 3)){
         put_char(STDOUT, '|');
         put_char(STDOUT, ' ');
       }

       if(isUserPosition(i, j)){
         if(pritnUserPosition){
           put_char(STDERR, s[i][j]);
         }else{
           put_char(STDOUT, ' ');
         }
         pritnUserPosition = !pritnUserPosition;
       }else{
         put_char(STDOUT, s[i][j]);
       }

       put_char(STDOUT, ' ');

    }
    put_char(STDOUT, '|');
    put_char(STDOUT, '\n');
  }
  my_printf("         -------------------------\n");
  return;
}

void initSudoku(void){

 uint8_t userPositionSeted = 0;

  for(int i=0 ; i < DIM ; ++i){
    for(int j=0 ; j < DIM ; ++j){
      s[i][j] = base[i][j];
      if(base[i][j] == '-'){
        ++freeSpaces;
        if(!userPositionSeted){
          userPosition[0] = i;
          userPosition[1] = j;
          userPositionSeted = 1;
        }
      }
    }
  }
}

static uint8_t validPosition(int nextx, int nexty){
  return (nextx >= 0) && (nextx <= 9) && (nexty >= 0) && (nexty <= 9);
}

static void getNextPosition(uint8_t * userPosition, int dirx, int diry){
    int nextx = userPosition[0]+dirx;
    int nexty = userPosition[1]+diry;

    while(validPosition(nextx, nexty) && base[nextx][nexty]!='-'){
      nextx += dirx;
      nexty += diry;
    }
    if(validPosition(nextx, nexty)){
      userPosition[0] = nextx;
      userPosition[1] = nexty;
    }
}

void moveUser(int direction){
  // Vector de direcciones para saber hacia donde nos movemos
  static int dir[4][2] = {{0,-1},{-1,0},{0,1},{1,0}};
  // Como los valores ascii de las direcciones van del 37 al 40, utilizamos
  // esos nros como indices del vector.
  uint8_t idx = direction - LEFT;

  //Obtenemos la nueva posicion
  getNextPosition(userPosition, dir[idx][0], dir[idx][1]);
}

static int checkSquare(int fil, int col){
  int i, j;
  uint16_t flags = 0;
  uint16_t aux;
  for(i=fil ; i<fil+3 ; ++i){
    for(j=col ; j<col+3 ; ++j){
      if(s[i][j]!='-'){
        aux = (1 << (s[i][j]-'1'));
        if(flags&aux){
          return 0;
        }else{
          flags= flags|aux;
        }
      }
    }
  }
  return 1;
}

static int checkRowCol(int row, int col){
  uint16_t row_flags = 0;
  uint16_t col_flags = 0;
  uint16_t aux;
  for(int j=0 ; j<DIM ; ++j){
    if(s[row][j]!='-'){
      aux = 1 << (s[row][j] - '1');
      if(row_flags&aux){
        return 0;
      }else{
        row_flags|=aux;
      }
    }
    if(s[j][col]!='-'){
      aux = 1 << (s[j][col] - '1');
      if(col_flags&aux){
        return 0;
      }else{
        col_flags|=aux;
      }
    }
  }
  return 1;
}

static int checkCompleteSudoku(void){
  int ok = 1;
  for(int i=0 ; i<DIM && ok ; i+=3){
    for(int j=0 ; j<DIM && ok ; j+=3){
      ok = checkSquare(i,j);
    }
  }

  for(int i=0 ; i<DIM && ok ; ++i){
    ok = checkRowCol(i,i);
  }

  return ok;
}

void writeNumber(int number){
  freeSpaces -= s[userPosition[0]][userPosition[1]] == '-' ? 1 : 0;
  s[userPosition[0]][userPosition[1]] = number;

  int row = userPosition[0];
  int col = userPosition[1];

  static int valid = 1;

  if(!valid){
    valid = checkCompleteSudoku();
  }

  valid = valid && checkRowCol(row, col);

  int squareFil = (row / 3) * 3;
  int squareCol = (col / 3) * 3;
  valid = valid && checkSquare(squareFil,squareCol);

  if(!valid){
    sprint(STDERR, "Numero invalido");
  }else if(freeSpaces){
    my_printf("Sudoku valido  ");
  }else{
    my_printf("Sudoku completado!  ");
  }
}
