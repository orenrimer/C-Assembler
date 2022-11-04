#include "headers/first_pass.h"



BOOL handle_line_first(LINE_DATA *lineData, char *line, Entry *list, long *dataImg, WORD_BLOCK **codeImg, unsigned int *outIC, unsigned int *outDC, unsigned int *outL){
    INSTRUCTION_TYPE insType = NONE_INST;
    char *symbolEnd = NULL, symbol[MAX_SYMBOL_LEN + 1];   /* char array to store the symbol */
    BOOL flagSymbol = FALSE, flagError = FALSE;
    int symbolLen = 0;

    /* real start */
    line += skip_white_spaces(line);
    
    /* empty line or comment line */
    if(!line[0] || line[0] == ';') return TRUE;

    /* check if there is a symbol */
    symbolEnd = strchr(line, ':');

    /* if so, check if it is valid and save it */
    if(symbolEnd){
        symbolLen = symbolEnd - line;

        /* check if the symbol is valid */
        if(!is_symbol_valid(lineData, line, symbolLen)) flagError = TRUE;

        else{
            strncpy_safe(symbol, MAX_SYMBOL_LEN + 1, line, symbolLen);    /* if symbol valid, store the symbol */
            flagSymbol = TRUE;  
        }

        line = symbolEnd + 1;      /*skip the symbol */
    }   

    line += skip_white_spaces(line);

    /* symbol only line, skip it */
    if (!line[0]) return TRUE; 

    /* get the instruction type */
    insType = get_instruction_type(line);

    if(insType == NONE_INST){
        print_error_msg(lineData, "Invalid instruction type, instruction type has to come after '.' with no spaces");
        return FALSE;
    } 

    /* skip over the '.data' / '.string' / '.struct' */
    if(insType != CODE_INST) line += skip_word(line);

    if(insType == DATA_INST || insType == STRING_INST || insType == STRUCT_INST){
        /* if we found a symbol at this line, save the symbol as a data symbol */
        if(flagSymbol){
            /* if the symbol is already defined */
            if(list_get_entry(*list, symbol, 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY) != NULL){
                print_error_msg(lineData, "Can not add symbol, entry already exists");
                flagError = TRUE;  
            }else{
                /* store the symbol as a data symbol with the value DC */
                if(!list_insert(list, symbol, *outDC, DATA_ENTRY)) flagError = TRUE;
            }
        } 

        if(!handle_instruction(lineData, line, insType, dataImg, outDC)) flagError = TRUE;
    }else if(insType == EXTERN_INST || insType == ENTRY_INST){
        /* if there is a symbol before a extern or entry definiton , print a warning */
        if(flagSymbol) print_warning_msg(lineData, "Label with an extern \\ entry instruction is meaningless.");
        /* check if entry defintion is valid */
        if(insType == ENTRY_INST){
            if(!handle_entry_instruction(lineData, line)) flagError = TRUE;
        }
        else if(insType == EXTERN_INST){
            if(!handle_extern_instruction(lineData, line, symbol)) flagError = TRUE;

            if(list_get_entry(*list, symbol, 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY) != NULL){
                print_error_msg(lineData, "Can not add symbol, entry already exists");
                flagError = TRUE;  
            }
            else{
                /* store the symbol as a external symbol with the value 0 */
                if(!list_insert(list, symbol, 0, EXTERN_ENTRY)) flagError = TRUE;
            } 
        }
    /* if it's a code instruction */
    }else{
        /* if there is a symbol */
        if(flagSymbol){
                /* if the symbol is already defined */
               if(list_get_entry(*list, symbol, 3, DATA_ENTRY, CODE_ENTRY, EXTERN_ENTRY) != NULL){
                print_error_msg(lineData, "Can not add symbol, entry already exists");
                flagError = TRUE;  
            }else{
                /* store the symbol as a code symbol with the value total IC (including extra words) */
                if(!list_insert(list, symbol, (*outIC) + (*outL), CODE_ENTRY)) flagError = TRUE;
            }
        }

        if(!handle_code_instruction(lineData, line, codeImg, outIC, outL)) flagError = TRUE;
    }

    return !flagError;  /* if there are no errors return TRUE, otherwise false */
}


BOOL handle_instruction(LINE_DATA *lineData, char *instruction, INSTRUCTION_TYPE insType, long *data_img, unsigned int *outDC){
    if(insType == DATA_INST) return handle_data_instruction(lineData, instruction, data_img, outDC);
    else if(insType == STRING_INST) return handle_string_instruction(lineData, instruction, data_img, outDC);
    else if(insType == STRUCT_INST) return handle_struct_instruction(lineData, instruction, data_img, outDC);
    else return FALSE;
}


