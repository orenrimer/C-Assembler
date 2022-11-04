#include "headers/common.h"



char *instructionTypes[NUM_INST_TYPES] = {".data", ".string", ".struct", ".entry" , ".extern"}; /* instruction types */
char *instructionNames[16] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "get", "prn", "jsr", "rts", "hlt"};  /*instruction Names */




BOOL get_operands(LINE_DATA *lineData, char *instruction, char **outOperands, BOOL checkValid){
    char *p = NULL;     /* temp pointer for strtok */
    int i = 0;
    size_t operandLen = 0;

    p = strtok(instruction, ",");   /* get the first operand */

    while(p){
        /* check if the operand is valid, in the second pass we can skip this part because we know that the operands are already valid */
        if(checkValid){
            if(!is_valid_operand(lineData, p)) return FALSE;
        }

        /* if the operand is valid, strip it and copy it inside the given array */
        remove_white_spaces(p);
        operandLen = strlen(p);
        strncpy_safe(outOperands[i], MAX_LINE_LEN + 1, p, operandLen);

        p = strtok(NULL, ",");  /* get the next part */
        i++;
    }

    return TRUE;
}



BOOL is_symbol_valid(LINE_DATA *lineData, char *symbol, int symbolLen){
    int j = 0, end; 
    end = (symbolLen == -1) ? strlen(symbol) : symbolLen;   /* how many characters we want to check */

    /* if the symbol is longer than the max symbol length */
    if(end > MAX_SYMBOL_LEN){
        print_error_msg(lineData, "Symbol can not be longer than characters");
        return FALSE;       
    } 

    /* the first character of the symbol can not be a digit */
    if(isdigit(symbol[0])){
        print_error_msg(lineData, "Symbol can not start with a digit");
        return FALSE;
    }

    /* if the symbol is a reserved word (i.e register name or instruction name) */
    if(is_reserved_word(lineData, symbol)) return FALSE;

    
    /* iterate the symbol */
    while (j < end){
        /* check if there are any whitespaces or special characters inside the symbol, if so return false */
        if(isspace(symbol[j]) || symbol[j] == '\t' || (!isalpha(symbol[j]) && !isdigit(symbol[j]))){
            print_error_msg(lineData, "Symbol can only contain alpha-numerical characters");
            return FALSE;
        }
        j++;
    }
    
    return TRUE;
}


BOOL is_reserved_word(LINE_DATA *lineData, char *word){
    long immediate = -1;
    OPCODE op = NONE_OP; 

    word += skip_white_spaces(word);

    /* check if the word is a register name r0 - r7 */
    if(word[0] == 'r'){ 
        str_to_num(&word[1], &immediate);
       if(immediate >= 0 && immediate <= 7){
            print_error_msg(lineData, "Symbol can not be a register name");
            return TRUE;
       }
    }

    /* check if the word is an instruction name */
    op = get_opcode(word);
    if(op != NONE_OP){
        print_error_msg(lineData, "Symbol can not be an instruction name");
        return TRUE;
    } 

	return FALSE;
}


BOOL is_valid_operand(LINE_DATA *lineData, char *operand){
    long immediate = -1;
    char *dot = NULL;
    
	operand += skip_white_spaces(operand);
	
    /* if operands is starts with '#' it's an immediate value*/
    if(operand[0] == '#'){
        /* check if the are whitespaces between '#' and the value */
        if(isspace(operand[1]) || operand[1] == '\t'){
            print_error_msg(lineData, "Invalid operand, cannot have whitespaces between '#' and the immediate value");
            return FALSE;
        }

        /* check if the values is valid */
        if(!str_to_num(&operand[1], &immediate)){
            print_error_msg(lineData, "Invalid operand, immediate value must be a natural number");
            return FALSE;
        }else if(immediate > MAX_IMMED_VAL || immediate < MIN_IMMED_VAL){
            /* 
             *immediate values will be stored in 8 bits, meaning we can only store values between  -127 - 127
             * this is just a warning not an error, given a larger number we will keep only the 8 LSB 
             */
            print_warning_msg(lineData, "Immediate operands are stored in 8 bits, can only support values from -127 to 127");
        }
    }
    /* if operand starts with 'r' it's a register */
    else if(operand[0] == 'r'){ 
        str_to_num(&operand[1], &immediate);
        /* check if the register number is valid (r0-r7) */
        if(!(immediate >= 0 && immediate <= 7)){
            print_error_msg(lineData, "Invalid operand, register must be between r0 - r7");
            return FALSE;
        }
    }
    /* its a symbol */
    else{ 
    	remove_white_spaces(operand);
        dot = strchr(operand, '.');     

        /* if it's a normal symbol just check if it's valid */
        if(!dot){
            return(is_symbol_valid(lineData, operand, strlen(operand)));
        }
        /* if the operand has '.' its a struct, check the indexing */
        else{
            str_to_num(dot + 1, &immediate);
            if(!(immediate >= 1 && immediate <= 2)){
                print_error_msg(lineData, "Invalid operand, struct index can only be 1 or 2");
                return FALSE;
            }
        }
    }

    return TRUE;
}


