#include <myhman.h>
#include <userstdlib.h>

#define WORDS_QTY 4
#define MAX_WORD_LENGHT 10
#define TO_MAY_MASK 0xDF

#define TO_MAY(a) ( (a)&TO_MAY_MASK )

static uint8_t words[WORDS_QTY][MAX_WORD_LENGHT] = { {'P', 'E', 'Z', 0},
                                                     {'A', 'M', 'I', 'G', 'O', 0},
                                                     {'A', 'G', 'U', 'A', 0},
                                                     {'K', 'I', 'W', 'I', 0},
                                                     {'I', 'N', 'T', 'E', 'L', 0},
                                                     {'P', 'R', 'O', 'C', 'E', 'S', 'A', 'D', 'O', 'R', 0},
                                                     {'C', 'A', 'L', 'I', 'F', 'R', 'A', 'G', 'I', 'L', 'I', 'S', 'T', 'I', 'C', 'O', 0}};

static uint8_t current_word[MAX_WORD_LENGHT];
static uint8_t word_idx = 0;
static uint8_t lifes;
static uint8_t left_letters;
static uint8_t game_finished;

void printHangman(void){

  restartCursor();

  if(game_finished)
    return;

  my_printf("Vidas: %d \n\n\n         ", lifes);

  for(int i=0 ; words[word_idx][i] ; ++i){
    put_char(STDOUT, current_word[i]);
    put_char(STDOUT, ' ');
  }

  if(!left_letters){
    my_printf("\n\n\n Felicitaciones, ganaste :) \n");
    my_printf("Presiona \'p\' para volver a jugar \n");
    game_finished = 1;
  }

  if(!lifes){
    sprint(STDERR, "\n\n\n Perdiste, no te quedan mas vidas :( \n");
    my_printf("La palabra era: %s", words[word_idx]);
    my_printf("\n Presiona \'p\' para volver a jugar \n");
    game_finished = 1;
  }
}

void initHangman(void){

  clearScreen();

  // Get the next word in the array of words, or the first one if we have reached the last one. 
  word_idx = (word_idx + 1) % WORDS_QTY;
  lifes = 8;
  left_letters = 0;
  game_finished = 0;

  // Initialize the word to be shown printing as many '_' as letters in the word we are playing with.
  for(int i = 0 ; words[word_idx][i] ; ++i){
    current_word[i] = '_';
    ++left_letters;
    //Make sure all the letters in the word are in mayus
    words[word_idx][i] = TO_MAY(words[word_idx][i]);
  }

  // Print the initial hangman
  printHangman();
}

void check_letter(int c){
  
  // If the game has finished, the user can start a new game by pressing 'p'
  if(game_finished){
    if(c == 'p'){
      initHangman();
    }
    // If any other letter is pressed, nothing has to be done
    return;
  }

  c = TO_MAY(c);
  uint8_t not_found = 1;

  // Check if the letter recived belongs to the word we are playing with
  for(int i = 0 ; words[word_idx][i] ; ++i){
    
    if(words[word_idx][i] == c){
      // If we found the letter, just decrement the letters if the space was empty 
      // so that not to decrement twice for the same letter 
      if(current_word[i] == '_'){
        --left_letters;
      }
      // Add the letter to the word that is shown in the screen
      current_word[i] = c;
      // Mark the letter to avoid reducing attemps.
      not_found = 0;
    }
  }
  lifes -= not_found;

  printHangman();
}