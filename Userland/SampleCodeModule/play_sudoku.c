#include <sudoku.h>
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
                         {'7','-','3','-','1','8','-','-','-'}};
static t_sudoku s;
static uint8_t userPosition[2];
static uint8_t freeSpaces;

static void getNextPosition(uint8_t * userPosition, int dirx, int diry);
static int isUserPosition(int i, int j);
static uint8_t validPosition(int nextx, int nexty);


static int isUserPosition(int i, int j){
  return i==userPosition[0] && j==userPosition[1];
}

void printSudoku(){

  static uint8_t pritnUserPosition = 1;

  sprint(1,"\n\n\n");
  for(int i=0 ; i< DIM ; ++i ){

    if(!(i%3)){
      sprint(1,"         -------------------------\n");
    }
    sprint(1,"         ");
    for(int j=0 ; j<DIM ; ++j){

       if(!(j%3)){
         put_char(1,'|');
         put_char(1,' ');
       }

       if(isUserPosition(i,j)){
         if(pritnUserPosition){
           put_char(2,s[i][j]);
         }else{
           put_char(1,' ');
         }
         pritnUserPosition = !pritnUserPosition;
       }else{
         put_char(1,s[i][j]);
       }

       put_char(1,' ');

    }
    put_char(1,'|');
    put_char(1,'\n');
  }
  sprint(1,"         -------------------------\n");

  return;
}

void initSudoku(void){

 uint8_t userPositionSeted = 0;

  for(int i = 0 ; i < DIM ; ++i){
    for(int j = 0 ; j < DIM ; ++j){
      s[i][j] = base[i][j];
      if(base[i][j] == '-'){
        ++freeSpaces;

        if(!userPositionSeted){
          userPosition[0]=i;
          userPosition[1]=j;
          userPositionSeted = 1;
        }
      }
    }
  }

}

static uint8_t validPosition(int nextx, int nexty){
  return nextx >=0 && nextx <= 9 && nexty>=0 && nexty<=9;
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

void writeNumber(int number){
  freeSpaces-= s[userPosition[0]][userPosition[1]] == '-'?1:0;
  s[userPosition[0]][userPosition[1]] = number;
  int row = userPosition[0];
  int col = userPosition[1];
  int rNums[DIM] = {0};
  int cNums[DIM] = {0};

  int valid = 1;

  for(int i=0 ; i< DIM && valid; ++i ){
    if( s[row][i]!='-' ){
      if(rNums[s[row][i]-'1']){
          valid = 0;
      }else{
          ++rNums[s[row][i]-'1'];
      }
    }

    if( s[i][col]!='-' ){
      if(cNums[s[i][col]-'1']){
          valid = 0;
      }else{
          ++cNums[s[i][col]-'1'];
      }
    }
  }

  int squareFil = (userPosition[0]/3)*3;
  int squareCol = (userPosition[1]/3)*3;
  int qNums[DIM] = {0};

  for(int i=squareFil; i<squareFil+3 && valid; ++i ){
    for(int j=squareCol ; j<squareCol+3 && valid; ++j ){
      if( s[i][j]!='-' ){
        if(qNums[s[i][j]-'1']){
            valid = 0;
        }else{
            ++qNums[s[i][j]-'1'];
        }
      }
    }
  }

  if(!valid){
    sprint(2,"Numero invalido");
  }else if(freeSpaces){
    sprint(1,"Sudoku valido  ");
  }else{
    sprint(1,"Sudoku completado!  ");
  }

}
