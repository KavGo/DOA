/* 
234tree.h

Definitions and prototypes for a 2-3-4 tree.

Original solution written by Grady Fitzpatrick for 2022 COMP20007 Workshops
    with modifications for skeleton.
*/
struct tree;

/* Inserts the given value into the tree. */
struct tree *insertTree(struct tree *tree, int value);

/* Prints the context of the tree level by level, 
beginning from the root as the given level. */
void printTree(struct tree *tree, int level);

/* Prints the median value of all values of the tree
and its level, beginning from the root as level 0. */
void findMedian(struct tree *tree);

