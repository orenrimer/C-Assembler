#include "headers/output_manager.h"


BOOL write_files(char *fileName, WORD_BLOCK **codeImg, Entry *symbolList, long *dataImg, unsigned int totalIC,  unsigned int IC, unsigned int DC){
    /* write all the output files */
    return write_ob(fileName, codeImg, dataImg, totalIC, IC, DC) && 
        write_ent_ext(fileName, symbolList, ENT_ENTRY, "ent") && 
        write_ent_ext(fileName, symbolList, EXTERNAL_ADDRESS, "ext");

}


BOOL write_ent_ext(char *fileName, Entry *symbolList, ENTRY_TYPE entryType, char *extension){
	FILE *outputFile;
    Entry *entEntries = NULL;   /* temp list to store all the entries of the given type */
    char *fullName = NULL, key[MAX_SYMBOL_LEN + 1], base32[NUM_BITS_IN_32 + 1];
    long count = 0;    /**/
    long val = 0;
    int i = 0;


    /* we want to only print internals and external symbols */
    if(entryType != ENT_ENTRY && entryType != EXTERNAL_ADDRESS){
        printf("Unsupported Entry type, cannot create output file.\n");
        return FALSE;
    }

    /* get all the entries of the given type from the symbol list */
    entEntries = list_get_entries_by_type(*symbolList, entryType, &count);

    /* if there are no entries, no need to create the files */
    if(count == 0) return TRUE;

	/* allocate space for the extention */
    if (!(fullName = (char*) malloc((strlen(fileName) + 4) * sizeof(char)))){
        MALLOC_ERROR();
    }

    /* append the given extension to the end of the file */
    sprintf(fullName, "%s.%s", fileName, extension);

    /* open the file */
	if (!(outputFile = fopen(fullName, "w"))) {
		printf("Error: Can not write to file %s, Exiting program.\n", fullName);
        free(fullName);
		return FALSE;
	}

    /* iterate over the entries */
    for (i = 0; i < count; i++){
        if(!entry_get_val(entEntries[i], &val) || !entry_get_key(entEntries[i], key)){
            return FALSE;
        }

        dec_to_32(val, base32);
        fprintf(outputFile, "%s     %s \n", key, base32);
    }

    free(fullName);
    free(entEntries);
	fclose(outputFile);
	return TRUE;
}


BOOL write_am(char *outPath, char *line, char *macroImg, BOOL replace, long idx){
    FILE *outputFile = NULL;

    /* open the file in append mode, each call we will add a new line */
	if (!(outputFile = fopen(outPath, "a"))) {
		printf("Error: Can not write to file %s, exiting program.\n", outPath);
		return FALSE;
	}

    /*
     * if we are writhing a macro, copy it's content from the macro image array until we reach a null terminator 
     * when we reach a null terminator the current macro is over 
     */
    if(replace){
        while(macroImg[idx]){
            fprintf(outputFile, "%c", macroImg[idx]);
            idx++;
        }
    }else{
        /* if we are not writing a macro, just copy the line as it is */
        fputs(line, outputFile);	
    }

	fclose(outputFile);  /* close the file */
    return TRUE;
}


BOOL write_ob(char *fileName, WORD_BLOCK **codeImg, long *dataImg, unsigned int totalIC, unsigned int IC, unsigned int DC){
	FILE *outputFile;
    unsigned int i, j, address = START_ADDRESS;
    char *fullName = NULL, base32f[NUM_BITS_IN_32 + 1], base32s[NUM_BITS_IN_32 + 1];
	long val;

	/* allocate space for the .ob extention */
    if (!(fullName = (char*) malloc((strlen(fileName) + 4) * sizeof(char)))){
        MALLOC_ERROR();
    }
    
    /* append '.ob' extension to the end of the file path */  
    sprintf(fullName, "%s.ob", fileName);

    /* open the files */
	if (!(outputFile = fopen(fullName, "w"))) {
		printf("Error: Can not write to file %s, Exiting program.\n", fullName);
        free(fullName);
		return FALSE;
	}

	/* convert to base 32 */
    dec_to_32(totalIC, base32f);
    dec_to_32(DC, base32s);

    /* print IC and DC on top */
	fprintf(outputFile, "   %s %s   ", base32f, base32s);

	/* iterate over the code image */
	for(i = 0; i < IC ; i++){
        codeImg[i]->head->src_addressing = (codeImg[i]->head->src_addressing == NONE_ADDR) ? 0 : codeImg[i]->head->src_addressing;
        codeImg[i]->head->dest_addressing = (codeImg[i]->head->dest_addressing == NONE_ADDR) ? 0 : codeImg[i]->head->dest_addressing;

        val = (codeImg[i]->head->opcode << 6) | (codeImg[i]->head->src_addressing << 4) | (codeImg[i]->head->dest_addressing << 2);
        
        /* convert to base 32 */
        dec_to_32(address, base32f);
        dec_to_32(val, base32s);

        /* print the code instruction address and it's encoding */
        fprintf(outputFile, "\n%s       %s", base32f , base32s);

        address++;  /* increment the address */

        /* print the extra words representation */
		for(j = 0; j < codeImg[i]->numExtraWords; j++){
            val = codeImg[i]->data[j];
                   
            /* convert to base 32 */
            dec_to_32(address, base32f);
            dec_to_32(val, base32s);

            fprintf(outputFile, "\n%s       %s", base32f, base32s);
            address++;
        }
	}

	/* iterate over the data image */
	for (i = 0; i < DC; i++){
        /* write value in 10 bits */
		val = dataImg[i] & 0x000003FF;

        /* convert to base 32 */
        dec_to_32(address, base32f);
        dec_to_32(val, base32s);

        fprintf(outputFile, "\n%s       %s", base32f, base32s);
        address++;
	}
   
    free(fullName);
	fclose(outputFile);
	return TRUE;
}



void set_file_paths(char *fileName, char *fullName, char* outPath, char *amPath){
    int i = 0;
    char *start = NULL, *baseName = NULL;

    if(!(baseName = (char *) malloc(sizeof(char) * strlen(fileName)))){
        MALLOC_ERROR();
    }

    /* check if the file is inside another folder, for example - input_files/file */
    start = strrchr(fileName, '/');

    /* if it's not start at the beginning */
    if(!start){
        start = fileName;
    }
    /* else skip to the file name */
    else{
        start += 1;
    }

    /* get the base name */
    while(start[i] && start[i] != '.'){
        baseName[i] = start[i];
        i++;
    }

    baseName[i] = '\0';

    sprintf(fullName, "%s.as", fileName);   /* append the .as extension*/
    sprintf(outPath, "out/%s", baseName);   /* get the file output path */
    sprintf(amPath, "%s.am", outPath);      /* get the .am file path */
    free(baseName);
}


void print_error_msg(LINE_DATA *lineData, char *msg){
    printf("ERROR:: %s, line: %u :: %s.\n", lineData->fileName, lineData->lineNum, msg);
}


void print_warning_msg(LINE_DATA *lineData, char *msg){
    printf("WARNING:: %s, line: %u :: %s.\n", lineData->fileName, lineData->lineNum, msg);
}
