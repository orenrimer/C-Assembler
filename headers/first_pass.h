#include "utils.h"
#include "output_manager.h"
#include "symbol_list.h"
#include "common.h"




/** 
 * handle each line according to the first pass algorithm 
 * 
 * @param lineData a pointer the a line data object
 * @param line the current line we are processing 
 * @param symbolList a pointer the the symbol list where we will store all the symbols
 * @param dataImg a pointer to the dataImg array
 * @param codeImg a pointer to the codeImg array
 * @param outIC a pinter to the instruction counter
 * @param outDC a pinter to the data counter
 * @param outL a pointer to the extra words counter
 * @return: TRUE if there were no errors while processing the line, otherwise FALSE
 */
BOOL handle_line_first(LINE_DATA *lineData, char *line, Entry *symbolList, long *dataImg, WORD_BLOCK **codeImg, unsigned int *outIC, unsigned int *outDC, unsigned int *outL);



/** 
 * a wrapper method to handle .data .string .struct instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param insType the instruction type
 * @param dataImg a pointer to the dataImg array
 * @param outDC a pinter to the data counter
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_instruction(LINE_DATA *lineData, char *instruction, INSTRUCTION_TYPE insType, long *dataImg, unsigned int *outDC);




/** 
 * handles .data instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param insType the instruction type
 * @param dataImg a pointer to the dataImg array
 * @param outDC a pinter to the data counter
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_data_instruction(LINE_DATA *lineData, char *instruction, long *dataImg, unsigned int *outDC);


/** 
 * handles .string instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param insType the instruction type
 * @param dataImg a pointer to the dataImg array
 * @param outDC a pinter to the data counter
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_string_instruction(LINE_DATA *lineData, char *instruction, long *dataImg, unsigned int *outDC);


/** 
 * handles .struct instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param insType the instruction type
 * @param dataImg a pointer to the dataImg array
 * @param outDC a pinter to the data counter
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_struct_instruction(LINE_DATA *lineData, char *instruction, long *dataImg, unsigned int *outDC);


/** 
 * handles .extern instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param symbol a pointer to the char array to where we will copy the external symbol too
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_extern_instruction(LINE_DATA *lineData, char *instruction, char *symbol);


/** 
 * handles .entry instructions
 * in the first pass we will only check if the entry symbol is valid, and then in the second pass we will store it inside the symbol list
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @return: TRUE if the entry symbol is valid, otherwise FALSE
 */
BOOL handle_entry_instruction(LINE_DATA *lineData, char *instruction);


/** 
 * handles command instructions
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param codeImg a pointer to the codeImg array
 * @param outDC a pinter to the data counter
 * @param outL a pointer to the extra words counter
 * @return: TRUE if there were no errors while processing the instructions, otherwise FALSE
 */
BOOL handle_code_instruction(LINE_DATA *lineData, char *instruction, WORD_BLOCK **codeImg, unsigned int *outIC, unsigned int *outL);


/** 
 * builds a CODE_WORD object that stores the original instruction coding
 * 
 * @param lineData a pointer the a line data object
 * @param instruction the instruction we are processing 
 * @param codeImg a pointer to the codeImg array
 * @param outDC a pinter to the data counter
 * @param outL a pointer to the extra words counter
 * @return: a pointer to the new CODE_WORD object
 */
CODE_WORD * build_code_word(char **operands, OPCODE op, int numOfOperands, int *numOfExtraWords);


/** 
 * builds a WORD_BLOCK object the CODE_WORD and the extra data words that will be needed
 * 
 * @param head a pointer to the code word  
 * @param numOfExtraWords the number of the extra words that we will create
 * @return: a pointer to the new WORD_BLOCK object
 */
WORD_BLOCK * build_word(CODE_WORD *head, int numOfExtraWords);


/** 
 * check if the operands addressing type is matching the instruction (according to the table at page 32 in the booklet)
 * 
 * @param codeWord a pointer to the code word  
 * @param opcode the opcode of instruction we are processing 
 * @return: TRUE if the addressing type is valid, otherwise FALSE
 */
BOOL check_matching_operands(ADDRESSING_TYPE src, ADDRESSING_TYPE dest, OPCODE opcode);


/** 
 * counts how many extra words the instruction creates, determent by the operands type 
 * 
 * @param firstAddressing the first operand addressing type
 * @param secondAddressing the second operand addressing type
 * @return: the number of extra words
 */
int count_extra_words(ADDRESSING_TYPE firstAddressing, ADDRESSING_TYPE secondAddressing);


/** 
 * checks if the line structure is valid, including missing / consecutive commas, missing operands, extraneous text
 * 
 * @param lineData a pointer the a line data object
 * @param line the current line we want to check
 * @param numOfArgs the expected number of arguments, if unknown pass -1
 * @return: TRUE if the line is valid, otherwise FALSE
 */
BOOL check_line_structure(LINE_DATA *lineData, char *line, int numOfArgs);