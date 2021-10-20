#include <graphicMode.h>
#include <fonts.h>

static char buffer[64] = { '0' };

// la i es la fila
// la j es columna

// cw = current window
window cw[4];

static color_t WHITE = {0xFF,0xFF,0xFF};
static color_t BLACK = {0x00,0x00,0x00};
static color_t RED = {0xFF,0x00,0x00};

static const struct vbe_mode_info_structure * graphicModeInfo = (struct vbe_mode_info_structure *) 0x5C00;
static void getNextPosition(uint8_t id);
static void checkSpace(uint8_t id);
static void scrollUp(uint8_t id);
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static uint8_t * getPixelAddress(int i, int j) {
   return (uint8_t *) (graphicModeInfo->framebuffer+3*(graphicModeInfo->width*i+j));
}

static void drawPixel(int i, int j, color_t * color){
  uint8_t * pixel = getPixelAddress(i,j);
  pixel[0] = color->B;
  pixel[1] = color->G;
  pixel[2] = color->R;
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
  
  for(int i = 0 ; i<4 ; ++i){
    cw[i].current_i = 0;
    cw[i].current_j = 0;
    cw[i].width = (graphicModeInfo->width/(CHAR_WIDTH * 2)) -1;
    cw[i].height = (graphicModeInfo->height/(CHAR_HEIGHT * 2)) -1;
  }
  cw[0].start_i = 0;
  cw[0].start_j = 0;
  cw[1].start_i = 0;
  cw[1].start_j = graphicModeInfo->width/(CHAR_WIDTH * 2) + 1;
  cw[2].start_i = graphicModeInfo->height/(CHAR_HEIGHT * 2) + 1;
  cw[2].start_j = 0;
  cw[3].start_i = graphicModeInfo->height/(CHAR_HEIGHT * 2) +1;
  cw[3].start_j = graphicModeInfo->width/(CHAR_WIDTH * 2) + 1;

  int center = graphicModeInfo->height/2;
  for(int i = 0 ; i<graphicModeInfo->width; ++i){
     drawPixel(center, i, &WHITE);
     drawPixel(center + 1, i,  &WHITE);
  }

  center = graphicModeInfo->width/ 2;
  for(int i = 0 ; i<graphicModeInfo->height; ++i){
     drawPixel(i, center,  &WHITE);
     drawPixel(i, center + 1,  &WHITE);
  }
}

void printCharFormatId(uint8_t screen_id , uint8_t c, color_t * charColor, color_t * bgColor){

  if(c=='\n'){
    newLine(screen_id);
    return;
  }

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

static void getNextPosition(uint8_t id){
  cw[id].current_i += ((cw[id].current_j + 1) == cw[id].width )? 1:0;
  cw[id].current_j = (cw[id].current_j + 1) % cw[id].width;
}

static void checkSpace(uint8_t id){
  if(cw[id].current_i == cw[id].height)
      scrollUp(id);
}

static void scrollUp(uint8_t id){

  for(int i=1 ; i<cw[id].height * CHAR_HEIGHT; ++i){

    uint8_t * start = getPixelAddress(cw[id].start_i + i, cw[id].start_j);
    uint8_t * next = getPixelAddress(cw[id].start_i + CHAR_HEIGHT + i, cw[id].start_j);

    for(int j=0 ; j<cw[id].width*CHAR_WIDTH*3 ; ++j){
      start[j] = next[j];
    }
  }
   cw[id].current_i-=1;
} 

void printChar(uint8_t c){
  printCharFormat(c,&WHITE,&BLACK);
}

void print(const char * string){
 	for (int i = 0; string[i] != 0; ++i)
		printChar(string[i]);
}

// TO DO: print in divided window mode

void printCharFormat(uint8_t c, color_t * charColor, color_t * bgColor){
    printCharFormatId(0,c, charColor,bgColor);
}

void printCharId(uint8_t c, uint8_t screen_id){
  printCharFormatId(screen_id,c, &WHITE, &BLACK);
}

void newLine(uint8_t id){
     cw[id].current_j=0;
     cw[id].current_i+=1;
}

void clearAll(uint8_t id){
  cw[id].current_i = 0;
  cw[id].current_j = 0;
  for(int i=0; i<cw[id].height ; ++i ){
    for(int j=0; j<cw[id].width ; ++j){
        printCharFormatId(id, ' ', &WHITE, &BLACK);
    }
  }
  cw[id].current_i = 0;
  cw[id].current_j = 0;
}  

// TO DO: printBases in divided window mode

void printDec(uint64_t value)
{
	printBase(value, 10);
}

void printHex(uint64_t value)
{
	printBase(value, 16);
}

void printBin(uint64_t value)
{
	printBase(value, 2);
}

void printBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    print(buffer);
}

// Function copied from naiveConsole.c
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}