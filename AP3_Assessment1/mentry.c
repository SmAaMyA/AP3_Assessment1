#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "mentry.h"

#define MAX_LINE 3
#define SIZE_OF_LINE 50

/**
 * me_get - created a new mail entry
 * returns mail entry if success
 * returns NULL if fail
 **/
MEntry* me_get(FILE *fd)
{
    MEntry *me;
    char line[MAX_LINE][SIZE_OF_LINE], *word = NULL, *store = NULL;
    unsigned long addr_size, i, j;
    
    if (fd == NULL)
        goto error_input_fail;
    
    /* read lines of a mail */
    for (i = 0; i < MAX_LINE; ++i)
        if (fgets(line[i], sizeof(line[i]), fd) == NULL)	// terminate if the program does not read a line correctly
            goto error_eof;
    
    if ((me = (MEntry *) calloc(1, sizeof(MEntry))) == NULL)
        goto error_mem_alloc;
    
    /* add full address into a mail entry */
    for (addr_size = 0, i = 0; i < MAX_LINE; ++i)
        addr_size += strlen(line[i]);	// for memory efficiency, calculate the actual length of full address
    ++addr_size;						// space for null at the end of string
    if ((me->full_address = (char *) calloc(addr_size, sizeof(char))) == NULL)
        goto error_mem_alloc;
    for (i = 0; i < MAX_LINE; ++i)
        strcat(me->full_address, line[i]);
    
    /* add surname into a mail entry */
    if ((me->surname = (char *) calloc((strlen(line[0]) + 1), sizeof(char))) == NULL)
        goto error_mem_alloc;
    if (strstr(line[0], ",") != NULL)
        strcpy(me->surname, strtok(line[0], " ,"));
    else {
        for (word = strtok(line[0], " \n"); word != NULL; word = strtok(NULL, " \n"))
            store = word;
        strcpy(me->surname, store);
    }
    
    /* add house number into a mail entry */
    me->house_number = atoi(strtok(line[1], " ,"));
    
    /* add postcode into a mail entry */
    if ((me->postcode = (char *) calloc((strlen(line[2]) + 1), sizeof(char))) == NULL)
        goto error_mem_alloc;
    for (i = 0, j = 0; i < strlen(line[2]); ++i)
        if (isalnum(line[2][i])) {			// read only alphanumeric character from string
            me->postcode[j] = line[2][i];	// append to string
            ++j;
        }
    me->postcode[j] = '\0';		// append NULL to the end of string
    
    return me;
    
    /* error section */
error_input_fail:
    fprintf(stderr, "fail to read from input while getting a mail entry\n");
    return NULL;
error_mem_alloc:
    fprintf(stderr, "memory allocation error while getting a mail entry\n");
    return NULL;
error_eof:
    return NULL;
}

/**
 * me_hash computes a hash of the mail entry
 * returns calculated hash value mod size
 **/
unsigned long me_hash(MEntry *me, unsigned long size)
{
    unsigned long i, hashval = 0;
    char *s;
    
    if ((s = (char *) calloc(strlen(me->full_address) + 1, sizeof(char))) == NULL)
        fprintf(stderr, "memory allocation error while hashing\n");
    else {
        // concatenate surname + post code + house number into a string
        sprintf(s, "%s%s%d", me->surname, me->postcode, me->house_number);
        
        // compute hash number from a concatenated string
        for (hashval = 0, i = 0; i < strlen(s); ++i)
            hashval = s[i] + 31 * hashval;
        
        free(s);
    }
    
    return hashval % size;	// mod size to prevent index out of range
}

/* me_print prints the full address on mail entry */
void me_print(MEntry *me, FILE *fd)
{
    if (fd == NULL)
        fprintf(stderr, "NULL pointer entered while printing a mail entry\n");
    else
        fprintf(fd, "%s%s", me->full_address, (me->full_address[strlen(me->full_address) - 1] == '\n') ? "" : "\n");
}

/**
 * me_compare compares two mail entries
 * ordered by surname > post code > house number
 * returns <0 if me1<me2
 * returns 0 if me1=me2
 * returns >0 if me1>me2
 **/
int me_compare(MEntry *me1, MEntry *me2)
{
    int result;
    result = strcmp(me1->surname, me2->surname);
    if (result != 0)
        return result;
    result = strcmp(me1->postcode, me2->postcode);
    if (result != 0)
        return result;
    return me1->house_number - me2->house_number;
}

/* me_destroy destroys the mail entry */
void me_destroy(MEntry *me)
{
    free((void *) me->full_address);
    free((void *) me->surname);
    free((void *) me->postcode);
    free((void *) me);
}
