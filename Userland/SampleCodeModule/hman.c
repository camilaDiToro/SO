#include <hangman.h>
#include <userstdlib.h>

#define WORDS_QTY 4
#define MAX_WORD_LENGHT 10
#define TO_MAY_MASK 0xDF

#define TO_MAY(a) ( (a)&TO_MAY_MASK )

static uint8_t  words[WORDS_QTY][MAX_WORD_LENGHT] = {{'P','E','Z',0},{'A','M','I','G','O',0},{'A','G','U','A',0},{'K','I','W','I',0}};
static uint8_t  current_word[MAX_WORD_LENGHT];
static uint8_t word_idx = 0;
static uint8_t lifes;
static uint8_t left_letters;
static uint8_t game_finished;

void printHangman(void){

  restartCursor();

  if(game_finished)
    return;
  my_printf("Vidas: %d",lifes);
  sprint(1,"\n\n\n");
  sprint(1,"         ");
  for(int i = 0 ; words[word_idx][i] ; ++i){
    put_char(1,current_word[i]);
    put_char(1,' ');
  }

  if(!left_letters){
    sprint(1,"\n\n\n Felicitaciones, ganaste :) \n");
    sprint(1,"Presiona p para volver a jugar \n");
    game_finished = 1;
  }

  if(!lifes){
    sprint(2,"\n\n\n Perdiste, no te quedan mas vidas :( \n");
    my_printf("La palabra era: %s", words[word_idx]);
    sprint(1,"\n Presiona p para volver a jugar \n");
    game_finished = 1;
  }

}

void initHangman(void){

  clearScreen();

  word_idx = (word_idx + 1) % WORDS_QTY;
  lifes = 8;
  left_letters = 0;
  game_finished = 0;

  for(int i = 0 ; words[word_idx][i] ; ++i){
    current_word[i] = '_';
    ++left_letters;
    //Make sure all the letters in the word are in mayus
    words[word_idx][i] = TO_MAY(words[word_idx][i]);
  }

  printHangman();
}

void check_letter(int c){
  if(game_finished){
    if(c=='p'){
      initHangman();
    }
    return;
  }

  c=TO_MAY(c);
  uint8_t not_found = 1;
  for(int i = 0 ; words[word_idx][i] ; ++i){
    if(words[word_idx][i] == c){
      if(current_word[i] == '_')
        --left_letters;
      current_word[i] = c;
      not_found = 0;
    }
  }
  lifes-=not_found;

  printHangman();

}