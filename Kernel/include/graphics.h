#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

/* Standard Library */
#include <stdint.h>

/* Local headers */
#include <process.h>

/**
 * @struct color 
 * @brief stores the color of a pixel
 */
typedef struct color {
    uint8_t R; /// < Red
    uint8_t G; /// < Green
    uint8_t B; /// < Black
} TColor;

/* From graphicMode.c */
extern const TColor RED;
extern const TColor WHITE;
extern const TColor GRAY;
extern const TColor BLACK;

void scr_init();

void scr_clear();
void scr_printLine();
void scr_printChar(char c);
void scr_printCharFormat(char c, const TColor* charColor, const TColor* bgColor);
void scr_print(const char* string);
void scr_printDec(uint64_t value);
void scr_printHex(uint64_t value);
void scr_printBin(uint64_t value);
void scr_printBase(uint64_t value, uint32_t base);
void scr_printRegisterFormat(uint64_t reg);
void scr_restartCursor();

/**
 * @brief Maps the screen (as text output) onto a process' I/O table.
 * 
 * @returns The file descriptor on which the screen was mapped for the
 * process, or -1 if an error occurred.
 */
int scr_mapToProcessFd(TPid pid, const TColor* color);

#endif
