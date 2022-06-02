#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

/* Local headers */
#include <kernelTypes.h>

/**
 * @struct color 
 * @brief Stores the color of a pixel
 */
typedef struct color {
    uint8_t R; /// < Red
    uint8_t G; /// < Green
    uint8_t B; /// < Black
} TColor;

/* Constans from graphicMode.c */
extern const TColor RED;
extern const TColor ORANGE;
extern const TColor GREEN;
extern const TColor BLUE;
extern const TColor WHITE;
extern const TColor GRAY;
extern const TColor BLACK;

/**
 * @brief Initialize screen
 */
void scr_init();

/**
 * @brief Clears screen
 */
void scr_clear();

/**
 * @brief Prints a line in the screen
 */
void scr_printLine();

/**
 * @brief Prints a char in the screen
 * 
 * @param c Char to print
 */
void scr_printChar(char c);

/**
 * @brief Prints a char with a specefic format in the screen
 * 
 * @param c Char to print
 * @param charColor Char's color
 * @param bgColor Char's background color
 */
void scr_printCharFormat(char c, const TColor* charColor, const TColor* bgColor);

/**
 * @brief Prints a string in the screen
 * 
 * @param string String to print
 */
void scr_print(const char* string);

/**
 * @brief Prints a number in decimal format in the screen
 * 
 * @param value Value to print
 */
void scr_printDec(uint64_t value);

/**
 * @brief Prints a number in hexadecimal format in the screen
 * 
 * @param value Value to print
 */
void scr_printHex(uint64_t value);

/**
 * @brief Prints a number in binary format in the screen
 * 
 * @param value Value to print
 */
void scr_printBin(uint64_t value);

/**
 * @brief Prints a number in a specific base format in the screen
 * 
 * @param value Value to print
 * @param base Base to transform
 */
void scr_printBase(uint64_t value, uint32_t base);

/**
 * @brief Prints in register format in the screen
 * 
 * @param reg Value to print
 */
void scr_printRegisterFormat(uint64_t reg);

/**
 * @brief Restarts screen cursor
 * 
 */
void scr_restartCursor();

/**
 * @brief Maps the screen (as text output) onto a process' I/O table.
 * 
 * @returns The file descriptor on which the screen was mapped for the
 * process, or -1 if an error occurred.
 */
int scr_mapToProcessFd(TPid pid, int fd, const TColor* color);

#endif
