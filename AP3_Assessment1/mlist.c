#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mlist.h"

#define INIT_SIZE 512		// the initial size of hash table
#define MAX_COLLISION 20	// maximum collision before resize
#define GROWTH_TIMES 2		// growth rate of hash table

typedef struct mlistnode {
    struct mlistnode *next;
    MEntry *entry;
} MListNode;

typedef struct mlinklist {
    struct mlistnode *first;
    struct mlistnode *last;
    unsigned int size;
} MLinkList;

struct mlist {
    struct mlinklist *hash_table;
    unsigned long size;
};

int ml_verbose = 0;		// if true, print diagnostics on stderr

/**
 * ml_create - created a new mailing list
 * returns mail list if success
 * returns NULL if fail
 **/
MList* ml_create(void)
{
    MList *ml;
    
    if (ml_verbose)
        fprintf(stderr, "mlist: creating mailing list\n");
    
    if ((ml = (MList *) calloc(1, sizeof(MList))) != NULL)
        if ((ml->hash_table = (MLinkList *) calloc(INIT_SIZE, sizeof(MLinkList))) != NULL) {
            ml->size = INIT_SIZE;
            return ml;
        }
    
    return NULL;
}

/**
 * ml_add - adds a new MEntry to the list;
 * returns 1 if successful, 0 if error (calloc)
 * returns 1 if it is a duplicate
 **/
int ml_add(MList **ml, MEntry *me)
{
    MLinkList *t = (*ml)->hash_table;	// temporary reference to hash table in mail list
    MListNode *n;
    unsigned long hash = me_hash(me, (*ml)->size);
    
    if (ml_verbose)
        fprintf(stderr, "mlist: add entry\n");
    
    if (ml_lookup((*ml), me) != NULL)	// return 1 if it is a duplicate
        return 1;
    
    if ((n = (MListNode *) calloc(1, sizeof(MListNode))) == NULL)	// return 0 if error (calloc)
        return 0;
    
    n->entry = me;
    n->next = t[hash].first;		// add first node of the bucket to next reference of the new node
    t[hash].first = n;				// set the head of bucket to be the new node
    if (!(t[hash].last))			// if there is only one item in the bucket
        t[hash].last = n;			// then set head and tail of the bucket to point to the node
    ++t[hash].size;
    
    if (t[hash].size > MAX_COLLISION) {		// if the bucket size is over the limit, then re-size and re-hash
        MLinkList *old_t, *new_t;
        MListNode *cur, *freed;
        unsigned long i, old_size = (*ml)->size, new_size = (*ml)->size * GROWTH_TIMES;
        
        if (ml_verbose)
            fprintf(stderr, "mlist: re-hash table from size %lu to %lu\n", (*ml)->size, new_size);
        
        if ((new_t = (MLinkList *) calloc(new_size, sizeof(MLinkList))) != NULL) {
            old_t = (*ml)->hash_table;
            (*ml)->hash_table = new_t;
            (*ml)->size = new_size;
            for (i = 0; i < old_size; ++i) {
                for (cur = old_t[i].first; cur != NULL;) {
                    ml_add(ml, cur->entry);
                    freed = cur;
                    cur = cur->next;
                    free(freed);
                }
            }
            free((void *) old_t);
        }
    }
    
    return 1;
}

/**
 * ml_lookup - looks for MEntry in the list
 * returns matching entry if it is matched
 * returns NULL if it is not found
 **/
MEntry* ml_lookup(MList *ml, MEntry *me)
{
    MListNode *cur;
    
    if (ml_verbose)
        fprintf(stderr, "mlist: ml_lookup() entered\n");
    
    for (cur = ml->hash_table[me_hash(me, ml->size)].first; cur != NULL; cur = cur->next)
        if (me_compare(me, cur->entry) == 0)
            return cur->entry;
    return NULL;
}

/* ml_destroy - destroy the mailing list */
void ml_destroy(MList *ml)
{
    MListNode *cur, *freed;
    unsigned long i;
    for (i = 0; i < ml->size; ++i) {	// iterate to all buckets in hash table
        for (cur = ml->hash_table[i].first; cur != NULL;) {		// iterate to all nodes in a bucket
            freed = cur;				// remember the node to be destroy
            cur = cur->next;			// prepare to traverse to the next node
            me_destroy(freed->entry);	// destroy mail entry in the node
            free(freed);				// destroy the node
        }
    }
    free((void *) ml->hash_table);
    free((void *) ml);
}
