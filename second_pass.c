#include "headers/second_pass.h"


BOOL handle_line_second(LINE_DATA *lineData, char *line, WORD_BLOCK **code_img, Entry *symbolList, unsigned int *outIC, unsigned int *outL){
    INSTRUCTION_TYPE insType = NONE_INST;
    char *symbolEnd = NULL;

    /* real start */
    line += skip_white_spaces(line);

    /* empty line or comment line - just continue to the next line */
    if(!line[0] || line[0] == ';') return TRUE;

    /* if has symbol, skip it*/
    if((symbolEnd = strchr(line, ':'))){
        line = symbolEnd + 1;
        line += skip_white_spaces(line);
    } 
    
    insType = get_instruction_type(line);   /* get the instruction type */
    line += skip_word(line);               

    /* if .entry we need to save it as an entry symbol */
    if(insType == ENTRY_INST) return markEntry(lineData, line, symbolList);

    /* if code instruction we have to see it there are any extra data words we need to store, and if so handle them */
    else if(insType == CODE_INST) return handle_extra_words(lineData, line, code_img, symbolList, outIC, outL);

    return TRUE;
}


BOOL markEntry(LINE_DATA *lineData, char *line, Entry *symbolList){
    Entry entry = NULL;
    long val = 0;   
     
    remove_white_spaces(line); /* get the symbol, we know it's valid because of the first pass */

    /* check if the symbol is defined in the file */
    if(!(entry = list_get_entry(*symbolList, line, 3, CODE_ENTRY, DATA_ENTRY, EXTERN_ENTRY))){
        print_error_msg(lineData, "Undefined symbol as entry");
        return FALSE;
    }
    /* if it is defined, check that it isn't defined as an external symbol */
    else if(entry_get_type(entry) == EXTERN_ENTRY){
        print_error_msg(lineData, "Entry symbol can not be defined as an external symbol");
        return FALSE;
    }

    /* get the DC value of the symbol */
    if(!entry_get_val(entry, &val)) return FALSE;

    /* store it as an new entry symbol */
    list_insert(symbolList, line, val + START_ADDRESS, ENT_ENTRY);
    return TRUE;
}



BOOL handle_extra_words(LINE_DATA *lineData, char *line, WORD_BLOCK **codeImg, Entry *symbolList, unsigned int *outIC, unsigned int *outL){
    WORD_BLOCK *currWord  = NULL;     /* the current word from the code image */
    int numOfOperands = 0;
    char *operands[2] = {NULL};

    /* get the current word in which you will store the extra data words */
    currWord = codeImg[*(outIC)];

    /* if no extra words needed, just return */
    if(currWord->numExtraWords < 1){
        (*outIC)++;
        return TRUE;
    } 

    /* allocate memory to store the operands */
    numOfOperands = get_num_operands(currWord->head->opcode);
    if(numOfOperands >= 1){
        if(!(operands[0] = (char *) malloc((MAX_LINE_LEN + 1) * sizeof(char)))){
            MALLOC_ERROR();
        }

        if(numOfOperands == 2){
            if(!(operands[1] = (char *) malloc((MAX_LINE_LEN + 1) * sizeof(char)))){
                free(operands[0]);
                MALLOC_ERROR();
            }
        }
    }

    /* get the operands, this time we dont have to check if they are valid */
    get_operands(lineData, line, operands, FALSE);
 
    /* build the extra data words from the operands, each operand value will be stored inside one (or more) data words */
    if(!build_data_words(lineData, currWord, operands, symbolList, outIC, outL)){
        free_operands(operands, numOfOperands);
        return FALSE;
    }

    (*outIC)++;
    (*outL) += currWord->numExtraWords;         /* update the extra words counter */
    free_operands(operands, numOfOperands);     /* free the operands */
    return TRUE;
}



