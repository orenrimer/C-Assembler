#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utils.h"
#include "globals.h"


#ifndef _Entry_H
#define _Entry_H



typedef struct entry* Entry;



/* entries type */
typedef enum entry_type{
    MACRO_ENTRY = 0,        
    DATA_ENTRY = 1,        
    CODE_ENTRY = 2,         /* for instructions labels */
    ENT_ENTRY = 3,          /* for entry symbols */
    EXTERN_ENTRY = 4,       /* for extern symbols */
    EXTERNAL_ADDRESS = 5,   /* address that contains a reference to an external symbol */

    NONE_ENTRY = -1
}ENTRY_TYPE;




/** 
 * inserts a new symbol entry to the end of the list
 * 
 * @param head a pointer to the list head
 * @param key the symbol
 * @param val the value
 * @param ENTRY_TYPE the symbol type (macro / data / code / entry / extern / external address )
 * @return TRUE is insertion was successful, otherwise FALSE 
 */
BOOL list_insert(Entry *head, char *key, long val, ENTRY_TYPE entryType);


/** 
 * frees the symbol list
 * 
 * @param head the list head
 * @return TRUE is the list was freed successfully, otherwise FALSE 
 */
BOOL list_delete(Entry head);


/** 
 * return an entry from the list
 * 
 * @param head the list head
 * @param key the entry key
 * @param numEntryTypes number of relevant entries types
 * @param ... all the matching entry types
 * @return The matching entry, if no entry found return NULL
 */
Entry list_get_entry(const Entry list, char *key, int numEntryTypes, ...);


/** 
 * return an entry from the list
 * 
 * @param key the entry key
 * @param val the entry value
 * @param entryType entry type
 * @return the new entry, if filed returns NULL
 */
Entry entry_create(char *key, long val, ENTRY_TYPE entryType);


/** 
 * updates all the entries in the list of the given entry type
 * 
 * @param head the list head
 * @param updateValue the value we want to add
 * @param entryType the type of entries we want to update
 * @return TRUE if successful, otherwise FALSE
 */
BOOL list_update_entry(const Entry head, long updateValue, ENTRY_TYPE entryType);


/** 
 * return the given entry value
 * 
 * @param entry the entry 
 * @param outVal out paramater to store the value
 * @return TRUE if successful, otherwise FALSE
 */
BOOL entry_get_val(const Entry entry, long *outVal);


/** 
 * return the given entry key
 * 
 * @param entry the entry 
 * @param outKey out paramater to copy the key
 * @return TRUE if successful, otherwise FALSE
 */
BOOL entry_get_key(const Entry entry, char *outKey);


/** 
 * return the type of a given entry
 * 
 * @param entry the entry 
 * @return the entry type
 */
ENTRY_TYPE entry_get_type(const Entry entry);


/** 
 * return a list of all the entries in the symbol list that match the given entry type
 * 
 * @param entry the list head
 * @param entryType the type of entry we are looking for
 * @param outCount out paramater to get the count
 * @return a list of all the entries
 */
Entry *list_get_entries_by_type(const Entry list, ENTRY_TYPE entryType, long *outCount);


#endif