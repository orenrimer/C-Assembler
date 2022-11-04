/**
 * @file common.h
 * contains common methods used by all the passes 
 */


#include "globals.h"
#include "utils.h"
#include "output_manager.h"





/** 
 * returns the addressing type of a given operand
 * 
 * @param operand the operand we want to get the addressing type for
 * @return: the addressing type, or none if the operand is invalid
 */
ADDRESSING_TYPE get_addressing_type(char *operand);



/** 
 * returns whether the instruction is .data .string .struct .extern .entry or a code instruction  
 * 
 * @param instruction the instruction we want to analyze
 * @return: the instruction type, or none if the instruction is invalid
 */
INSTRUCTION_TYPE get_instruction_type(char *instruction);


/** 
 * returns the opcode of a given instruction 
 * 
 * @param instruction the instruction we want to get the opcode for
 * @return: the instruction opcode, or none if the instruction is invalid
 */
OPCODE get_opcode(char *instruction);


/** 
 * returns how many operands the given instruction should have
 * 
 * @param op the opcode of the instruction
 * @return: the number of expected operands
 */
int get_num_operands(OPCODE op);


/** 
 * checks if the given symbol is valid (according to page 26-27 in the booklet)
 * 
 * @param lineData a pointer to a line data object
 * @param symbol the symbol we want to check
 * @param symbolLen the length of the symbol, if -1 check the entire string
 * @return: TRUE if the symbol is valid, otherwise FALSE
 */
BOOL is_symbol_valid(LINE_DATA *lineData, char *symbol, int symbolLen);


/** 
 * checks if the given operand is valid
 * 
 * @param lineData a pointer to a line data object
 * @param operand the operand we want to check
 * @return: TRUE if the operand is valid, otherwise FALSE
 */
BOOL is_valid_operand(LINE_DATA *lineData, char *operand);

/** 
 * checks if the given symbol is a reserved word (i.e register name or instruction name)
 * 
 * @param lineData a pointer to a line data object
 * @param word the word we want to check
 * @return: TRUE if the word is a reserved word, otherwise FALSE
 */
BOOL is_reserved_word(LINE_DATA *lineData, char *word);


/** 
 * gets the operands from a given instruction, checks if the operands are valid, and if so copies them inside the given char array
 * 
 * @param lineData a pointer to a line data object
 * @param instruction the instruction we wish to get the operands from
 * @param outOperands a pointer to the array where we will copy the operands too
 * @param checkValid whether we want to check if the operands are valid
 * @return: TRUE if the operands are valid, otherwise FALSE
 */
BOOL get_operands(LINE_DATA *lineData, char *instruction, char **outOperands, BOOL checkValid);


/** 
 * frees the given operands array
 * 
 * @param operands a pointer to the operands array we will free
 * @param numOfOperands the number of operands 
 */
void free_operands(char **operands, int numOfOperands);