BOOL build_data_words(LINE_DATA *lineData, WORD_BLOCK *word, char **operands, Entry *symbolList, unsigned int *IC, unsigned int *L){
    Entry tempEntry = NULL;
    ENTRY_TYPE entryType = NONE_ENTRY;
    ADDRESSING_TYPE firstOperand = NONE_ADDR, secondOperand = NONE_ADDR;    /* the operands addressing type */
    int wordsCount = 0;         /* count how mant extra words we created */
    long val = 0, data = 0;     /* val will store the raw value from the operand, data will store the modified value we will store inside the extra word (after shifting ect.)  */
    char *temp = NULL;          /* temp pointer used for strtok */


    firstOperand = get_addressing_type(operands[0]);    /* get the first operand addressing type */ 

    /* if immediate addressing */
    if(firstOperand == IMMEDIATE_ADDR){
        /* convert the immediate value from string to long */
        if(!str_to_num(&operands[0][1], &val)) return FALSE; 

        /* 
         * immediate values will be stored in 8 bits, meaning we can only store values between  -127 - 127
         * values that are not in this range will be cut at the 8 lower bits 
         */
        if(val > MAX_IMMED_VAL || val < MIN_IMMED_VAL){
            val &= 0x00000FF;   /* mask only the 8 lower bits */
        }

        data = (val << 2) & (0XFFFFFFFC);   /* shift left by 2 bits (for the ARE bits) and mask to save the sign */
        word->data[wordsCount++] = data;    /* store the final data inside a new data word and increment the word count */
    }
    /* if direct addressing */
    else if(firstOperand == DIRECT_ADDR){
        /* get the symbol from the symbol list */
        if(!(tempEntry = list_get_entry(*symbolList, operands[0], 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY))){
            print_error_msg(lineData, "Operand is an undefined symbol");
            return FALSE;
        } 
        
        if(!(entry_get_val(tempEntry, &data))) return FALSE;  /* get the symbol address */
      
        entryType = entry_get_type(tempEntry);
        if(entryType == NONE_ENTRY){
            return FALSE;
        }
        /* if the operand is the address of a external symbol */
        else if(entryType == EXTERN_ENTRY){
            data = data << 2;   /* make room fore the 2 ARE bits */
            data |= 0x1;        /* if external symbol ARE = 01 */
            list_insert(symbolList, operands[0], (*IC) + wordsCount + START_ADDRESS + (*L) + 1, EXTERNAL_ADDRESS);  /* store the address of the instruction that calls the external symbol */
        }
        /* if the operand is the address of a local symbol */
        else{  
            data = (data + START_ADDRESS) << 2;     /* make room fore the 2 ARE bits */
            data |= 0x2;                            /* if entry symbol ARE = 10*/
        }

        word->data[wordsCount++] = data;    /* store the final data inside a new data word and increment the word count */
    }
    /* if struct addressing */
    else if(firstOperand == STRUCT_ADDR){
        temp = strtok(operands[0], ".");    /* get the first part, which is the symbol name */

        /* get the symbol from the symbol table */
        if(!(tempEntry = list_get_entry(*symbolList, temp, 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY))){
            print_error_msg(lineData, "Operand is an undefined symbol");
            return FALSE;
        } 
        
        if(!(entry_get_val(tempEntry, &data))) return FALSE;    /* get the symbol address */

        entryType = entry_get_type(tempEntry);
        if(entryType == NONE_ENTRY){
            return FALSE;
        }
        /* if the operand is the address of a external symbol */
        else if(entryType == EXTERN_ENTRY){
            data = data << 2;   /* make room fore the 2 ARE bits */
            data |= 0x1;        /* if external symbol ARE = 01 */
            list_insert(symbolList, operands[0], (*IC) + wordsCount + START_ADDRESS + (*L) + 1, EXTERNAL_ADDRESS);  /* store the address of the instruction that calls the external symbol */
        }
        /* if the operand is the address of a local symbol */
        else{  
            data = (data + START_ADDRESS) << 2;     /* make room fore the 2 ARE bits */
            data |= 0x2;                            /* if entry symbol ARE = 10*/
        }

        word->data[wordsCount++] = data;    /* store the final data inside a new data word and increment the word count */

        temp = strtok(NULL, ".");           /* get the second part which is the index */
        if(!str_to_num(temp, &val)) return FALSE;       /* convert to number */

        data = (val << 2) & (0XFFFFFFFC);               /* shift left (for ARE bits) and musk to save the sign */
        word->data[wordsCount++] = data;                /* store the final data inside a new data word and increment the word count */
    }
    /* if register addressing */
    else if(firstOperand == REGISTER_ADDR){
        if(!str_to_num(&operands[0][1], &val)) return FALSE;    /* get the register number */

        /* check if it is target register or source register */
        if(word->head->opcode <=3){
			data = val << 6;    /* if dest store the register number in bits 6-9  */
		}else{
			data = val << 2;   /* if source store the register number in bits 2-5  */
		}
        word->data[wordsCount++] = data;  /* store the final data inside a new data word and increment the word count */
    }

    secondOperand = get_addressing_type(operands[1]);       /* get the second operand addressing type */

    if(secondOperand == IMMEDIATE_ADDR){
        if(!str_to_num(&operands[1][1], &val)) return FALSE;    /* convert the immediate value to long */

        if(val > MAX_IMMED_VAL || val < MIN_IMMED_VAL){
            val &= 0x00000FF;   /* mask only the 8 lower bits */
        }

        data = (val << 2) & (0XFFFFFFFC);   /* shift left by 2 (buts 0-2 are the ARE bits) and mask to keep the sign */
        word->data[wordsCount++] = data;    /* store the final data inside a new data word and increment the word count */
    }else if(secondOperand == DIRECT_ADDR){
        /* get the operand from the symbol list */
        if(!(tempEntry = list_get_entry(*symbolList, operands[1], 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY))){
            print_error_msg(lineData, "Operand is an undefined symbol");
            return FALSE;
        }

        if(!(entry_get_val(tempEntry, &data))) return FALSE;    /* get it's address */

        entryType = entry_get_type(tempEntry);

        if(entryType == NONE_ENTRY){
            return FALSE;
        }else if(entryType == EXTERN_ENTRY){
            data = data << 2;
            data |= 0x1;    /* extern ARE = 01 */
            list_insert(symbolList, operands[1], (*IC) + wordsCount + START_ADDRESS + (*L) + 1, EXTERNAL_ADDRESS);  /* store the address of the instruction that calls the external symbol */
        }else{
            data = (data + START_ADDRESS) << 2;
            data |= 0x2;  /* entry ARE = 10 */
        }

        word->data[wordsCount++] = data;    /* store the final data inside a new data word and increment the word count */
    }else if(secondOperand == STRUCT_ADDR){
        temp = strtok(operands[1], ".");

        if(!(tempEntry =list_get_entry(*symbolList, temp, 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY))){
            print_error_msg(lineData, "Operand is an undefined symbol");
            return FALSE;
        }
        
        if(!(entry_get_val(tempEntry, &data))) return FALSE;
        
        entryType = entry_get_type(tempEntry);

        if(entryType == NONE_ENTRY){
            return FALSE;
        }else if(entryType == EXTERN_ENTRY){
            data = data << 2;
            data |= 0x1;

            list_insert(symbolList, operands[1], (*IC) + wordsCount + START_ADDRESS + (*L) + 1, EXTERNAL_ADDRESS);  /* store the address of the instruction that calls the external symbol */
        }else{
            data = (data + START_ADDRESS) << 2;
            data |= 0x2;
        }

        word->data[wordsCount++] = data;
    
        temp = strtok(NULL, ".");
        if(!str_to_num(temp, &val)) return FALSE; 
        
        data = (val << 2) & (0XFFFFFFFC);
        word->data[wordsCount++] = data;
    }else if(secondOperand == REGISTER_ADDR){
        if(!str_to_num(&operands[1][1], &val)) return FALSE; 

        data = val << 2;

        /* if the src operand was also a register, we need to store both of them inside the same word */
        if(word->head->src_addressing == REGISTER_ADDR){
            word->data[wordsCount-1] |= data;
        }else{
            word->data[wordsCount++] = data;
        }
    }

    return TRUE;
}   








