#ifndef _STRING_H_
#define _STRING_H_

/* Standard library */
#include <stddef.h>

/**
 * @brief Calculates the length of the string pointed to by s, excluding the terminating null byte ('\0').
 *
 * @returns The number of bytes in the string pointed to by s.
 */       
size_t strlen(const char* s);

/**
 * @brief Copies the string pointed to by src, including the terminating null byte ('\0'), to the buffer  
 * pointed to by dest. The strings may not overlap, and the destination string dest must be large 
 * enough to receive the copy.
 *
 * @returns A pointer to the destination string dest.
 */    
char* strcpy(char* dest, const char* src);

/**
 * @brief Copies at most size bytes from the string pointed to by src, including the terminating null byte ('\0'), 
 * to the buffer pointed to by dest. The strings may not overlap, and the destination string dest must be large 
 * enough to receive the copy.
 *
 * @returns A pointer to the destination string dest.
 */   
char* strncpy(char* dest, const char* src, size_t size);

/**
 * @brief Compares the two strings s1 and s2.
 *
 * @returns An integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than,
 * to match, or be greater than s2.
 */   
int strcmp(const char* s1, const char* s2);

/**
 * @brief Appends the src string to the dest string, overwriting the terminating null byte ('\0') at the end of dest,
 * and then adds a terminating null byte.
 *
 * @returns  A pointer to the resulting string dest.
 */   
char* strcat(char* dest, const char* src);


#endif