#include "globals.h"
#include "common.h"
#include "output_manager.h"
#include "symbol_list.h"

#define MACRO_LEN 5
#define MACRO_START "macro"
#define MACRO_END "endmacro"



/** 
 * handles each line according to the pre-assembler stage algorithm 
 * 
 * @param lineData a pointer the a line data object
 * @param outPath the path of the .am file we will create 
 * @param line the current line we are processing 
 * @param macroList a pointer the the symbol list where we store all the macros names
 * @param dataImg a pointer to the dataImg array
 * @param macroImg a pointer to the macro image array where we store all the macros content
 * @param outFlagMacro flag if we are currently copying a macro 
 * @param macroCount a pointer to a counter that counts the macro content
 * @return: TRUE if there were no errors while processing the line, otherwise FALSE
 */
BOOL handle_line_pre(LINE_DATA *lineData, char *outPath, char *line, char *macroImg, Entry *macroList, BOOL *outFlagMacro, unsigned int *macroCount);


/** 
 * checks if the given line has a macro defintion
 * macro definiton starts with the word "macro" followed by the macro name, for example:
 * macro m1
 * 
 * @param line the current line we are processing 
 * @return: TRUE if there is a macro definition inside the given line, otherwise FALSE
 */
BOOL has_macro(char *line);


/** 
 * checks if the given macro name is valid
 * a valid macro name contains alpha-numeric characters, and it's no longer than 30 characters
 * 
 * @param lineData a pointer the a line data object
 * @param macroName the name we want to check
 * @param outLen out paramater that returns the name length
 * @return: TRUE if the given name is valid, otherwise FALSE
 */
BOOL is_macro_valid(LINE_DATA *lineData, char *macroName, int *outLen);


/** 
 * checks if a given name is a macro name, if so finds it inside the macro list
 * 
 * @param line the current line we are processing 
 * @param macroList a pointer the the symbol list where we store all the macros names
 * @param outVal out paramater that returns the value of the macro inside the macro list
 * @return: TRUE if the given name is valid, otherwise FALSE
 */
BOOL is_macro_name(char *line, Entry *macroList, long *outVal);


/** 
 * stores the macro content inside the macro image array
 * 
 * @param line the current line we are processing 
 * @param macroImg a pointer to the macro image array where we store all the macros content
 * @param macroCount a pointer to a counter that counts the macro content
 */
void store_macro_content(char *line, char *macroImg, unsigned int *macroCount);


