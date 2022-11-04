#include "headers/pre_assembler.h"
#include "headers/first_pass.h"
#include "headers/second_pass.h"
#include "headers/output_manager.h"


/** 
 * handle the current file
 *
 * @param fileName the name of the file
 * @return TRUE if file handled successfully, otherwise FALSE
 */
BOOL handle_file(char *fileName);


/** 
 * read the current file line by line
 *
 * @param inputFile the current file we are processing 
 * @param lineData a file data object
 * @param outPath the output path where we will write the files too
 * @param symbolList a pointer the the symbol list where we will store all the symbols / macros
 * @param dataImg a pointer to the dataImg array
 * @param codeImg a pointer to the codeImg array
 * @param macroImg a pointer to the macroImg array
 * @param IC a pinter to the instruction counter
 * @param DC a pinter to the data counter
 * @param L a pointer to the extraWords counter
 * @param pass the stage of we are reding the current file, 0 - pre assembler , 1- first pass, 2 - second pass
 * @return: TRUE if there were no errors while reading the file, otherwise FALSE
 */
BOOL read_file(FILE *inputFile,  LINE_DATA *lineData, char *outPath, Entry *symbolList, long *dataImg, WORD_BLOCK **codeImg, char *macroImg, unsigned int *IC, unsigned int *DC, unsigned int *L, int pass);


/** 
 * frees the code words stored in the code img array 
 * @param codeImg a pointer to the codeImg array
 * @param IC a pinter to the instruction counter of the first pass
 */
void free_code_image(WORD_BLOCK **code_image, unsigned int IC);


/** 
 * sets up all the file paths and file data object
 * @param lineData a pointer to the file data object that stores the file name and current line number
 * @param fileName the file name 
 * @param filePath a pointer to the array where we will store the full file path
 * @param outPath a pointer to the array where we will store the output folder path
 * @param amPath a pointer to the array where we will store the '.am' file path
 */
void file_setup(LINE_DATA *lineData, char *fileName, char *filePath, char *outPath, char *amPath);


/** 
 * frees all the dynamicly allocated object and closes the file
 * @param currFile a pointer to the current file
 * @param symbolList a pointer the the symbol table
 * @param macroList a pointer the the macro table
 * @param codeImg a pointer to code image array
 * @param IC the instruction counter of the first pass
 */
void file_cleanup(FILE *currFile, Entry symbolList, Entry macroList, WORD_BLOCK **codeImg, unsigned int IC);



/** 
 * the main function
 */
int main(int argc, char *argv[]){
    int i = 1;  

    /* the user have to pass at least one input file */
    if(argc < 2){
        printf("Error:: You must provide an input file\\s name.\n");
        exit(1);
    }

    /* loop trough the given file one by one, handle them one at a time */
    while(i < argc){
        handle_file(argv[i++]);  
        printf("\n\n\n\n");
    }
    
    return 0;
}


