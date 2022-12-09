/* 
leap.c

Implementations for leap list construction and manipulation.

Skeleton written by Grady Fitzaptrick for COMP20007 Assignment 1 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "leap.h"
#include "utils.h"

struct leapListInner;

struct leapList {
    /* IMPLEMENT: Fill in structure. */
    struct leapListInner **heads;
    int height;
    double p;
};

struct leapListInner {
    struct leapListInner **nexts;
    struct leapListInner **prevs;
    int key;
};

struct leapList *newList(int maxHeight, double p, enum problemPart part){
    /* IMPLEMENT: Set up list */
    struct leapList *newList = NULL;

    newList = (struct leapList *) malloc(sizeof(struct leapList));
    assert(newList);

    newList->height = maxHeight;
    newList->p = p;
    newList->heads = (struct leapListInner **) malloc(sizeof(struct leapListInner *) * maxHeight);
    assert(newList->heads);

    for(int i = 0; i < maxHeight; i++){
        (newList->heads)[i] = NULL;
    }

    return newList;
}

void printLevel(struct leapList *list, int level){
    if(! list){
        printf("\n");
        return;
    }
    /* IMPLEMENT (Part B): loop over list at given level, printing out each value. */
    /* Note: while additional next elements, print a space after the key. If no additional next elements, 
    print a new line and then return. */
    struct leapListInner *current = (list->heads)[level];
    while(current && (current->nexts)[level]){
        printf("%d ", current->key);
        current = (current->nexts)[level];
    }
    if(current){
        printf("%d", current->key);
    }
    printf("\n");
    return;
}

int determineLevel(int maxLevel, double p){
    int level = 1;
    while(level < maxLevel && ((double) rand() / RAND_MAX < p)){
        level++;
    }
    return level;
};

void insertKey(int key, struct leapList *list){
    /* IMPLEMENT: Insert the key into the given leap list. */
    /* Determine the level the node should be inserted at. */
    int level = determineLevel(list->height, list->p);
    /* Set up initial traversal state. */
    struct leapListInner **prevs = (struct leapListInner **) malloc(sizeof(struct leapListInner *) * (list->height));
    assert(prevs);
    /* 
    nexts will hold the current traversal point, so we insert behind them. Other choices are possible, but here we
    do not update the next pointers where they are beyond the level the node is being inserted at.
    */
    struct leapListInner **nexts = (struct leapListInner **) malloc(sizeof(struct leapListInner *) * (list->height));
    assert(nexts);

    struct leapListInner *newNode = (struct leapListInner *) malloc(sizeof(struct leapListInner));
    assert(newNode);

    int i;

    for(i = 0; i < list->height; i++){
        prevs[i] = NULL;
        if(i < level){
            /* Will be present on this level. */
            nexts[i] = (list->heads)[i];
        } else {
            /* Will not be present on this level. */
            nexts[i] = NULL;
        }
    }

    /* Unlike the express lists themselves, traversal to find the insertion points 
    only require a single pointer. */
    struct leapListInner *current = NULL;
    /* Similar to search, start from the top and move down to finer grained. */
    for(i = list->height - 1; i >= 0; i--){
        if(current == NULL){
            if((list->heads)[i] == NULL){
                continue;
            } else {
                current = (list->heads)[i];
            }

            /* Perform a check to see if the head for this level is beyond the item
            we're searching for. */
            if(current->key > key){
                /* Use NULL to denote that we're at the head of the list. */
                current = NULL;
            }
        }
        while(current && (current->nexts)[i] && ((current->nexts)[i])->key < key){
            current = (current->nexts)[i];
        }
        if(level > i){
            /* Insert to head if before first element, or after first element otherwise. */
            if(!current){
                nexts[i] = (list->heads)[i];
            } else {
                nexts[i] = (current->nexts)[i];
            }
            
            prevs[i] = current;
            if(!current){
                (list->heads)[i] = newNode;
            } else {
                (current->nexts)[i] = newNode;
            }
            
            if(nexts[i]){
                nexts[i]->prevs[i] = newNode;
            }
        }
    }
    /* See if any additional connections need to be made. */
    for(i = level - 1; i >= 0; i--){
        if(prevs[i] == NULL){
            list->heads[i] = newNode;
        }
    }
    newNode->nexts = nexts;
    newNode->prevs = prevs;
    newNode->key = key;
}

