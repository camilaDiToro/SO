#ifndef _USERSTDLIB_H_
#define _USERSTDLIB_H_

/**
 * @brief Delay for a specified amount of milliseconds
 */
void sleep(unsigned long millis);

/**
 * @brief Reads the next character from STDIN
 */
int getChar();

/**
 * @brief Writes the character into STODUT
 */
int putChar(char c);

/**
 * @brief Reads the next character from the given output stream
 */
int fgetChar(int fd);

/**
 * @brief Writes the character to the given output stream
 */
int fputChar(int fd, char c);

/**
 * @brief Writes a string into STDOUT
 */
int print(const char* str);

/**
 * @brief Writes a string to the given output stream
 */
int fprint(int fd, const char* str);

/**
 * @brief Produces output according to a format
 */
void printf(const char* frmt, ...);

/**
 * @brief Writes output to the given output stream
 */
void fprintf(int fd, const char* frmt, ...);

/**
 * @brief Convert a string to an integer
 */
int atoi(const char* str);

/**
 * @brief Reads an entire line from STDIN. The buffer is null-terminated.
 */
int getLine(char* buffer, int maxSize);

/**
 * @brief Reads an entire line from the given output stream. The buffer is null-terminated.
 */
int fgetLine(int fd, char* buffer, int maxSize);

/**
 * @brief Raises an invalid opcode exception
 */
void invalidOp();

/**
 * @brief Raises a divide by zero exception
 */
void divideByZero();

#endif
