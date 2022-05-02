#ifndef _GRAPHIC_MODE_H_
#define _GRAPHIC_MODE_H_

#include <stdint.h>

typedef struct window {
    uint8_t current_i;      // current pixel
    uint8_t current_j;      // current pixel
    uint8_t start_i;        // window start pixel
    uint8_t start_j;        // window start pixel
    uint16_t width, height; // window dimensions
} window;

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} TColor;

extern const TColor RED;
extern const TColor WHITE;
extern const TColor BLACK;

void printChar(char c);
void printCharFormat(char c, const TColor* charColor, const TColor* bgColor);
void print(const char* string);
void printDec(uint64_t value);
void printHex(uint64_t value);
void printBin(uint64_t value);
void printBase(uint64_t value, uint32_t base);
void printRegisterFormat(uint64_t reg);
uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base);

void initUniqueWindow();
void initDividedWindow();
void restartCursor();
void setScreen(uint8_t screen_id);
void clearAll();
void newLine();

#endif
