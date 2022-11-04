#include "utils.h"
#include "output_manager.h"
#include "common.h"
#include "symbol_list.h"


/** 
 * handles each line according to the second pass algorithm 
 * 
 * @param lineData a pointer the a line data object
 * @param symbolList a pointer the the symbol list where we store all the symbols
 * @param outIC a pinter to the instruction counter
 * @param outL a pointer to the extra words counter
 * @return: TRUE if there were no errors while processing the line, otherwise FALSE
 */
BOOL handle_line_second(LINE_DATA *lineData, char *line, WORD_BLOCK **code_img, Entry *symbolList, unsigned int *outIC, unsigned int *outL);


/**
 * store symbols defined as .entry  
 * 
 * @param lineData a pointer the a line data object
 * @param line the current line we are processing 
 * @param symbolList a pointer the the symbol list where we store all the symbols
 * @return: TRUE if there were no errors, otherwise FALSE
 */
BOOL markEntry(LINE_DATA *lineData, char *line, Entry *symbolList);


/** 
 * gets the code instruction and it's operands, which will be used to build the extra data words (if there are any)
 * 
 * @param lineData a pointer the a line data object
 * @param line the current line we are processing 
 * @param codeImg a pointer to the code image array
 * @param symbolList a pointer the the symbol list where we will store all the symbols
 * @param outIC a pinter to the instruction counter
 * @param outL a pointer to the extra words counter
 * @return: TRUE if there were no errors while creating the extra words, otherwise FALSE
 */
BOOL handle_extra_words(LINE_DATA *lineData, char *line, WORD_BLOCK **codeImg, Entry *symbolList, unsigned int *outIC, unsigned int *outL);


/** 
 * gets the data from the given operands and stores it inside an extra data word/s
 * 
 * @param lineData a pointer the a line data object
 * @param word a pointer to a WORD_BLOCK object that stores the extra data words
 * @param operands the operands of the code instruction that we creating the extra words from
 * @param symbolList a pointer the the symbol list where we will store all the symbols
 * @param outIC a pinter to the instruction counter
 * @param outL a pointer to the extra words counter
 * @return: TRUE if there were no errors while creating the extra words, otherwise FALSE
 */
BOOL build_data_words(LINE_DATA *lineData, WORD_BLOCK *word, char **operands, Entry *symbolList, unsigned int *IC, unsigned int *L);