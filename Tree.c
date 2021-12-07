//
// Created by Yann on 07/12/2021.
//

#include <malloc.h>
#include "Tree.h"

Tree* createNode(int data){
    Tree *treeTemp = (Tree*)malloc(sizeof(Tree));
    treeTemp->data = data;
    treeTemp->size = 0;
    return treeTemp;
}
void addLeef(Tree root, Tree leef){
    root.leefs[root.size] = &leef;
    root.size = root.size+1;
}