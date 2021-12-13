//
// Created by Yann on 07/12/2021.
//

#ifndef AIGAMEAWELE_TREE_H
#define AIGAMEAWELE_TREE_H

#endif //AIGAMEAWELE_TREE_H

typedef struct Tree {
    char* label;
    int data;
    struct Tree *leefs[32];
    int size;
} Tree;

Tree* createNode(char* label,int data);
void addLeef(Tree* root, Tree* leef);
void printTree(Tree t);