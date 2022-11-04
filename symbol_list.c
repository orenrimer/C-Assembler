#include "headers/symbol_list.h"


struct entry{
    char *key;
    long val;
    ENTRY_TYPE entryType;
    Entry next;
};





Entry entry_create(char *key, long val, ENTRY_TYPE entryType){
    int keyLen = 0;
    Entry newEntry = NULL;

    if (!key || !key[0]) return NULL;   /* if key is empty */

    keyLen = strlen(key) + 1;          /* how much memory we need to allocate to store the key */

    if(!(newEntry = (Entry) malloc(sizeof(struct entry)))){
        MALLOC_ERROR();
    }

    if (!(newEntry->key = (char *) malloc(sizeof(char) * keyLen))){
        free(newEntry);
        MALLOC_ERROR();
    } 
    
    /* copy the key */
    if(!strncpy_safe(newEntry->key, keyLen, key, keyLen - 1)) return FALSE;

    newEntry->val = val;
    newEntry->entryType = entryType;
    newEntry->next = NULL;
    return newEntry;
}




BOOL list_insert(Entry *head, char *key, long val, ENTRY_TYPE entryType){
    Entry newEntry = NULL, temp = NULL;
    /* if table is null or key is null */
    if (!head || !key[0]) return FALSE;

    if(!(newEntry = entry_create(key, val, entryType))) return FALSE;

    /* if the list is empty */
	if (!(*head)){
		(*head) = newEntry;
		return TRUE;
	}
    
    /* else go to the end of the list */
    temp = *head;
	while(temp && temp->next){
		temp = temp->next;
	}

    temp->next = newEntry;  /* add the new entry at at the end */
    return TRUE;
}


BOOL entry_get_val(const Entry entry, long *outVal){
    if(!entry) return FALSE;    /* if entry is NULL */

    (*outVal) = entry->val;
    return TRUE;
}


Entry list_get_entry(const Entry head, char *key, int numEntryTypes, ...){
    Entry temp = NULL;
    ENTRY_TYPE *entryTypes = NULL;
    int i;
    va_list types;

    if(!head || !key[0]) return NULL;   /* if list is NULL */

	va_start(types, numEntryTypes);

    /* allocate space to store all the relevant entry types */
    if(!(entryTypes = (ENTRY_TYPE *) malloc(numEntryTypes * sizeof(ENTRY_TYPE)))){
        MALLOC_ERROR();
    }

    /* get all the relevant entry types */
	for(i = 0; i < numEntryTypes ; i++){
		entryTypes[i] = va_arg(types, ENTRY_TYPE);
	}

	va_end(types);

    temp = head;
    while(temp){
        /* if the key is matching */
        if (temp->key[0] && !strcmp(temp->key, key)){
            for(i = 0 ; i < numEntryTypes ; i++){
                /* if the type is matching */
                if(temp->entryType == entryTypes[i]){
                    free(entryTypes);
                    return temp;        
                } 
            }
        } 

        temp = temp->next;
    }

    free(entryTypes);
    return NULL;
}



ENTRY_TYPE entry_get_type(const Entry entry){
    if(!entry) return NONE_ENTRY;   /* if entry is NULL */

    return entry->entryType;
}



BOOL list_delete(Entry head){
    Entry curr = NULL, temp = NULL;
    if(!head) return FALSE;

    curr = head;

    /* iterate over the list, freeing each node */
    while(curr){
        temp = curr;
        curr =curr->next;
        free(temp->key);    /* free the key */
        free(temp);
    }

    return TRUE;
}


BOOL list_update_entry(const Entry head, long updateValue, ENTRY_TYPE entryType){
    Entry temp = NULL;

    if(!head) return FALSE;
    
    if(updateValue == 0) return TRUE;   /* if the update value is zero no need to do anything */

    temp = head;
    while(temp){
        if(temp->key && temp->entryType == entryType){
            /*printf("key: %s, old value: %ld, ", temp->key, temp->val);*/
            temp->val += updateValue;
            /*printf("new value: %ld\n", temp->val);*/
        }
        temp = temp->next;
    }

    return TRUE;
}


Entry *list_get_entries_by_type(const Entry head, ENTRY_TYPE entryType, long *outCount){
    Entry temp = NULL, *entries;
    long idx = 0, count = 0;

    if(!head) return NULL;
    
    temp = head;

    /* first count how many entries of the given type are in the list */
    while(temp){
        if(temp->entryType == entryType) count++;
        temp = temp->next;
    }
    

    /* allocate space to according to the count */
    if(!(entries = (Entry *) malloc(sizeof(Entry) * count))){
        MALLOC_ERROR();
    }

    temp = head;
    while(temp){
        /* get the matching entries */
        if(temp->entryType == entryType){
            entries[idx++] = temp;
        }
        temp = temp->next;
    }

    *outCount = idx;
    return entries;
}


BOOL entry_get_key(const Entry entry, char *outKey){
    int i = 0;

    if(!entry || !outKey) return FALSE;

    /* copy the key into the given array*/
    while(entry->key[i]){
        outKey[i] = entry->key[i];
        i++;
    }
    
    outKey[i] = '\0';
    return TRUE;
}