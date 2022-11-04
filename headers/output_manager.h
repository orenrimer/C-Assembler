#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "symbol_list.h"



/** 
 * wrapper method that writes all the .ob .end and .ext files
 * 
 * @param filePath the path were we are going to output the files
 * @param codeImg a pointer to the code image array
 * @param symbolList a pointer to the symbol list
 * @param dataImg a pointer to the data image array
 * @param totalIC the total number of instructions (including extra words)
 * @param IC the number of instructions (not including extra words)
 * @param DC the data counter
 * @return: TRUE if there were no errors while writing the files, otherwise FALSE
 */
BOOL write_files(char *filePath, WORD_BLOCK **codeImg, Entry *symbolList, long *dataImg, unsigned int totalIC,  unsigned int IC, unsigned int DC);


/** 
 * writes the .ob file
 * 
 * @param filePath the path were we are going to output the file
 * @param codeImg a pointer to the code image array
 * @param dataImg a pointer to the data image array
 * @param totalIC the total number of instructions (including extra words)
 * @param IC the number of instructions (not including extra words)
 * @param DC the data counter
 * @return: TRUE if there were no errors while writing the file, otherwise FALSE
 */
BOOL write_ob(char *filePath, WORD_BLOCK **codeImg, long *dataImg, unsigned int totalIC, unsigned int IC, unsigned int DC);


/** 
 * writes the .ent and .ext file
 * 
 * @param filePath the path were we are going to output the file
 * @param symbolList a pointer to the symbol list
 * @param entryType the entry type (entry or extern)
 * @param extension the file extension ('.ent' or '.ext')
 * @return: TRUE if there were no errors while writing the file, otherwise FALSE
 */
BOOL write_ent_ext(char *filePath, Entry *symbolList, ENTRY_TYPE entryType, char *extension);


/** 
 * writes the .am file
 * 
 * @param outPath the path were we are going to output the file
 * @param line the current line we want to write
 * @param macroImg a pointer to the macro image array
 * @param replace FALSE is we want to copy the line as it is, TRUE if we want to replace it with the macro content 
 * @param macroCount the index of the macro Image
 * @return: TRUE if there were no errors while writing the file, otherwise FALSE
 */
BOOL write_am(char *outPath, char *line, char *macroImg, BOOL replace, long macroCount);


/** 
 * sets the .as, .am and output folder paths 
 * 
 * @param fileName the file name the user has entered, without the .as extension
 * @param fullPath a pointer to the array where we will store the full file path
 * @param outPath a pointer to the array where we will store the out folder path
 * @param amPath a pointer to the array where we will store the am file path
 */
void set_file_paths(char *fileName, char *fullPath, char* outPath, char *amPath);


/** 
 *  prints an error message including te file name and line number
 * 
 * @param fileName the file name the user has entered, without the .as extension
 * @param fullPath a pointer to the array where we will store the full file path
 * @param outPath a pointer to the array where we will store the out folder path
 * @param amPath a pointer to the array where we will store the am file path
 */
void print_error_msg(LINE_DATA *lineData, char *msg);


/** 
 *  prints an warning message including te file name and line number
 * 
 * @param fileName the file name the user has entered, without the .as extension
 * @param fullPath a pointer to the array where we will store the full file path
 * @param outPath a pointer to the array where we will store the out folder path
 * @param amPath a pointer to the array where we will store the am file path
 */void print_warning_msg(LINE_DATA *lineData, char *msg);
