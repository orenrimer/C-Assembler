#include "headers/pre_assembler.h"


BOOL handle_line_pre(LINE_DATA *lineData, char *outPath,  char *line, char *macroImg, Entry *macroTable, BOOL *outFlagMacro, unsigned int *idx){
    char macroName[MAX_SYMBOL_LEN + 1], *temp = NULL;
    Entry tempEntry = NULL;
    long val = 0; 
    int macroLen = 0;

    /* if the current line is a macro definition */
    if(has_macro(line)){
        /* if we have nested macro */
        if(*outFlagMacro){
            print_error_msg(lineData, "Can not handle nested macros");
            return FALSE;
        }else{
            line = get_next_word(line);     /* skip to the macro name */
            if(!is_macro_valid(lineData, line, &macroLen)) return FALSE;    /* check if the macro name is valid */

            strncpy_safe(macroName, MAX_SYMBOL_LEN + 1, line, macroLen);    /* if the name is valid copy it */

            /* check if we already have a macro with this name */
            if((tempEntry = list_get_entry(*macroTable, macroName, 1, MACRO_ENTRY)) != NULL){
                print_error_msg(lineData, "Macro is already defined");
                *outFlagMacro = FALSE;
                return FALSE;
            }
            
            list_insert(macroTable, macroName, *idx, MACRO_ENTRY);      /* store the macro */
            *outFlagMacro = TRUE;    /* we now want to store the macro content, so we turn on the flagMacro flag to signal the we are currently inside a macro defintion */
        }
    }
    /* if there is an 'open' macro definition */
    else if(*outFlagMacro){
        temp = line + skip_white_spaces(line);
        
        /* if the current line is endmacro line */
        if(temp[0] && !(strncmp(temp, MACRO_END, skip_word(temp)))){
            macroImg[(*idx)++] = '\0';      /* add null terminator so we know when each macro ends */
            *outFlagMacro = FALSE;          /* close the macro */
        }
        /* the current line is part of the macro content so store it inside the macro image array */
        else{
            store_macro_content(line, macroImg, idx);
        } 
    }
    /* regular line */
    else{
        /* if the current line has a macro call */
        if(is_macro_name(line, macroTable, &val)){
            write_am(outPath, line, macroImg, TRUE, val);   /* replace the macro call with the macro content we stored inside the macro image array */
            
            line = get_next_word(line);     /* can not have other text after call to macro  */
            if(line[0]){
                print_error_msg(lineData, "Extraneous text after call to macro");
                return FALSE;
            }
        }
        /* else, just copy the line as it is */
        else{
            write_am(outPath, line, macroImg, FALSE, 0);
        }
    }

    return TRUE;
}



BOOL has_macro(char *line){
    int len = 0;

    line += skip_white_spaces(line);    /* skip to real start */
    len = skip_word(line);  /* get the first word len*/

    if(!line[0]) return FALSE;  

    /* macro has to be the first word */
    if(!strncmp(line, MACRO_START, len)) return TRUE;

    return FALSE;
}


BOOL is_macro_valid(LINE_DATA *lineData, char *macroName, int *outLen){
    int macroLen = 0;

    macroName += skip_white_spaces(macroName); 
    macroLen = skip_word(macroName);    /* get the macro name length */

    /* check if the name is valid */
    if(!is_symbol_valid(lineData, macroName, macroLen)) return FALSE;

    macroName = get_next_word(macroName);   /* can bot have text after macro name */
    if(macroName[0]){
        print_error_msg(lineData, "Extraneous text after macro name");
        return FALSE;
    }

    *outLen = macroLen;     /* update the macro name length*/
    return TRUE;
}


void store_macro_content(char *line, char *macroImg, unsigned int *macroCount){
    int i = 0, count = 0;
 
	for(i = 0 ; line[i] ; i++) {
		macroImg[(*macroCount)++] = line[i];   /* copy each character in the line to the macro image array */
        count++;
	}
}


BOOL is_macro_name(char *line, Entry *macroTable, long *outVal){
    int len = 0;
    Entry temp = NULL;
    char key[MAX_SYMBOL_LEN + 1];   /* temp array to store the macro name */
                                     
    line += skip_white_spaces(line);
    len = skip_word(line);  /* get the first word len */

    if(len <= 0 || len > MAX_SYMBOL_LEN) return FALSE;  /* if the line is longer than the MAX_SYMBOL_LEN it cannot be a macro name (we check before we save if the name is valid) */

    strncpy_safe(key, MAX_SYMBOL_LEN + 1, line, len);   /* copy the name inside the temp array*/
    
    /* check if the name is indeed a macro name, if so get it's value */
    if((temp = list_get_entry(*macroTable, key, 1, MACRO_ENTRY))){
        entry_get_val(temp, outVal);
        return TRUE;
    }

    return FALSE;
}