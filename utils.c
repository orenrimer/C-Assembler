#include "headers/utils.h"



char baseSymbols[32]={'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};



void dec_to_32(int num, char *base32){    
    int i = NUM_BITS_IN_32 - 1, temp = 0;
    unsigned int abs = num;
    
    if(!base32) return;

    /* convert the number from decimal to base 32 */
    while(i >= 0){
        temp = (abs % 32);     /* keep dividing by 32, then storing the remainder (in reverse order) */
        base32[i] = baseSymbols[temp];
        abs = abs/32;
        i--;
    }

    base32[2] = '\0';   /* null terminate */
}


int skip_white_spaces(char *line){
    int i = 0;
   
    /* continue counting while we reach a char that isn't a white space */
    while(line[i] && (isspace(line[i]) || line[i] == '\t' || line[i] == '\n')){
        i++;
    }

    /* return the pointer the the first real char we see*/
    return i;
}


int skip_word(char *word){
    int i = 0;
   
    /* continue counting while we reach a char that isn't a white space */
    while(word[i] && !isspace(word[i]) && word[i] != '\t' && word[i] != '\n' && word[i] != ':' && word[i] != ','){
        i++;
    }

    return i; 
}


char *get_next_word(char *str){
    char *temp = NULL;

    str += skip_white_spaces(str);  /* get to the start of the current word */
    temp = str + skip_word(str);    /* skip the current word */
    return temp + skip_white_spaces(temp);     /* skip white spaces, get to the start of the next word */
}


BOOL str_to_num(char *str, long *outVal){
    char *end = NULL;               
    long temp = 0;

    remove_white_spaces(str);   
    temp = strtol(str, &end, 10);   /* strtol to convert to a long */

    if(end[0]) return FALSE;        /* if there are any parts left, the conversion stoped in the middle */
    if(outVal) *outVal = temp;      /* if all valid, store to converted number inside the out paramater*/

    return TRUE;
}


void remove_white_spaces(char *str) {
    int i = 0;
    int newIdx = 0;         /* the last index that is not a white space */

    while (str[i] != '\0'){
        if(!isspace(str[i]) && str[i] != '\t' && str[i] != '\n'){
            /* move up the "real" characters to the front of the string */
            str[newIdx] = str[i];
            newIdx++;
        }

        i++;
    }

    /* end the string, the rest is white spaces */
    str[newIdx] = '\0';
    return;
}


BOOL strncpy_safe(char *dest, size_t lenDst, const char *src, size_t lenSrc){
    /* check if any of the strings is null */
    if (dest == NULL || src == NULL) return FALSE;

    /* validate buffer space */
    if(lenSrc >= lenDst){
        printf("ERROR:: failed to copy string, sourc string is too large.\n");
        return FALSE;
    }

    /* copy and add null-terminator */
    memcpy(dest, src, lenSrc);
    *(dest + lenSrc) = '\0';
    return TRUE;
}
