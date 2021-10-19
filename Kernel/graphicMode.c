#include <graphicMode.h>
#include <fonts.h>


color_t WHITE = {0xFF,0xFF,0xFF};
color_t BLACK = {0x00,0x00,0x00};
color_t RED = {0xFF,0x00,0x00};

static const struct vbe_mode_info_structure * graphicModeInfo = (struct vbe_mode_info_structure *) 0x5C00;

static uint8_t * getPixelAddress(int i, int j) {
   return (uint8_t *) (graphicModeInfo->framebuffer+3*(graphicModeInfo->width*i+j));
}

static void drawPixel(int i, int j, color_t color){
  uint8_t * pixel = getPixelAddress(i,j);
  pixel[0] = color.R;
  pixel[1] = color.G;
  pixel[2] = color.B;
}

void printCharFormat(uint8_t c, color_t charColor, color_t bgColor){
  uint8_t * character = getCharMapping(c);

  int start_i = 0;
  int start_j = 0;

  uint8_t mask;

  for(int i = 0 ; i<CHAR_HEIGHT ; ++i){
    for(int j = 0 ; j<CHAR_WIDTH ; ++j){
      mask = 1 << (CHAR_WIDTH - j - 1);
      if(character[i] & mask){
        drawPixel(start_i + i, start_j + j, charColor);
      }else{
        drawPixel(start_i + i, start_j + j, bgColor);
      }
    }
  }
}

void prueba(){
  drawPixel(0,0,WHITE);
}

void printChar(uint8_t c){
  printCharFormat(c,WHITE,BLACK);
}
