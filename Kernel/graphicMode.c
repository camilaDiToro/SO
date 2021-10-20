#include <graphicMode.h>
#include <fonts.h>

color_t WHITE = {0xFF,0xFF,0xFF};
color_t BLACK = {0x00,0x00,0x00};
color_t RED = {0xFF,0x00,0x00};

// la i es la fila
// la j es columna


// cw = current window
window cw[4];

static const struct vbe_mode_info_structure * graphicModeInfo = (struct vbe_mode_info_structure *) 0x5C00;
static void getNextPosition(uint8_t id);
static void checkSpace(uint8_t id);
static void scrollUp(uint8_t id);
static void copyChar( uint8_t iFrom, uint8_t jFrom, uint8_t iTo, uint8_t jTo);

static uint8_t * getPixelAddress(int i, int j) {
   return (uint8_t *) (graphicModeInfo->framebuffer+3*(graphicModeInfo->width*i+j));
}

static void drawPixel(int i, int j, color_t color){
  uint8_t * pixel = getPixelAddress(i,j);
  pixel[0] = color.R;
  pixel[1] = color.G;
  pixel[2] = color.B;
}

void initUniqueWindow(){
  cw[0].current_i = 0;
  cw[0].current_j = 0;
  cw[0].start_i = 0;
  cw[0].start_j = 0;
  cw[0].width = graphicModeInfo->width / CHAR_WIDTH;
  cw[0].height = graphicModeInfo->height / CHAR_HEIGHT;
}

void initDividedWindow(){
  // Future impl 
}

void printCharFormatId(uint8_t screen_id , uint8_t c, color_t charColor, color_t bgColor){
  uint8_t * character = getCharMapping(c);
  checkSpace(screen_id);
  // upper left pixel of the current character
  uint16_t write_i = (cw[screen_id].start_i + cw[screen_id].current_i) * CHAR_HEIGHT;
  uint16_t write_j = (cw[screen_id].start_j + cw[screen_id].current_j) * CHAR_WIDTH;

  uint8_t mask;

  for(int i = 0 ; i<CHAR_HEIGHT ; ++i){
    for(int j = 0 ; j<CHAR_WIDTH ; ++j){
      mask = 1 << (CHAR_WIDTH - j - 1);
      if(character[i] & mask){
        drawPixel(write_i + i, write_j + j, charColor);
      }else{
        drawPixel(write_i + i, write_j + j, bgColor);
      }
    }
  }
  getNextPosition(screen_id);
  
}

void getNextPosition(uint8_t id){
  cw[id].current_i += ((cw[id].current_j + 1) == cw[id].width )? 1:0;
  cw[id].current_j = (cw[id].current_j + 1) % cw[id].width;
}

/* void copyChar( uint8_t iFrom, uint8_t jFrom, uint8_t iTo, uint8_t jTo){

  iFrom*=CHAR_HEIGHT;
  jFrom*=CHAR_WIDTH;
  iTo*=CHAR_HEIGHT;
  jTo*=CHAR_WIDTH;

  for(int i = 0 ; i<CHAR_HEIGHT ; ++i){    
    for(int j = 0 ; j<CHAR_WIDTH ; ++j){
      uint8_t * pixelFrom = getPixelAddress(iFrom + i,jFrom + j);
      uint8_t * pixelTo = getPixelAddress(iTo + i, jTo + j);
       pixelTo[0] = pixelFrom[0];
       pixelTo[1] = pixelFrom[1];
       pixelTo[2] = pixelFrom[2];
    }
  }
}
*/

void checkSpace(uint8_t id){
  if(cw[id].current_i == cw[id].height)
      scrollUp(id);
}

void scrollUp(uint8_t id){

  for(int i=1 ; i<cw[id].height * CHAR_HEIGHT; ++i){

    uint8_t * start = getPixelAddress(cw[id].start_i + i, cw[id].start_j);
    uint8_t * next = getPixelAddress(cw[id].start_i + CHAR_HEIGHT + i, cw[id].start_j);

    for(int j=0 ; j<cw[id].width*CHAR_WIDTH*3 ; ++j){
      start[j] = next[j];
    }
  }

  //   for(int i=1 ; i<=cw[id].height ; ++i){
  //     for(int j=0 ; j<cw[id].width ; ++j){
  //       copyChar(i,j,i-1,j);
  //     }
  //   }
   cw[id].current_i-=1;
} 

void printChar(uint8_t c){
  printCharFormat(c,WHITE,BLACK);
}

void printCharFormat(uint8_t c, color_t charColor, color_t bgColor){
    printCharFormatId(0,c, charColor,bgColor);
}

void newLine(uint64_t id){
  do {

    printChar(' '); 
  
  } while(((uint64_t)(cw[id].current_j - cw[id].start_j)%(cw[id].width))!=0);
}