BOOL handle_data_instruction(LINE_DATA *lineData, char *instruction, long *data_img, unsigned int *outDC){
    char *p = NULL;     /* temp pointer for strtok */
    long val = 0;
    
    instruction += skip_white_spaces(instruction);

    /* if there are no argument after .data deceleration */
    if(!instruction[0]){
        print_error_msg(lineData, "Expected argument\\s after '.data' deceleration");
    }

    /*
     * check if the line structure is valid before we will parse it. 
     * we are passing -1 as we dont know how many immediate values there are.
     */
    if(!check_line_structure(lineData, instruction, -1)) return FALSE;
    
    remove_white_spaces(instruction);
    p = strtok(instruction, ",");  /* parse the instruction */

    while(p){
        /* convert the immediate values from string to long */
        if(!str_to_num(p, &val)){
            print_error_msg(lineData, "Invalid value, immediate value must be a natural number");
            return FALSE;
        }else if(val > MAX_DATA_VAL || val < MIN_DATA_VAL){
            print_warning_msg(lineData, "Immediate values are stored in 10 bits, can only support values from -1024 to 1023");
        }

        data_img[(*outDC)++] = val;     /* store the value inside the data image array and increment the DC*/
        p = strtok(NULL, ",");          /* continue to the next part */
    }

    return TRUE;
}


BOOL handle_string_instruction(LINE_DATA *lineData, char *instruction, long *data_img, unsigned int *outDC){
    int i, closingIdx = 1;
    char *temp = NULL;

    instruction += skip_white_spaces(instruction);

    /* if there are no argument after .string deceleration */
    if(!instruction[0]){
        print_error_msg(lineData, "Expected argument\\s after '.string' deceleration");
        return FALSE;
    }
    /* if there string doesn't start with quotes */
    else if(instruction[0] != '"'){
        print_error_msg(lineData, "Missing opening quotations");
        return FALSE;
    } 

    /* search for the closing quotes */
    while(instruction[closingIdx] && instruction[closingIdx] != '"') closingIdx++;

    /* there is no closing quotes */
    if(!instruction[closingIdx]){
        print_error_msg(lineData, "Missing closing quotations");
        return FALSE;
    }

    temp = &instruction[closingIdx + 1] + skip_white_spaces(&instruction[closingIdx + 1]);

    /* check if there are more arguments after the closing quotes */
    if(temp[0]){
        print_error_msg(lineData, "Multiple strings definition");
        return FALSE;
    }

	for(i = 1 ; i < closingIdx ; i++) {
		data_img[(*outDC)++] = instruction[i];  /* store each character inside the data image array and increment the DC */
	}

	/* add null terminator */
	data_img[(*outDC)++] = '\0';
	return TRUE;
}


BOOL handle_struct_instruction(LINE_DATA *lineData, char *instruction, long *dataImg, unsigned int *outDC){
    char *p = NULL;  /* temp pointer for strtok */
    long val = 0;

    instruction += skip_white_spaces(instruction);

    /* if there are no argument after .struct deceleration */
    if(!instruction[0]){
        print_error_msg(lineData, "Expected arguments after '.strcut' deceleration");
        return FALSE;
    }

    /* check if the line structure is valid before we will parse it. after struct deceleration we are expecting 2 arguments */
    if(!check_line_structure(lineData, instruction, 2)) return FALSE;

    remove_white_spaces(instruction);

    /* get the first argument which is an immediate value */
    p = strtok(instruction, ",");
    if(!str_to_num(p, &val)){
        print_error_msg(lineData, "Invalid value , immediate value must be a natural number");
        return FALSE;
    } 

    dataImg[(*outDC)++] = val;  /* store the first argument in the data image array, and increment the DC */

    /* get the second argument which is a string */
    p = strtok(NULL, ",");
    if(!handle_string_instruction(lineData, p, dataImg, outDC)) return FALSE;

    return TRUE; 
}


BOOL handle_extern_instruction(LINE_DATA *lineData, char *instruction, char *symbol){
    int symbolLen = 0;  /* the external symbol length */
    char *temp = NULL;

    instruction += skip_white_spaces(instruction);

    /* if there are no argument after .extern deceleration */
    if(!instruction[0]){
        print_error_msg(lineData, "Expected symbol after '.extern' deceleration");
        return FALSE;
    }

    symbolLen = skip_word(instruction);    /* get the symbol length */

    /* check if the symbol is valid */
    if(!is_symbol_valid(lineData, instruction, symbolLen)) return FALSE;

    temp = get_next_word(instruction);
    /* if there are multiple symbols defined */
    if(temp[0]){
        print_error_msg(lineData, "Multiple symbols after '.extern' deceleration ");
        return FALSE;
    } 

    /* if the symbol is valid, save it */
    strncpy_safe(symbol, MAX_SYMBOL_LEN, instruction, symbolLen);
    return TRUE;
}