INSTRUCTION_TYPE get_instruction_type(char *instruction){
    int i = 0;
    int instLen = 0;

    instruction += skip_white_spaces(instruction);

    if(instruction[0] != '.') return CODE_INST;     /* if the instruction doesn't start with '.', it's a code instruction*/
    
    /* iterate over the instruction types array */
    while(i < NUM_INST_TYPES){
        instLen = strlen(instructionTypes[i]);
        if(!strncmp(instruction, instructionTypes[i], instLen)) break;      /* if found a match */
        i++;
    }
    
    /* if it is an instruction but we couldn't find a match*/
    if(instruction[0] == '.' && i == NUM_INST_TYPES) return NONE_INST;
    
    /* return the instruction type */
    switch (i){
        case 0:
            return DATA_INST;
        case 1:
            return STRING_INST;
        case 2:
            return STRUCT_INST;
        case 3:
            return ENTRY_INST;
        case 4: 
            return EXTERN_INST;
        default:
            return NONE_INST;
            break;
    }
}


OPCODE get_opcode(char *instruction){
    int i = 0, instLen = 0;

	instruction += skip_white_spaces(instruction);

    if(!instruction[0]) return NONE_OP;     /* if empty instruction*/

    /* iterate over the instruction names array */
    while(i < NUM_COMMAND){
        instLen = strlen(instructionNames[i]);

        /* if we found a match */
        if(!strncmp(instruction, instructionNames[i], instLen)){
            break;
        }
        i++;
    }

    /* return the instruction opcode (according to page 18 in the booklet) */
    switch(i){
        case 0:
           return MOV_OP;
        case 1:
            return CMP_OP;
        case 2:
            return ADD_OP;
        case 3:
            return SUB_OP;
        case 4: 
            return NOT_OP;
        case 5:
            return CLR_OP;
        case 6:
            return LEA_OP;
        case 7:
            return INC_OP;
        case 8:
            return DEC_OP;
        case 9:
            return JMP_OP;
        case 10:
            return BNE_OP;
        case 11:
            return GET_OP;
        case 12:
            return PRN_OP;
        case 13:
            return JSR_OP;
        case 14:
            return RTS_OP;
        case 15:
            return HLT_OP;
        default:
            return NONE_OP;
    }
}


int get_num_operands(OPCODE op){
    /* return the number of expected operands (according to pages 20-20in the booklet) */
    switch(op){
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP:
            return 2;
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case GET_OP:
        case PRN_OP:
        case JSR_OP:
            return 1;
        case RTS_OP:
        case HLT_OP:
            return 0;
        default:
            return 0;
    }
}


ADDRESSING_TYPE get_addressing_type(char *operand){
    /* if nothing, just return none */
	if (!operand || !operand[0]) return NONE_ADDR;

	/* if the first char is 'r', it's a register */
	if (operand[0] == 'r') return REGISTER_ADDR;

    /* if the first char is '#', it's immediate addressing */
	if (operand[0] == '#') return IMMEDIATE_ADDR;

    /* only struct addressing and direct addressing left,
     * if the symbol has the character '.'  it's struct addressing
     * else it a direct addressing
     */
	if(strchr(operand, '.')){
        return STRUCT_ADDR;
    }else{
        return DIRECT_ADDR;    
    }
}


void free_operands(char **operands, int numOfOperands){
    if(numOfOperands >= 1){
        free(operands[0]);
        if(numOfOperands == 2){
            free(operands[1]);
        }
    }
}