/* Queries the leap list for the given key and places the result in the solution structure. */
int findKey(int key, struct leapList *list, enum problemPart part, struct solution *solution){
    int found = UNKNOWN;
    int element = key;
    int baseAccesses = 0;
    int requiredAccesses = 0;
    assert(solution);
    /* IMPLEMENT: Find the given key in the leap list. */
    struct leapListInner *current = NULL;
    struct leapListInner *lastChecked = NULL;
    int level = list->height;
    while(level > 0 && current == NULL){
        level--;
        current = list->heads[level];
    }
    if(current == NULL){
        found = NOTFOUND;
    } else {
        while(found == UNKNOWN){
            if(! lastChecked){
                /* If there is a last-checked, we've already performed the comparison once,
                rather than storing this in another cache, it saves code to simply check for
                this case. */
                requiredAccesses++;
            }
            /* Because we have prev pointers, easier logic to move immediately. */
            if(current->key < key){
                while(lastChecked && current->nexts[level] == lastChecked && level >= 0){
                    /* Already visited the next node, so solution can't be at this pointer. */
                    if(level == 0){
                        /* Not at level 0, so absent. */
                        found = NOTFOUND;
                        break;
                    } else {
                        level--;
                    }
                }
                if(found == NOTFOUND){
                    break;
                }
                /* If next is NULL, continue dropping level as much as possible until we have a next. */
                while(level > 0 && current->nexts[level] == NULL){
                    level--;
                }
                current = current->nexts[level];
                /* If we didn't find a next value, the key is not in the dictionary so give up. */
                if(! current){
                    found = NOTFOUND;
                    break;
                }
                /* Reset last checked - not critical but affects some special cases to do with 
                the heads. */
                lastChecked = NULL;
            } else if(current->key == key){
                found = FOUND;
                break;
            } else {
                /* current->key < key, so move to previous. */
                lastChecked = current;
                if(current->prevs[level] == NULL){
                    /* At head of list, so move down heads until a differing node is found. */
                    while(current == lastChecked && level > 0){
                        level--;
                        current = list->heads[level];
                    }
                    if(level == 0 && current == lastChecked){
                        found = NOTFOUND;
                        break;
                    }
                    /* Restarting from the head, so assume no cached comparison. */
                    lastChecked = NULL;
                } else {
                    /* In middle of list. */
                    lastChecked = current;
                    current = current->prevs[level];
                }
            }
        }
    }
    
    /* Traverse to find value at base level to find how many steps would be required with
    a regular sorted linked list. */
    current = list->heads[0];
    while(current && current->key < key){
        baseAccesses++;
        current = current->nexts[0];
    }
    /* Handle final case where last value is compared against. */
    if(current){
        baseAccesses++;
    }
    

    /* Insert result into solution. */
    (solution->queries)++;
    solution->queryResults = (int *) realloc(solution->queryResults, sizeof(int) * solution->queries);
    assert(solution->queryResults);
    (solution->queryResults)[solution->queries - 1] = found;
    solution->queryElements = (int *) realloc(solution->queryElements, sizeof(int) * solution->queries);
    assert(solution->queryElements);
    solution->queryElements[solution->queries - 1] = element;
    solution->baseAccesses = (int *) realloc(solution->baseAccesses, sizeof(int) * solution->queries);
    assert(solution->baseAccesses);
    solution->baseAccesses[solution->queries - 1] = baseAccesses;
    solution->requiredAccesses = (int *) realloc(solution->requiredAccesses, sizeof(int) * solution->queries);
    assert(solution->requiredAccesses);
    solution->requiredAccesses[solution->queries - 1] = requiredAccesses;
    return found;
}

void deleteKey(int key, struct leapList *list, enum problemPart part){
    /* IMPLEMENT: Remove the given key from the leap list. */
    struct leapListInner *current = NULL;
    int level = list->height - 1;
    while(level >= 0){
        if(current == NULL){
            current = list->heads[level];
            /* Find first head. */
            if(! current){
                level--;
                continue;
            }
            if(current->key > key){
                /* Head is past key, start from head at lower level. */
                current = NULL;
                level--;
                continue;
            }
        }
        if(current->key == key){
            /* Found item to delete. */
            break;
        }
        while(current->nexts[level] && current->nexts[level]->key < key){
            /* Move along level as far as we can. */
            current = current->nexts[level];
        }
        if(current->nexts[level] && current->nexts[level]->key == key){
            current = current->nexts[level];
            break;
        } else {
            /* Try one more level down. */
            level--;
        }
    }
    if((! current) || current->key != key){
        fprintf(stderr, "Failed to find key %d in list\n", key);
        return;
    }
    for(level = 0; level < list->height; level++){
        /* Connect through deleted node. */
        if(current->prevs[level]){
            current->prevs[level]->nexts[level] = current->nexts[level];
        } else if(list->heads[level] == current){
            /* Update to head at this level as this was the first element. */
            list->heads[level] = current->nexts[level];
        }
        if(current->nexts[level]){
            current->nexts[level]->prevs[level] = current->prevs[level];
        }
    }
    if(current->nexts){
        free(current->nexts);
    }
    if(current->prevs){
        free(current->prevs);
    }
    free(current);
}

void freeList(struct leapList *list){
    /* IMPLEMENT: Free all memory used by the list. */
    /* Traverse at base level to free all nodes. */
    if(! list){
        return;
    }
    struct leapListInner *current = list->heads[0];
    struct leapListInner *next = NULL;
    while(current){
        next = current->nexts[0];
        if(current->prevs){
            free(current->prevs);
        }
        if(current->nexts){
            free(current->nexts);
        }
        free(current);
        current = next;
    }
    if(list->heads){
        free(list->heads);
    }
    free(list);
}

void freeSolution(struct solution *solution){
    if(! solution){
        return;
    }

    freeList(solution->list);
    if(solution->queries > 0){
        free(solution->queryResults);
        free(solution->queryElements);
        free(solution->baseAccesses);
        free(solution->requiredAccesses);
    }
    free(solution);
}