BOOL handle_entry_instruction(LINE_DATA *lineData, char *instruction){
    int symbolLen = 0;  /* the external symbol length */

    instruction += skip_white_spaces(instruction);

    /* if there are no argument after .extern deceleration */
    if(!instruction[0]){
        print_error_msg(lineData, "Expected symbol after '.entry' deceleration");
        return FALSE;
    }

    symbolLen = skip_word(instruction);    /* get the symbol length */

    /* check if the symbol is valid */
    if(!is_symbol_valid(lineData, instruction, symbolLen)) return FALSE;

    instruction = get_next_word(instruction);
    /* if there are multiple symbols defined */
    if(instruction[0]){
        print_error_msg(lineData, "Multiple symbols after '.entry' deceleration ");
        return FALSE;
    } 

    return TRUE;
}


BOOL handle_code_instruction(LINE_DATA *lineData, char *instruction,WORD_BLOCK **codeImg, unsigned int *outIC, unsigned int *outL){
    OPCODE op = NONE_OP;
    CODE_WORD *codeWord = NULL;     /* the CODE_WORD object where we will store the original instruction fields */
    WORD_BLOCK *word = NULL;              /* the WORD_BLOCK object that will store the original code instruction and the extra data words if needed */
    int numOfOperands = 0, numOfExtraWords = 0; 
    char *operands[2] = {NULL};     /* a char pinter array that will store the instruction operands, an instruction has at max 2 operands */


    op = get_opcode(instruction);   /* recognize the instruction by its opcode */
    /* if we couldnt recognize the instruction */
    if(op == NONE_OP){
        print_error_msg(lineData, "Invalid instruction name");
        return FALSE;
    }

    numOfOperands = get_num_operands(op);   /* get the number of the expected operands */

    /* allocate space to store each operand */
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

    instruction = get_next_word(instruction);   /* skip to the operands part */
    if(!check_line_structure(lineData, instruction, numOfOperands)) return FALSE;   /* check if the line structure is valid before we will parse it */
    
    /* check if the operands are valid and copy them to the operands array */
    if(!get_operands(lineData, instruction, operands, TRUE)){
        free_operands(operands, numOfOperands);
        return FALSE;
    }

    /* build the code word */
    if(!(codeWord = build_code_word(operands, op, numOfOperands, &numOfExtraWords))){
        free_operands(operands, numOfOperands);
        return FALSE;
    }

    /* check if the operands are matching the instruction (according to the table at page 32 in the booklet) */
    if(!check_matching_operands(codeWord->src_addressing, codeWord->dest_addressing, op)){
        print_error_msg(lineData, "Incompatible operand type to instruction");
        free_operands(operands, numOfOperands);
        return FALSE;
    }

    /* 
     * build the word object, for now we will only store the code instruction and the number of extra words.
     * in the second pass we will store the extra data words
     */
    if(!(word = build_word(codeWord, numOfExtraWords))){
        free_operands(operands, numOfOperands);
        return FALSE;
    }
    
    (*outL) += word->numExtraWords;     /* update the extra words counter */
    codeImg[(*outIC)++] = word;         /* store the WORD_BLOCK object inside the code image array and increment the IC*/

    free_operands(operands, numOfOperands);     /* free the operands array */
    return TRUE;
}



CODE_WORD * build_code_word(char **operands, OPCODE op, int numOfOperands, int *numOfExtraWords){
    CODE_WORD *codeWord = NULL; 
    ADDRESSING_TYPE firstOperandAddressing = NONE_ADDR, secondOperandAddressing = NONE_ADDR;

    if(!(codeWord = (CODE_WORD *) malloc(sizeof(CODE_WORD)))){
        MALLOC_ERROR();
    }

    codeWord->opcode = op;  /* set the code word opcode */

    /* get the addressing type of each operand */
    firstOperandAddressing = get_addressing_type(operands[0]);
    secondOperandAddressing = get_addressing_type(operands[1]);

    if(numOfOperands == 1){
        codeWord->src_addressing = NONE_ADDR;   /* if there is only one operand, it will be the destination operand */
        codeWord->dest_addressing = firstOperandAddressing;
    }else{
        codeWord->src_addressing = firstOperandAddressing;
        codeWord->dest_addressing = secondOperandAddressing;
    }
    
    return codeWord;
}



WORD_BLOCK * build_word(CODE_WORD *head, int numOfExtraWords){
    WORD_BLOCK *word = NULL;

    if(!(word = (WORD_BLOCK *) malloc(sizeof(WORD_BLOCK)))){
        MALLOC_ERROR();
    }

    word->head = head;  /* store a pointer to the original code word */
    
    /* allocate an array of integers to store the extra data words */
    if(!(word->data = (long *) malloc(numOfExtraWords * sizeof(long)))){
        free(word);
        MALLOC_ERROR();
    }
    
    word->numExtraWords = count_extra_words(head->src_addressing, head->dest_addressing);     /* how many extra words */
    return word;
}


