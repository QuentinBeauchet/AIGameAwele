//
// Created by Yann on 07/12/2021.
//

#include <malloc.h>
#include <stdio.h>
#include "Tree.h"

Tree* createNode(char* label,int data){
    Tree *treeTemp = (Tree*)malloc(sizeof(Tree));
    treeTemp->label = label;
    treeTemp->data = data;
    treeTemp->size = 0;
    return treeTemp;
}
void addLeef(Tree* root, Tree* leef){
    root->leefs[root->size] = leef;
    root->size = root->size+1;
}

void printTree(Tree t){
    printf("%i",t.size);
    printf("(%s -> %i [",t.label,t.data);
    for (int i = 0; i < t.size; ++i) {
        printTree(*t.leefs[i]);
    }
    printf("])");
}