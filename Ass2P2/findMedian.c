#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "234tree.h"

int main(int argc, char **argv){
    int value;

    struct tree *tree = NULL;

    while(scanf("%d", &value) > 0){
        tree = insertTree(tree, value);
    }

    // printTree(tree, 0);

    findMedian(tree);

    return 0;
}


