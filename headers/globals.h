#include <string.h>
#include <stdio.h>



#ifndef _GLOBALS_H
#define _GLOBALS_H
#define NUM_COMMAND 16			/* number of available code instruction */
#define NUM_INST_TYPES 5		/* number of instruction types */
#define MAX_LINE_LEN 80			/* max line length  */
#define START_ADDRESS 100		/* starting address for instructions */
#define MAX_SYMBOL_LEN 30		/* max symbol length */
#define MAX_IMG_SIZE 1000		/* max size for data image, code image and macro image */
#define MAX_IMMED_VAL 127		/* max immediate value for operands (2^7 - 1) */
#define MIN_IMMED_VAL -128		/* min immediate value for operands (-2^7) */
#define MAX_DATA_VAL 1023		/* max immediate value for data (2^10 - 1) */
#define MIN_DATA_VAL -1024		/* min immediate value for data (-2^10) */


#define MALLOC_ERROR() \
	printf("ERROR:: Error while trying to allocate memory, exiting program.\n");\
	exit(1);



typedef enum bool{TRUE = 1, FALSE = 0} BOOL;	/* boolean type */
    

/* instructions opcodes (page 18 in the booklet) */	
typedef enum opcode{
	MOV_OP = 0,
	CMP_OP = 1,
	ADD_OP = 2,
	SUB_OP = 3,
	NOT_OP = 4,
	CLR_OP = 5,
	LEA_OP = 6,
	INC_OP = 7,
	DEC_OP = 8,
	JMP_OP = 9,
	BNE_OP = 10,
	GET_OP = 11,
	PRN_OP = 12,
	JSR_OP = 13,
	RTS_OP = 14,
	HLT_OP = 15,
	NONE_OP = -1
}OPCODE;


/* operands addressing types (page 19 in the booklet) */
typedef enum addressing_type{
	IMMEDIATE_ADDR = 0,
	DIRECT_ADDR = 1,
	STRUCT_ADDR = 2,
	REGISTER_ADDR = 3,
	NONE_ADDR = -1
}ADDRESSING_TYPE;


/* instruction types */
typedef enum instruction_type{
	DATA_INST,
	STRING_INST,
	STRUCT_INST,
	ENTRY_INST,
	EXTERN_INST,
	CODE_INST,
	NONE_INST = -1
}INSTRUCTION_TYPE;


/* represents a code instruction */
typedef struct code_word{
	int dest_addressing;	/* the dest operand addressing type */
	int src_addressing;		/* the source operand addressing type */
	int opcode;				/* the instruction opcode */
}CODE_WORD;


/* represents a complete word block - the original code instruction and thee extra data words (if there are any) */
typedef struct word{
	CODE_WORD *head;
	long *data;
	int numExtraWords;
}WORD_BLOCK;


/* stores the file name and current line number, used for printing errors */
typedef struct file_data{
	char fileName[FILENAME_MAX + 1];
	unsigned int lineNum;
}LINE_DATA;

#endif