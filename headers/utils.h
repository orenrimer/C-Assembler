#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"


#define NUM_BITS_IN_32 2    /* max number of bits we need to represent a number in base 32 */



/** 
 * converts the given number from decimal (base 10) to base 32
 * 
 * @param num the number we want to convert
 * @param base32 the char array where we will store the base 32 representation
 */
void dec_to_32(int num, char *base32);


/** 
 * removes any whitespaces from the given string
 * 
 * @param str the string we want to strip
 */
void remove_white_spaces(char *str);


/** 
 * skips to the next non-whitespace character in the string 
 * 
 * @param str the string we processing 
 * @return: how many characters we skiped
 */
int skip_white_spaces(char *str);


/** 
 * skips over the current word inside the string
 * 
 * @param str the string we processing 
 * @return: how many characters we skiped (the word length)
 */
int skip_word(char *str);


/** 
 * return a pointer to the next word inside the string
 * 
 * @param str the string we processing 
 * @return: a pointer to the next word
 */
char *get_next_word(char *str);


/** 
 * copies the given src string inside the given dest string.
 * before copying check if the src string fits inside the dest string 
 * after copying, null terminate the new string
 * 
 * @param dest a pointer to the destination string 
 * @param lenDst the destination string size
 * @param src a pointer to the source string 
 * @param lenSrc the source string size
 * @return: TRUE if we copied the string successfully, otherwise FALSE
 */
BOOL strncpy_safe(char *dest, size_t lenDst, const char *src, size_t lenSrc);


/** 
 * converts a given string into a long 
 * a valid number can only be a positive / negative natural number
 * 
 * @param str string representation of the number we want to convert
 * @param outVal out value in which we will store the converted number
 * @return: TRUE if we converted the string successfully, otherwise FALSE
 */
BOOL str_to_num(char *str, long *outVal);