int count_extra_words( ADDRESSING_TYPE srcAddressing, ADDRESSING_TYPE destAddressing){
    int count = 0;

    /* immediate addressing operands creates one extra words each */
    if(srcAddressing == IMMEDIATE_ADDR) count += 1;
    if(destAddressing == IMMEDIATE_ADDR) count += 1;

    /* direct addressing operands creates one extra words each */
    if(srcAddressing == DIRECT_ADDR) count += 1;
    if(destAddressing == DIRECT_ADDR) count += 1;

    /* struct addressing operands creates two extra words each */
    if(srcAddressing == STRUCT_ADDR) count += 2;
    if(destAddressing == STRUCT_ADDR) count += 2;

    /* register addressing operands creates one extra words */
    if(srcAddressing == REGISTER_ADDR || destAddressing == REGISTER_ADDR) count += 1;

    return count;
}


BOOL check_matching_operands(ADDRESSING_TYPE src, ADDRESSING_TYPE dest, OPCODE opcode){
    if((opcode == MOV_OP || opcode == ADD_OP || opcode == SUB_OP) && dest == IMMEDIATE_ADDR) return FALSE;

    if((opcode == NOT_OP || opcode == CLR_OP || opcode == INC_OP ||opcode == DEC_OP || opcode == JMP_OP || opcode == BNE_OP || opcode == GET_OP || opcode == JSR_OP) 
    && (src != NONE_ADDR || dest < DIRECT_ADDR)) 
            return FALSE;

    if(opcode == LEA_OP && (src == IMMEDIATE_ADDR || src == REGISTER_ADDR || dest == IMMEDIATE_ADDR)) return FALSE;

    if((opcode == HLT_OP || opcode == RTS_OP) && ((src != IMMEDIATE_ADDR  && src != NONE_ADDR) || (dest != IMMEDIATE_ADDR  && dest != NONE_ADDR))) return FALSE;

    return TRUE;
}


BOOL check_line_structure(LINE_DATA *lineData, char *line, int numOfArgs){
    int i = 0;
    int wordCounter = 0, commasCounter = 0, lastWordEnd = 0, lastCommaIdx = 0, lastWordStart = 0;

    line += skip_white_spaces(line);
    if(!line[0] && numOfArgs <= 0) return TRUE;
    

    /* if the arguments start with a comma*/
    if(line[0] == ','){
        print_error_msg(lineData, "Illegal comma before operand\\s");
        return FALSE;
    }

    while(line[i]){
        /* if the current character is a comma */
        if(line[i] == ','){
            /* if the last word start was before the last comma, there are consecutive commas    */
            if(lastCommaIdx > lastWordStart){
                print_error_msg(lineData, "Multiple consecutive commas");
                return FALSE;
            }
            /* if the previous char was character wasn't a white space, we finished a word    */
            else if(line[i-1] != '\t' && !isspace(line[i-1])){
                wordCounter += 1;
                lastWordEnd = i - 1;
            }

            commasCounter += 1; /* increment the commas counter */
            lastCommaIdx = i;   /* update the last comma */
        }
        /* if the current character is a white space */
        else if((isspace(line[i]) || line[i] == '\t')){
            /* if the previous char was character was alphanumeric, we finished a word */
            if(!isspace(line[i-1]) && line[i-1] != '\t' && line[i-1] != ',' && line[i-1] != '#'){
                wordCounter += 1;
                lastWordEnd = i - 1;

                /* the number of commas need to be the number of words - 1 */
                if(wordCounter - commasCounter > 1){
                    print_error_msg(lineData, "Missing comma");
                    return FALSE;
                }
            }
        }
        /* if we are a char */
        else{
            /* if the previous char was whitespace, we starting a new word */
            if(isspace(line[i-1]) || line[i-1] == '\t' || line[i-1] == ',' || line[i-1] == '#'){;
                lastWordStart = i;  /* update the last word start index */
            }
        }

        i++;
    }
    

    if(!isspace(line[i-1]) && line[i-1] != '\t' && line[i-1] != ','){
        wordCounter += 1;
        lastWordEnd = i-1;
    } 

    if(wordCounter < numOfArgs){
        print_error_msg(lineData, "Missing arguments\\s");
        return FALSE;
    }else if(numOfArgs != -1 && numOfArgs < wordCounter){
        print_error_msg(lineData, "Too many arguments");
        return FALSE;
    }

    /* if there is unexpected text at the end of the command */
    if(lastCommaIdx > lastWordEnd){
        print_error_msg(lineData, "Extraneous comma after end of command");
        return FALSE;
    } 

    return TRUE;
}