BOOL handle_file(char *fileName){
    FILE *currFile = NULL;      /* a pointer to the current file we ara handling */
    Entry symbolList = NULL, macroList = NULL;  /* list objects to store the symbols and macros */
    LINE_DATA lineData;     /* a lineData object that stores the current file path and line number */
    unsigned int IC = 0, DC = 0, secondIC = 0, L = 0, totalIC = 0; /* counters for the IC, DC, second pass IC, extra words, and total IC */
    char filePath[FILENAME_MAX + 4], outPath[FILENAME_MAX + 5], amPath[FILENAME_MAX + 4];   /* char arrays to store all the relevant paths */
    long dataImg[MAX_IMG_SIZE];    /* data image array, stores all the data instructions content */
    char macroImg[MAX_IMG_SIZE];   /* macro image array, stores all the macro content */
    WORD_BLOCK *codeImg[MAX_IMG_SIZE];  /* code image array, store WORD_BLOCK objects */
    

    /* set up the file pates */
    file_setup(&lineData, fileName, filePath, outPath, amPath);

    /* if can not open the given file */
    if (!(currFile = fopen(filePath, "r"))){ 
        printf("Error:: Can not read file \"%s\", file is missing.\n", filePath);     
        return FALSE;
    }

    printf("##################################################################################################################\n");
    printf("                                      Opening file: '%s'\n", filePath);
    printf("##################################################################################################################\n");

    printf("Starting the pre-assembler stage:\n");

    /* start pre-assembler stage */
    if(!read_file(currFile, &lineData, amPath, &macroList, dataImg, codeImg, macroImg, &IC, &DC, &L, 0)){
        printf("Error in pre-assembler stage, can not go to first pass.\n");
        return FALSE;
    }

    printf("Pre-assembler successful, continue to first pass.\n");
    DC = 0; /* reset the DC for the first stage */
    if(!strncpy_safe(lineData.fileName, FILENAME_MAX + 1, amPath, FILENAME_MAX)) return FALSE;    /* update the file name to the .am file, from now on we will be reading from it */

    /* open the .am file that we have create in the pre-assembler stage */
    if (!(currFile = fopen(amPath, "r"))){ 
        printf("Error:: Can not read file \"%s\", file is missing.\n", filePath);     
        return FALSE;
    }

   /* start first pass */
    if(!read_file(currFile, &lineData, filePath, &symbolList, dataImg, codeImg, macroImg, &IC, &DC, &L, 1)){
        printf("Error in first pass, can not go to second pass.\n");
        return FALSE;
    }

    printf("First pass successful, continue to second pass.\n");

    /* IC counts only the instructions themself (no extra words), 
    *  the totalIC counts the total number of instruction including the extra words created by each instruction.
    */
    totalIC = IC + L;

    L = 0;  /* reset the extra words counter */

    /* add the data entries the totalIC, as seen at stage 17 (page 33 in the booklet) */
    if(!list_update_entry(symbolList, totalIC, DATA_ENTRY)) return FALSE;   

    /* return to the start of the file */
    rewind(currFile);    

    /* start second pass */
    if(!read_file(currFile, &lineData, filePath, &symbolList, dataImg, codeImg, macroImg, &secondIC, &DC, &L, 2)){
        printf("Error in second pass, can not write output files.\n");
        return FALSE;
    }

    printf("Second pass successful, writing files.\n");

    /* write the .ob file and the .ent .ext files (if needed) */
    if(!write_files(outPath, codeImg, &symbolList, dataImg, totalIC, IC, DC)){
        printf("Error while writing files.\n");
        return FALSE;
    } 
    
    file_cleanup(currFile, symbolList, macroList, codeImg, IC);             /* clean up the symbol list and close the file */
    return TRUE;    /* no errors in file */
}


BOOL read_file(FILE *inputFile,  LINE_DATA *lineData, char *outPath, Entry *symbolList, long *dataImg, WORD_BLOCK **codeImg, char *macroImg, unsigned int *IC, unsigned int *DC, unsigned int *L, int pass){
    char currLine[MAX_LINE_LEN + 2], temp;
    BOOL isValid = TRUE, flagMacro = FALSE;

    lineData->lineNum = 1;      /* we start reading from the first line */

    /* read file line by line */
    while(fgets(currLine, MAX_LINE_LEN + 2, inputFile)){
        /* if line is too long, print a warning and skip to the next line */
        if(!(strchr(currLine, '\n')) && !(feof(inputFile))){
            print_warning_msg(lineData, "Line is too long, skipping to the next line");
            /* clear buffer for the next line */
            do{
				temp = fgetc(inputFile);
			}while(temp != '\n' && temp != EOF);
        }
        /* else if the line is not to long, proccess it */
        else{
            switch(pass){
                case 0:     /* pre assembler */
                    if(!handle_line_pre(lineData, outPath, currLine,macroImg, symbolList, &flagMacro, DC)) isValid = FALSE;
                    break;
                case 1:     /* first pass */
                    if(!handle_line_first(lineData, currLine, symbolList, dataImg, codeImg, IC, DC, L)) isValid = FALSE;
                    break;
                case 2:     /* second pass */
                    if (!handle_line_second(lineData, currLine, codeImg, symbolList, IC, L)) isValid = FALSE;
                    break;
                default:
                    return FALSE;
            }
        }

        (lineData->lineNum)++;  /* update line counter */  
    }

    return isValid; /* if there were no errors in the file isValid = TRUE, else isValid = FALSE */
}




void file_setup(LINE_DATA *lineData, char *fileName, char *filePath, char *outPath, char *amPath){
    set_file_paths(fileName, filePath, outPath, amPath);    /* set up all the file paths */
    strncpy_safe(lineData->fileName, FILENAME_MAX+1, filePath, FILENAME_MAX);   /* store the file path inside the LINE_DATA object */
}

void file_cleanup(FILE *currFile, Entry symbolList, Entry macroList, WORD_BLOCK **codeImg, unsigned int IC){
    free_code_image(codeImg, IC);   /* free the code image array*/    
    list_delete(macroList);         /* free the macro list */
    list_delete(symbolList);        /* free the symbol list */
    fclose(currFile);               /* close the current file */
}



void free_code_image(WORD_BLOCK **codeImg, unsigned int IC){
    unsigned int i = 0;
    
    /* iterate through the code image array */
    for(i = 0 ; i < IC ; i++){
        free(codeImg[i]->data);     /* free the data words array */
        free(codeImg[i]->head);     /* free the code word */
    }
}
