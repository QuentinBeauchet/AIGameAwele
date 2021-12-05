//-------------List of Strings---------------

struct List_s {
    char **tab;
    int length;
};
typedef struct List_s List_s;

List_s *newList_s(int size) {
    List_s *list = malloc(sizeof(List_s));
    list->tab = malloc(sizeof(char *) * size);
    list->length = 0;
    return list;
}

void freeList_s(List_s *list) {
    for (int i = 0; i < list->length; i++) {
        free(list->tab[i]);
    }
    free(list->tab);
    free(list);
}

//-------------List of Ints---------------

struct List_i {
    int *tab;
    int length;
};
typedef struct List_i List_i;

List_i *newList_i(int size) {
    List_i *list = malloc(sizeof(List_i));
    list->tab = malloc(sizeof(int) * size);
    list->length = 0;
    return list;
}

void freeList_i(List_i *list) {
    free(list->tab);
    free(list);
}

//-------------List of List of Ints---------------

struct List_ii {
    List_i **tab;
    int length;
};
typedef struct List_ii List_ii;

List_ii *newList_ii(int size) {
    List_ii *list = malloc(sizeof(List_ii));
    list->tab = malloc(sizeof(List_i *) * size);
    list->length = 0;
    return list;
}

void freeList_ii(List_ii *list) {
    for (int i = 0; i < list->length; i++) {
        freeList_i(list->tab[i]);
    }
    free(list->tab);
    free(list);
}

void freeListBlackList_ii(List_ii *list, int indexBlackList) {
    for (int i = 0; i < list->length; i++) {
        if (i != indexBlackList)
            freeList_i(list->tab[i]);
    }
    free(list->tab);
    free(list);
}